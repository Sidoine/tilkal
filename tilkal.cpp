#include "tilkal.h"
#include "server.h"
#include "clients.h"

using namespace Tilkal;

int main(int argc, char **argv)
{
	if (!Server::InitWorld())
		return 1;
	if (!Server::InitNetwork())
		return 1;
	Server::DefaultWorld();
	Client.StartThread();
	Server::MainLoop();
	Server::shut_down();
	return 0;
}