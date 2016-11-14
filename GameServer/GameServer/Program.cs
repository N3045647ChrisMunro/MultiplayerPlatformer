using System;
using System.Net;
using System.Net.Sockets;

namespace GameServer
{
	class MainClass
	{
        public static void Main (string[] args)
		{

            GameSever test = new GameSever();
            test.StartServer(8080);
        }
	}
}
