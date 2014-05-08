#include "tilkal.h"
#include "clients.h"
#include "players.h"
#include "tools.h"
#include "server.h"
#include "bans.h"
#include "wwwfiles.h"
#include "httpclient.h"

using namespace Tilkal;
using namespace std;

void CClients::report(char *txt, EReportLevel lvl)
{
	int i,l;
	int m;
	char buf[BUFSIZ];

	sprintf(buf,"<report> %s",txt);
	for (i=0;i<Length;i++)
	{
		if (!IsActive(i))
			continue;
		m=t[i].player;
		
		if (m==NOTHING || !Player[m].god)
			continue;
		if (Player[m].lvl_report<=lvl)
			l=Player[m].lvl;
		else
			l=-10;
		if ((t[i].Socket!=-1) && (l>=lvl))
			t[i].WriteEvent(buf);
	}
	if (lvl>0)
		Server::write_log(txt);	
}

int CClients::New(SOCKET descr, const char *nom, const char *mdp)
{
	//Ne rien écrire sur le socket !
	//Renvoyer -1 en cas d'erreur
	sockaddr_in client;
	socklen_t client_len;

	int p=Player.Find(nom);
	if (p==NOTHING || strcmp(Player[p].pass,mdp))
	{
		if (p==NOTHING)
			cout<<"Impossible de trouver "<<nom<<endl;
		else
			cout<<"Mauvais mot de passe : "<<mdp<<"/"<<Player[p].pass<<endl;
		return NOTHING;
	}

	getpeername(descr,(struct sockaddr *)&client,(socklen_t *)&client_len);
	char *ip=inet_ntoa(client.sin_addr);

	if (Ban.Find(ip)!=-1)
	{
		//sprintf(buffer,"You can't connect. You are banned !");
		//t[u].WriteErrorOnly(buffer);
		sprintf(buffer,"BANNED guest connected from [%s]. <b>Rejected</b>",ip);
		report(buffer,REPORT_BAN_CONNECT);
		cout << "Banni tentant de se connecter"<<endl;
		return NOTHING;
	}
	
	int u=CClientsBase::New();
	assert(u!=NOTHING);
	t[u].Socket=descr;
	t[u].lTampon=0;
	t[u].last_cmd[0]=0;
	t[u].buff.Clear();
	strcpy(t[u].ip,ip);
	t[u].player=p; 
	t[u].state=STATE_PLAY;
	t[u].obj().add_to_obj(START_ROOM);
	Player[p].client=u;
				
		/*int n=Player.New();
		//t[u].WriteEventOnly("Welcome to Tilkal");
		descr=-1;
		t[u].echo_on();
		t[u].httpid=httpid;
		t[u].player=n;
		Player[n].create_standard();
		Player[n].client=u;
		t[u].name=t[u].obj().name;
		t[u].state=STATE_LOGIN;
	sprintf(buffer,"Guest connected from [%s]",t[u].ip);
	report(buffer,REPORT_CONNECT);
	cout <<"Nouveau client connecté"<<endl;
	TODO: On appelle pas report sinon ça essaierait d'écrire
	dans le socket...
	Voir comment faire*/
	return u;
}

inline int hexval(char c)
{
	if (c>='a' && c<='f')
		return c-'a'+10;
	if (c>='A' && c<='F')
		return c-'A'+10;
	return c-'0'+10;
}

inline void FailAndClose(SOCKET s, const char * error)
{
	send(s,error,(int)strlen(error),MSG_NOSIGNAL);
	closesocket(s);
}


void CClients::Thread()
{
	puts("[Tilkal] Thread started.");
	sockaddr_in client;
//	int i;
	SOCKET descr;
	socklen_t client_len;
	fd_set sock_set;

	WaitMutex(Server::mutex);
	
	while (1)
	{
		FD_ZERO(&sock_set);
	//	FD_SET(Server::telnetsock,&sock_set);
		FD_SET(Server::httpsock,&sock_set);		
		
		SOCKET fd=Server::httpsock;
	/*	for (i=0;i<Length;i++)
			if (IsActive(i))
			{
				FD_SET(Client[i].Socket,&sock_set);
				if (Client[i].Socket>fd)
					fd=Client[i].Socket;
			}*/

		ReleaseMutex(Server::mutex);
		//clog<<"Début select"<<endl;
		int ret=select(int(fd)+1,&sock_set,NULL,NULL,NULL);
		//clog<<"Fin sélect"<<endl;
		WaitMutex(Server::mutex);
		
		if (ret>0)
		{
			/* Test a new connection. 
			if (FD_ISSET(Server::telnetsock,&sock_set))
			{
				clog <<"telnetsock is set"<<endl;
				client_len=sizeof(client);
				descr=accept(Server::telnetsock,(sockaddr*)&client,&client_len);
				
				if (descr>=0)
					New(descr);
			}*/
			
			if (FD_ISSET(Server::httpsock,&sock_set))
			{
				//clog <<"httpsock is set"<<endl;
				client_len=sizeof(client);
				descr=accept(Server::httpsock,(sockaddr*)&client,&client_len);
				
				if (descr<0)
				{
					cerr<<"accept";
					closesocket(descr);
				}
				else
				{
					HTTPClient client(descr);
					client.Do();
				}
			}

			/* Test players commands. */
		/*	for (i=0;i<Length;i++)
				if (IsActive(i) && FD_ISSET(t[i].Socket,&sock_set))
					t[i].Do();*/
		}
		else
		{
			perror("select");
		//	Sleep(1000);
		}
	}
	ReleaseMutex(Server::mutex);
	puts("[Tilkal] Thread exited.");
}

#ifdef _WIN32
void check_sockets(void *arg)
{
	Client.Thread();
}
#else
void * check_sockets(void * arg)
{
	Client.Thread();
	return NULL;
}
#endif

bool CClients::StartThread()
{
	printf("[Tilkal] Starting thread.\r\n");
	Server::write_log("[Tilkal] Starting thread.\r\n");
#ifdef _WIN32
	_beginthread(&check_sockets,0,NULL);
#else
	int ret;
	pthread_t thread_sock;
	if (( ret=pthread_create(&thread_sock,NULL,&check_sockets,NULL))!=0)
	{
		fprintf(stderr,"%s\r\n",strerror(ret));
		return false;
	}
#endif
	return true;
}
