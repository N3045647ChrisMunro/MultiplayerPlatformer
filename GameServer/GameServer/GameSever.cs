using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Data.SQLite;
using System.Collections;
using Google.Protobuf;

namespace GameServer
{
    class GameSever
    {

        //Private Server/TCP variables
        private Socket listener_;
        //private Socket udpListener_;
        //private TcpListener TCPlistener_;
        private static int serverPortNumber_;
        private Boolean isRunning_;

        private const int BUFFSIZE = 32;
        //private byte[] recvBuffer_ = new byte[BUFFSIZE];

        //StateObjects stateObject = new StateObjects();
        public static ManualResetEvent allDone = new ManualResetEvent(false);

        //Private UDP variables
        private static bool udpMessageReceived_ = false;
        private static bool udpMessageSent_ = false;

        private static IPEndPoint broadcastEP = new IPEndPoint(IPAddress.Broadcast, 8082);

        //Queues
        private static Queue UDPmessageQueue_ = new Queue();

        //Database
        private static SQLiteConnection dbConnection;

        public void StartServer(int portNumber)
        {
            try
            {
                Console.WriteLine("Server Started");
                serverPortNumber_ = portNumber;

                //Create a TCPListener to Accept client connections
                IPHostEntry ipHostInfo = Dns.Resolve(Dns.GetHostName());

                Console.WriteLine(ipHostInfo.AddressList[0]);
                IPAddress ipAddress = ipHostInfo.AddressList[0];
                IPEndPoint localEndPoint = new IPEndPoint(ipAddress, serverPortNumber_);

                listener_ = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                listener_.Bind(localEndPoint);

                //Create the Client Database
                SQLiteConnection.CreateFile("ClientDatabase.sqlite");

                //Open the database file
                dbConnection = new SQLiteConnection("Data Source=ClientDatabase.sqlite; Version=3;");
                dbConnection.Open();

                //Check to see if a 'clients' table already exists, if one doesn't create one
                SQLiteCommand cmd = dbConnection.CreateCommand();
                cmd.CommandType = System.Data.CommandType.Text;
                cmd.Connection = dbConnection;
                cmd.CommandText = "SELECT * FROM sqlite_master WHERE type = 'table' AND name = @name";
                cmd.Parameters.AddWithValue("@name", "clients");
                //If the reader actually reads data that means the table already exists.
                using (SQLiteDataReader sqlReader = cmd.ExecuteReader())
                {
                    if (sqlReader.Read())
                    {
                        //Do nothing ,, table exists
                        Console.WriteLine("Table Exists");
                    }
                    else
                    {
                        //Table does not exist
                        //Create a clients table which will hold usernames and passwords...
                        //TODO look into password secruity. 
                        string sql = "CREATE TABLE clients (username VARCHAR(20), password VARCHAR(20))";

                        SQLiteCommand command = new SQLiteCommand(sql, dbConnection);

                        command.ExecuteNonQuery();
                    }
                }
                //Close the connection after excuting the command
                dbConnection.Close();

                //Start the accept loop to handle new connected clients
                isRunning_ = true;
                AcceptLoop();
            }
            catch (SocketException se)
            {
                Console.WriteLine(se.ErrorCode + ": " + se.Message);
                Console.WriteLine("Source: " + se.Source);
                Console.WriteLine("Message: " + se.Message);
                Environment.Exit(se.ErrorCode);
            }

        }

        public void AcceptLoop()
        {
            //While the server is Active, Accept incoming Client Connections
            while (isRunning_)
            {
                try
                {
                    // Set the event to a nonsignaled state.
                    allDone.Reset();

                    Console.WriteLine("Listening For Connections");
                    //TcpClient newClient = TCPlistener_.AcceptTcpClient();
                    listener_.Listen(100);

                    listener_.BeginAccept(new AsyncCallback(AcceptCallBack), listener_);

                    //Wait until a connection is made before continuing.
                    allDone.WaitOne();

                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                }

            }
        }

        //Accept a TCP socket
        //Create a UDP client
        public static void AcceptCallBack(IAsyncResult ar)
        {
            //Signal the main thread to continue
            allDone.Set();

            Console.WriteLine("Handling Client...");

            Socket listener = (Socket)ar.AsyncState;
            Socket handler = listener.EndAccept(ar);

            StateObjects state = new StateObjects();
            state.workSocket = handler;
            handler.BeginReceive(state.buffer, 0, StateObjects.BufferSize, 0, new AsyncCallback(ReadCallBack), state);

            //Create a udp socket for the new client
            IPEndPoint localIPEndpoint = new IPEndPoint(IPAddress.Any, 8081);
            UdpClient udpClient = new UdpClient();
            udpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);

            udpClient.Client.Bind(localIPEndpoint);
            udpClient.Client.EnableBroadcast = true;

            UdpState udpStateObj = new UdpState();
            udpStateObj.socket = udpClient;

            //Begin UDP Receive
            udpClient.BeginReceive(new AsyncCallback(RecvCallBack), udpStateObj);
        }

        //Read Data from a TCP socket
        public static void ReadCallBack(IAsyncResult ar)
        {
            String content = String.Empty;

            //Retrieve the state object and the handler socket
            //from the asynchronous state object
            StateObjects state = (StateObjects)ar.AsyncState;
            Socket handler = state.workSocket;

            SocketError errorCode;
            int bytesRead = handler.EndReceive(ar, out errorCode);

            if (bytesRead > 0)
            {
                //Allow Receiving to Happen again
                handler.BeginReceive(state.buffer, 0, StateObjects.BufferSize, 0, new AsyncCallback(ReadCallBack), state);

                //Copy the contents of the state.buffer to a newBuffer, so that the content of the new buffer
                //is filled with actual data and not empty "0's" / no data
                byte[] newBuffer = new byte[bytesRead];
                Array.Copy(state.buffer, 0, newBuffer, 0, bytesRead);

                //Using Protobuf, deserialize the recieved bytes/string so we can now use the protobuf generated functions
                //to access the data (protoData.Register.Username)
                GameDataTCP.DataMessage protoData = new GameDataTCP.DataMessage();
                protoData = GameDataTCP.DataMessage.Parser.ParseFrom(newBuffer);

                //Console.WriteLine("Proto Serialized String: Username: {0}, Password: {1}", testProto.Register.Username.ToString(), testProto.Register.Password.ToString());

                if (protoData.Register != null)
                {
                    //Open the database to add content
                    dbConnection.Open();

                    int exists = 0;

                    //First a query needs to be made to the database to see if the username is avaible
                    string query = "SELECT COUNT(*) FROM clients WHERE username = @userName";
                    using (SQLiteCommand cmd = new SQLiteCommand(query, dbConnection))
                    {
                        cmd.Parameters.AddWithValue("@userName", protoData.Register.Username.ToString());
                        exists = Convert.ToInt32(cmd.ExecuteScalar());
                    }
                    if (exists > 0)
                    {
                        //Cannot reg client with this user name
                        Send(handler, "Sorry! Username " + protoData.Register.Username.ToString() + " already exists. :(");
                    }
                    else
                    {
                        //Username isn't taken, register the client
                        Client newClient = new Client();

                        newClient.username = protoData.Register.Username.ToString();

                        string sql = "INSERT INTO clients (username, password) VALUES (@username, @password)";
                        SQLiteCommand command = new SQLiteCommand(sql, dbConnection);
                        command.Parameters.AddWithValue("@username", newClient.username);
                        command.Parameters.AddWithValue("@password", protoData.Register.Password.ToString());

                        command.ExecuteNonQuery();

                        Send(handler, "reg:OK");
                    }

                    string sql1 = "select * from clients";
                    SQLiteCommand command1 = new SQLiteCommand(sql1, dbConnection);

                    SQLiteDataReader reader = command1.ExecuteReader();
                    while (reader.Read())
                    {
                        Console.WriteLine("DATABASE: Username: " + reader["username"] + " Password: " + reader["password"]);
                    }

                    //Close the database
                    dbConnection.Close();
                }

                StringBuilder sb = new StringBuilder();
                sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));

                if (sb.Length > 1)
                {
                    content = sb.ToString();

                    char[] delimitors = { ':' };
                    String[] parts = content.Split(delimitors);

                    Console.WriteLine("read {0} bytes from the socket. \nData : {1}", content.Length, content);
                }

            }
        }

        public static void Send(Socket handler, String data)
        {
            byte[] byteData = Encoding.ASCII.GetBytes(data);

            //Begin sending data to the remote device
            handler.BeginSend(byteData, 0, byteData.Length, 0, new AsyncCallback(SendCallBack), handler);
        }

        public static void SendCallBack(IAsyncResult ar)
        {
            try
            {
                //Retrieve the socket from the stateObject.
                Socket handler = (Socket)ar.AsyncState;

                //Complete sending the data to the remote device
                int bytesSent = handler.EndSend(ar);
                Console.WriteLine("Sent {0} bytes to client", bytesSent);

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
        // ######################
        // UDP
        // ######################

        public static void RecvCallBack(IAsyncResult ar)
        {
            UdpState s = (UdpState)ar.AsyncState;

            byte[] recvBytes = s.socket.EndReceive(ar, ref s.endPoint);
            s.buffer = recvBytes;

            UDPmessageQueue_.Enqueue(recvBytes);

            Console.WriteLine("Udp message Queue: {0}", UDPmessageQueue_.Count);

            if (UDPmessageQueue_.Count > 0)
            {
                //Using Protobuf, deserialize the recieved bytes/string so we can now use the protobuf generated functions
                //to access the data (protoData.Register.Username)
                GameDataUDP.DataMessage protoData = new GameDataUDP.DataMessage();
                protoData = GameDataUDP.DataMessage.Parser.ParseFrom((byte[])UDPmessageQueue_.Dequeue());

                if (protoData.PositionUpdate != null)
                {
                    float a = protoData.PositionUpdate.XPos;
                    Console.WriteLine("Recieved UDP: {0}", protoData.PositionUpdate.ToString());

                    //Broadcast the position Update to all players
                    byte[] messageBuffer = protoData.ToByteArray();
                    SendUDPMessage(messageBuffer, s.socket, broadcastEP);
                }

                
            }
            // The message needs to be handled
            udpMessageReceived_ = true;

            s.socket.BeginReceive(new AsyncCallback(RecvCallBack), s);

        }

        public static void ReceiveMessages()
        {
            // Receive message and write to the console
            UdpState s = new UdpState();

            Console.WriteLine("Listening for udp Messages...");
            s.socket.BeginReceive(new AsyncCallback(RecvCallBack), s);

            while (!udpMessageReceived_)
            {
                Thread.Sleep(100);
            }



        }

        public static void UDPSendCallBack(IAsyncResult ar)
        {
            UdpClient u = (UdpClient)ar.AsyncState;

            Console.WriteLine("Number of udp bytes sent: {0}", u.EndSend(ar));
            udpMessageSent_ = true;

        }

        public static void SendUDPMessage(byte[] message, UdpClient client, IPEndPoint endPoint)
        {
            try
            {
                // Send the message
                client.BeginSend(message, message.Length, endPoint, new AsyncCallback(UDPSendCallBack), client);

                while (!udpMessageSent_)
                {
                    Thread.Sleep(100);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

        }

    }
}

