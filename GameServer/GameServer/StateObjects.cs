using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace GameServer
{
	public class StateObjects
	{
		//Client Socket
		public Socket workSocket = null;

		//Size of recieve Buffer
		public const int BufferSize = 1024;

		//Receive buffer
		public byte[] buffer = new byte[BufferSize];

		//Received data string
		public StringBuilder sb = new StringBuilder();

	}

	class UdpState
	{
		public IPEndPoint endPoint;
		public EndPoint tempRemote;
		public Socket socket;

		public const int BufferSize = 1024;

		public byte[] buffer = new byte[BufferSize];
	}
}

