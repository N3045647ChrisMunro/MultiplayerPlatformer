using System;
using System.Net;
using System.Net.Sockets;


namespace GameServer
{
	class MainClass
	{

		private const int BUFFSIZE = 32;

		public static void Main (string[] args)
		{
			Console.WriteLine ("Hello World!");

			if (args.Length > 1)
				throw new ArgumentException ("Parameters: [<Port>]");

			int servPort = (args.Length == 1) ? Int32.Parse (args [0]) : 8080;

			TcpListener listener = null; 

			try
			{
				//Create a TCPListener to Accept client connections
				listener = new TcpListener(IPAddress.Any, servPort);
				listener.Start();
			}
			catch(SocketException se)
			{
				Console.WriteLine (se.ErrorCode + ": " + se.Message);
				Environment.Exit (se.ErrorCode);
			}
			byte[] recvBuffer = new byte[BUFFSIZE]; // Receive Buffer

			int bytesRecvd; // Received byte Count

			for (; ;)
			{
				//Run forever, accepting and servicing connections
				TcpClient client = null;
				NetworkStream netStream = null;

				try
				{
					client = listener.AcceptTcpClient(); // Get Client Connection
					netStream = client.GetStream();

					Console.WriteLine("Handling Client - ");

					// Receive until client closes connection, indicated by the 0 return value
					int totalBytesEchoed = 0; 

					while((bytesRecvd = netStream.Read(recvBuffer, 0, recvBuffer.Length)) > 0)
					{
						netStream.Write(recvBuffer, 0, bytesRecvd);
						totalBytesEchoed += bytesRecvd;
					}

					Console.WriteLine("Echoed {0} BYTES", totalBytesEchoed);

					netStream.Close();
					client.Close();
				}
				catch(Exception e)
				{
					Console.WriteLine (e.Message);
					netStream.Close ();
				}

			}


		}
	}
}
