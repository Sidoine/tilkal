#ifndef _CLIENTS_INC
#define _CLIENTS_INC

#include "client.h"
#include "player.h"

namespace Tilkal
{
	typedef TMyArray<CClient,64> CClientsBase;

	class CClients:public CClientsBase
	{
		char buffer[BUFSIZ];

	public:
		void report(char *txt, EReportLevel lvl);
		CClients()
		{
			size_t n=0;
			while (CClient::DefCommande[n].Nom[0])
				n++;
			CClient::nCommandes=n;
		}
		int New(SOCKET s, const char * nom, const char *mdp);
		void Thread();
		bool StartThread();
		void AnswerToHTTPRequest(SOCKET descr);
	};
}

#ifdef _DEFINITIONS
Tilkal::CClients Client;
#else
extern Tilkal::CClients Client;
#endif

#endif
