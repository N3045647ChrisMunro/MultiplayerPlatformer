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
        private TcpListener TCPlistener_;
        private int serverPortNumber_;
        private Boolean isRunning_;

        private const int BUFFSIZE = 32;
        private byte[] recvBuffer_ = new byte[BUFFSIZE]; 

        public void StartServer(int portNumber)
        {
            try
            {
                Console.WriteLine("Server Started");
                serverPortNumber_ = portNumber;

                //Create a TCPListener to Accept client connections
                TCPlistener_ = new TcpListener(IPAddress.Any, serverPortNumber_);
                TCPlistener_.Start();

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
                    Console.WriteLine("Listening For Connections");
                    TcpClient newClient = TCPlistener_.AcceptTcpClient();

                    //Once a client is found start a thread to handle the client
                    Thread newThread = new Thread(new ParameterizedThreadStart(HandleClient));
                    newThread.Start(newClient);
                   
                }
                catch(Exception e)
                {
                    Console.WriteLine("aids");
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

    }
}
