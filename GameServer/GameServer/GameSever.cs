using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

using System.Net;
using System.Net.Sockets;

using System.Data.SQLite;

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

        //Database
        private static SQLiteConnection dbConnection;

        public void StartServer(int portNumber)
        {
            try
            {
                Console.WriteLine("Server Started");
                serverPortNumber_ = portNumber;

                //Create a TCPListener to Accept client connections
                //TCPlistener_ = new TcpListener(IPAddress.Any, serverPortNumber_);
                //TCPlistener_.Start();
                //Console.WriteLine("Computer Name " + Dns.GetHostName());
                //IPHostEntry ipHostInfo = Dns.GetHostEntry("localhost");

                IPHostEntry ipHostInfo = Dns.Resolve(Dns.GetHostName());

                Console.WriteLine(ipHostInfo.AddressList[0]);

                IPAddress ipAddress = ipHostInfo.AddressList[0];
                IPEndPoint localEndPoint = new IPEndPoint(ipAddress, serverPortNumber_);

                listener_ = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                listener_.Bind(localEndPoint);

                //Create the Client Database

                //SQLiteConnection.CreateFile("ClientDatabase.sqlite");

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
                        string sql = "create table clients (username varchar(20), password varchar(20))";

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
                //Environment.Exit(se.ErrorCode);
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

                    //Once a client is found start a thread to handle the client
                    //Thread newThread = new Thread(new ParameterizedThreadStart(HandleClient));
                    //newThread.Start(newClient);

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

            /*
			IPEndPoint localIPEndpoint = new IPEndPoint (IPAddress.Any, 8082);

			//Create a udp socket for the new client
			UdpClient udpCleint = new UdpClient(localIPEndpoint);
			udpCleint.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);

			UdpState udpStateObj = new UdpState ();
			udpStateObj.socket = udpCleint;

			udpCleint.Client.Bind (new IPEndPoint(IPAddress.Any, 8082));

			//Begin UDP Receive
			udpCleint.BeginReceive(new AsyncCallback(RecvCallBack), udpStateObj);
			*/

            Socket udpListener_;
            udpListener_ = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            udpListener_.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);

            IPEndPoint endPoint = new IPEndPoint(IPAddress.Any, 8081);

            udpListener_.Bind(endPoint);

            IPEndPoint sender = new IPEndPoint(IPAddress.Any, 8081);
            EndPoint tempRemote = (EndPoint)sender;

            UdpState udpStateObj = new UdpState();
            udpStateObj.socket = udpListener_;
            udpStateObj.endPoint = (IPEndPoint)endPoint;
            udpStateObj.tempRemote = tempRemote;

            udpListener_.BeginReceiveFrom(udpStateObj.buffer, 0, udpStateObj.buffer.Length, 0, ref tempRemote, new AsyncCallback(RecvCallBack), udpStateObj);

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
                StringBuilder sb = new StringBuilder();
                sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));

                handler.BeginReceive(state.buffer, 0, StateObjects.BufferSize, 0, new AsyncCallback(ReadCallBack), state);
                if (sb.Length > 1)
                {
                    content = sb.ToString();
                    char[] delimitors = { ':' };
                    String[] parts = content.Split(delimitors);

                    Console.WriteLine("read {0} bytes from the socket. \nData : {1}", content.Length, content);

                    //if the recieved string starts with reg: a new client is requesting to be registered onto the server
                    //so the server will then add the new client to a sqlite database storing username/id, password, and socket information
                    //Handle client registration..
                    if (parts[0] == "reg")
                    {
                        //Open the database to add content
                        dbConnection.Open();

                        int exists = 0;

                        //First a query needs to be made to the database to see if the username is avaible
                        string query = "SELECT COUNT(*) FROM clients WHERE username = @userName";
                        using (SQLiteCommand cmd = new SQLiteCommand(query, dbConnection))
                        {
                            cmd.Parameters.AddWithValue("@userName", parts[1]);
                            exists = Convert.ToInt32(cmd.ExecuteScalar());
                        }
                        if (exists > 0)
                        {
                            //Cannot reg client with this user name
                            Send(handler, "Sorry! Username " + parts[1] + " already exists. :(");
                        }
                        else
                        {
                            //Username isn't taken, register the client
                            Client newClient = new Client();

                            newClient.username = parts[1];

                            string sql = "INSERT INTO clients (username, password) VALUES (@username, @password)";
                            SQLiteCommand command = new SQLiteCommand(sql, dbConnection);
                            command.Parameters.AddWithValue("@username", newClient.username);
                            command.Parameters.AddWithValue("@password", parts[2]);

                            command.ExecuteNonQuery();

                            Send(handler, "Welcome " + parts[1] + " to the best multiplayer platformer game server EVER!");
                        }

                        string sql1 = "select * from clients";
                        SQLiteCommand command1 = new SQLiteCommand(sql1, dbConnection);

                        SQLiteDataReader reader = command1.ExecuteReader();
                        while (reader.Read())
                        {
                            Console.WriteLine("Username: " + reader["username"] + " Password: " + reader["password"]);
                        }

                        //Close the database
                        dbConnection.Close();
                    }
                }

            }
            else
            {
                //handler.BeginReceive(state.buffer, 0, StateObjects.BufferSize, 0, new AsyncCallback(ReadCallBack), state);
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

            int recvBytes = s.socket.EndReceive(ar);

            StringBuilder sb = new StringBuilder();
            sb.Append(Encoding.ASCII.GetString(s.buffer, 0, recvBytes));
            //s.buffer = recvBytes;

            string receiveString = Encoding.ASCII.GetString(s.buffer);

            Console.WriteLine("Recieved UDP: {0}", receiveString);

            SendUDPMessage(s, "Hello UDP Client");
            // The message needs to be handled
            udpMessageReceived_ = true;

            s.socket.BeginReceive(s.buffer, 0, s.buffer.Length, 0, new AsyncCallback(RecvCallBack), s);
        }

        public static void ReceiveMessages()
        {
            // Receive message and write to the console
            UdpState s = new UdpState();

            Console.WriteLine("Listening for udp Messages...");
            s.socket.BeginReceive(s.buffer, 0, s.buffer.Length, 0, new AsyncCallback(RecvCallBack), s.socket);

            while (!udpMessageReceived_)
            {
                Thread.Sleep(100);
            }

        }

        public static void UDPSendCallBack(IAsyncResult ar)
        {
            UdpState u = (UdpState)ar.AsyncState;

            //Console.WriteLine("Number of udp bytes sent: {0}", u.socket.EndSend(ar));
            udpMessageSent_ = true;

        }

        public static void SendUDPMessage(UdpState state, string message)
        {
            try
            {
                Byte[] sendBytes = Encoding.ASCII.GetBytes(message);

                Console.WriteLine("Trying to send udp message");

                // Send the message
                state.socket.BeginSend(sendBytes, 0, sendBytes.Length, 0, new AsyncCallback(UDPSendCallBack), state);

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

