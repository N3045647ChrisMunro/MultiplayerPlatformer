using System;
using System.Net;
using System.Net.Sockets;

namespace GameServer
{
	class MainClass
	{
        public static void Main (string[] args)
		{

            GameSever mainServer = new GameSever();
            mainServer.StartServer(8080);
        }
	}
}
