#ifndef _SERVER_INC
#define _SERVER_INC

namespace Tilkal
{

class Server
{
public:
	static int64 lastsave;
	static int64 t;//Millisecondes
	static MUTEX mutex;
	static SOCKET telnetsock, httpsock;
	static void DefaultWorld();
	static void shut_down(void);
	static void write_log(const char *txt);
	static bool InitWorld();
	static bool InitNetwork();
	static void MainLoop();
	static void Save();
	static void LoadTexts();
};

#ifdef _DEFINITIONS
int64 Server::lastsave;
int64 Server::t;
SOCKET Server::telnetsock, Server::httpsock;
MUTEX Server::mutex;
#endif

}

#endif
