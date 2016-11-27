using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace GameServer
{
    class StateObject
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
}
