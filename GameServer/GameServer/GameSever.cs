using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Data.SQLite;
using System.Collections;
using Google.Protobuf;
using System.Collections.Generic;

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
        private static Thread handleUDPMsgThread_;

        private static IPEndPoint broadcastEP = new IPEndPoint(IPAddress.Broadcast, 8082);

        //Queues
        private static Queue UDPmessageRecvQueue_ = new Queue();
        private static Queue UDPmessageSendQueue_ = new Queue();

        //Database
        private static SQLiteConnection dbConnection;

        //Clients List/Array for registered clients
        const int MAX_CLIENTS = 6;
        private static List<Client> registeredClients = new List<Client>();

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

                handleUDPMsgThread_ = new Thread(new ThreadStart(handleUDPRecvMessage));
                handleUDPMsgThread_.Start();

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
                        //Send reg fail
                        GameDataTCP.NewPlayerReg newRegData = new GameDataTCP.NewPlayerReg();
                        GameDataTCP.DataMessage DataMsg = new GameDataTCP.DataMessage();
                        newRegData.Status = "Failed";

                        DataMsg.NewPlayerReg.Insert(0, newRegData);

                        byte[] buffer = DataMsg.ToByteArray();

                        Send(handler, buffer);
                    }
                    else
                    {
                        //Username isn't taken, register the client
                        Client newClient = new Client();

                        newClient.socket = handler;
                        newClient.username = protoData.Register.Username.ToString();

                        string sql = "INSERT INTO clients (username, password) VALUES (@username, @password)";
                        SQLiteCommand command = new SQLiteCommand(sql, dbConnection);
                        command.Parameters.AddWithValue("@username", newClient.username);
                        command.Parameters.AddWithValue("@password", protoData.Register.Password.ToString());

                        command.ExecuteNonQuery();

                        dbConnection.Close();

                        //Add Client to Clients Array
                        registeredClients.Add(newClient);

                        //Send reg Success
                        GameDataTCP.NewPlayerReg newRegData = new GameDataTCP.NewPlayerReg();
                        GameDataTCP.DataMessage DataMsg = new GameDataTCP.DataMessage();
                        newRegData.Username = newClient.username;
                        newRegData.Status = "Success";

                        DataMsg.NewPlayerReg.Insert(0, newRegData);

                        //Relay the new reg to all currently registered players
                        //maybe send everything then check on client side if username is equal to player username
                        //if yes dont add enemy, no add one
                        GameDataTCP.NewPlayerReg[] oldClientData = new GameDataTCP.NewPlayerReg[registeredClients.Count];
                        if (registeredClients.Count > 1)
                        {
                            int idx = 1;
                            for (int i = 0; i <= registeredClients.Count - 1; ++i)
                            {
                                oldClientData[i] = new GameDataTCP.NewPlayerReg();
                                string tempUsername = registeredClients[i].username;
                                if (registeredClients[i].username == newClient.username)
                                {
                                    //Send the information about already registered clients to the new client
                                    foreach (var client in registeredClients)
                                    {
                                        //Make sure to not include the new client in the oldclient message
                                        if(client.username != newClient.username)
                                        {
                                            oldClientData[idx - 1].Username = client.username;
                                            oldClientData[idx - 1].Status = "oldClients";

                                            DataMsg.NewPlayerReg.Insert(idx, oldClientData[idx - 1]);

                                            idx++;
                                        }
                                   
                                    }

                                }
                                else
                                {
                                    //Send New Reg to all current Clients
                                    GameDataTCP.NewPlayerReg newClientData = new GameDataTCP.NewPlayerReg();
                                    GameDataTCP.DataMessage clientMsg = new GameDataTCP.DataMessage();
                                    newClientData.Username = newClient.username;
                                    newClientData.Status = "NewReg";

                                    clientMsg.NewPlayerReg.Insert(0, newClientData);

                                    byte[] newClientRegBuffer = clientMsg.ToByteArray();

                                    Send(registeredClients[i].socket, newClientRegBuffer);
                                }
                            }

                        }

                        //Send the Success reg message containing info and all previously registered clients
                        byte[] buffer = DataMsg.ToByteArray();
                        Send(handler, buffer);

                    }

                }
                else if(protoData.NewPlayerReg.Count == 1)
                {
                    string username = protoData.NewPlayerReg[0].Username;
                    string status = protoData.NewPlayerReg[0].Status;

                    //Check to see if a client has requested to be removed from the database/server
                    if(status == "UnReg")
                    {
                        dbConnection.Open();

                        string removeString = "DELETE FROM clients WHERE username='" + username + "'";
                        SQLiteCommand removeCMD = new SQLiteCommand(removeString, dbConnection);

                        removeCMD.ExecuteNonQuery();

                        dbConnection.Close();

                        //Remove from Client List
                        for(int i = 0; i < registeredClients.Count; i++)
                        {
                            if(registeredClients[i].username == username)
                            {
                                registeredClients.Remove(registeredClients[i]); 
                            }
                        }
                    }

                }

                //Print the registed clients ... FOR DEBUGGING
                dbConnection.Open();

                string sql1 = "SELECT * FROM clients";
                SQLiteCommand command1 = new SQLiteCommand(sql1, dbConnection);

                SQLiteDataReader reader = command1.ExecuteReader();
                while (reader.Read())
                {
                    Console.WriteLine("DATABASE: Username: " + reader["username"] + " Password: " + reader["password"]);
                }

                //Close the database
                dbConnection.Close();

            }
        }

        public static void Send(Socket handler, byte[] data)
        {
            //Begin sending data to the remote device
            handler.BeginSend(data, 0, data.Length, 0, new AsyncCallback(SendCallBack), handler);
        }

        public static void SendCallBack(IAsyncResult ar)
        {
            try
            {
                //Retrieve the socket from the stateObject.
                Socket handler = (Socket)ar.AsyncState;

                //Complete sending the data to the remote device
                int bytesSent = handler.EndSend(ar);
                Console.WriteLine("Sent {0} bytes to client {1}", bytesSent, handler.LocalEndPoint);

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

            //UDPmessageQueue_.Enqueue(recvBytes);
            addMsgToUDPGueue(recvBytes);

            byte[] response = getMsgFromUDPSendQueue();
            if (response.Length > 0)
            {
                SendUDPMessage(response, s.socket, broadcastEP);
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

            //Console.WriteLine("Number of udp bytes sent: {0}", u.EndSend(ar));
            udpMessageSent_ = true;

        }

        public static void SendUDPMessage(byte[] message, UdpClient client, IPEndPoint endPoint)
        {
            try
            {
                // Send the message
                client.BeginSend(message, message.Length, broadcastEP, new AsyncCallback(UDPSendCallBack), client);

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

        public static void addMsgToUDPGueue(byte[] msg)
        {
            Mutex m = new Mutex();
            m.WaitOne();

            UDPmessageRecvQueue_.Enqueue(msg);

            m.ReleaseMutex();
        }

        public static byte[] getUDPMsgFromQueue()
        {
            if (UDPmessageRecvQueue_.Count > 0)
            {
                Mutex m = new Mutex();

                m.WaitOne();

                byte[] msg = (byte[])UDPmessageRecvQueue_.Peek();

                UDPmessageRecvQueue_.Dequeue();

                m.ReleaseMutex();

                return msg;
            }
            byte[] empty = new byte[0];
            return empty;
        }

        //Handle the udp messages from stored in the message queue
        public static void handleUDPRecvMessage()
        {
            while (true)
            {
                byte[] msg = getUDPMsgFromQueue();
                if (msg.Length > 0)
                {
                    //Using Protobuf, deserialize the recieved bytes / string so we can now use the protobuf generated functions
                    //to access the data(protoData.Register.Username)
                    GameDataUDP.DataMessage protoData = new GameDataUDP.DataMessage();
                    protoData = GameDataUDP.DataMessage.Parser.ParseFrom(msg);

                    //Check the Datamsg for a Key Press Message
                    if(protoData.KeyPress != null)
                    {
                        Console.WriteLine("Recieved UDP: {0}", protoData.KeyPress.ToString());
                        string username_ = protoData.KeyPress.Username.ToString();
                        //Create a Response Message
                        GameDataUDP.DataMessage responseData = new GameDataUDP.DataMessage();
                        GameDataUDP.PlayerVelcityUpdate vel = new GameDataUDP.PlayerVelcityUpdate();

                        vel.Username = username_;
                        vel.YPos = 10;

                        responseData.PlayerVelocityUpdate = vel;

                        // Give the Player 10 Velocity in the Y axis
                        //responseData.PlayerVelocityUpdate.Username = username_;
                        //responseData.PlayerVelocityUpdate.YPos = 10;

                        byte[] messageBuffer = responseData.ToByteArray();
                        //Add to send Queue
                        addMsgToUDPSendQueue(messageBuffer);
                    }
                    //Check for Velocity Updates
                    if (protoData.PlayerPosUpdate != null)
                    {
                        GameDataUDP.PlayerPositionUpdate responsePosData = new GameDataUDP.PlayerPositionUpdate();

                        float xPos = protoData.PlayerPosUpdate.XPos;
                        float yPos = protoData.PlayerPosUpdate.YPos;
                        string username = protoData.PlayerPosUpdate.Username;

                        responsePosData.XPos = xPos;
                        responsePosData.YPos = yPos;
                        responsePosData.Username = username;

                        GameDataUDP.DataMessage data = new GameDataUDP.DataMessage();

                        data.PlayerPosUpdate = responsePosData;

                        Console.WriteLine("Recieved UDP: {0}", data.PlayerPosUpdate.ToString());

                        //Broadcast the position Update to all players
                        byte[] messageBuffer = data.ToByteArray();
                        //Add to send Queue
                        addMsgToUDPSendQueue(messageBuffer);
                    }
                    
                }
            }
        }

        public static void addMsgToUDPSendQueue(byte[] msg)
        {
            Mutex m = new Mutex();

            m.WaitOne();

            UDPmessageSendQueue_.Enqueue(msg);

            m.ReleaseMutex();
        }

        public static byte[] getMsgFromUDPSendQueue()
        {
            if (UDPmessageSendQueue_.Count > 0)
            {
                Mutex m = new Mutex();

                m.WaitOne();

                byte[] msg = (byte[])UDPmessageSendQueue_.Peek();

                UDPmessageSendQueue_.Dequeue();

                m.ReleaseMutex();

                return msg;
            }
            byte[] empty = new byte[0];
            return empty;
        }

    }
}

