using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

using System.Net;
using System.Net.Sockets;

namespace GameServer
{
    class GameSever
    {

        //Private Server/TCP variables
        private Socket listener_;
        private TcpListener TCPlistener_;
        private int serverPortNumber_;
        private Boolean isRunning_;

        private const int BUFFSIZE = 32;
        private byte[] recvBuffer_ = new byte[BUFFSIZE];

        StateObject stateObject = new StateObject();
        public static ManualResetEvent allDone = new ManualResetEvent(false);

        public void StartServer(int portNumber)
        {
            try
            {
                Console.WriteLine("Server Started");
                serverPortNumber_ = portNumber;


                //Create a TCPListener to Accept client connections
                //TCPlistener_ = new TcpListener(IPAddress.Any, serverPortNumber_);
                //TCPlistener_.Start();
                IPHostEntry ipHostInfo = Dns.Resolve(Dns.GetHostName());
                IPAddress ipAddress = ipHostInfo.AddressList[0];
                IPEndPoint localEndPoint = new IPEndPoint(ipAddress, serverPortNumber_);

                listener_ = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                listener_.Bind(localEndPoint);
                

                isRunning_ = true;
                AcceptLoop();
            }
            catch(SocketException se)
            {
                Console.WriteLine(se.ErrorCode + ": " + se.Message);
                Environment.Exit(se.ErrorCode);
            }

        }

        public void AcceptLoop()
        {
            //While the server is Active, Accept incoming Client Connections
            while(isRunning_)
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
                catch(Exception e)
                {
                    Console.WriteLine(e.Message);
                }

            }
        }

        public void HandleClient(object obj)
        {
            NetworkStream netStream = null;
            try
            {
                // Retrieve the Client from the parameter passed to the thread
                TcpClient client = (TcpClient)obj;

                Console.WriteLine("Handling Client...");

                netStream = client.GetStream();

                // Receive until client closes connection, indicated by the 0 return value
                int bytesRecvd = 0;
                int totalBytesEchoed = 0;

                while ((bytesRecvd = netStream.Read(recvBuffer_, 0, recvBuffer_.Length)) > 0)
                {
                    netStream.Write(recvBuffer_, 0, bytesRecvd);
                    totalBytesEchoed += bytesRecvd;
                }
                               
                Console.WriteLine("Echoed {0} BYTES", totalBytesEchoed);

                netStream.Close();
                client.Close();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
                netStream.Close();
            }
        }

        public static void AcceptCallBack(IAsyncResult ar)
        {
            //Signal the main thread to continue
            allDone.Set();

            Console.WriteLine("Handling Client...");

            Socket listener = (Socket)ar.AsyncState;
            Socket handler = listener.EndAccept(ar);

            StateObject state = new StateObject();
            state.workSocket = handler;
            handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0, new AsyncCallback(ReadCallBack), state);
        }

        public static void ReadCallBack(IAsyncResult ar)
        {
            String content = String.Empty;

            //Retrieve the state object and the handler socket
            //from the asynchronous state object
            StateObject state = (StateObject)ar.AsyncState;
            Socket handler = state.workSocket;

            int bytesRead = handler.EndReceive(ar);

            if(bytesRead > 0){
                state.sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));
                content = state.sb.ToString();
                char[] delimitors = { ':' };
                String[] parts = content.Split(delimitors);

                Console.WriteLine("read {0} bytes from the socket. \nData : {1}", content.Length, content);

                Send(handler, content);
            }
            else
            {
                handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0, new AsyncCallback(ReadCallBack), state);
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

                //handler.Shutdown(SocketShutdown.Both);
                //handler.Close();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

    }
}
