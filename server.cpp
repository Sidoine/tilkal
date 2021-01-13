#include "tilkal.h"
#include "all.h"

using namespace std;
using namespace Tilkal;
using namespace TilTools;

void handle_signals(int num)
{
	switch (num)
	{
	case SIGINT:
	case SIGTERM:
		Server::shut_down();
		break;
	}
}
/*
void wait_exit_status(int a)
{
  wait(0);
  signal(SIGCHLD,wait_exit_status);
}*/


void Server::DefaultWorld()
{
	char buf[BUFSIZ];
	int i;
	int s1,s2,s3;

	/* This is the first room. It should always have the number defined in START_ROOM. Put
	   this code at the start of this routine in order to get number 0. */
	if (Object.Length==0)
	{
		s1=Object.New();
		strcpy(buf,"You stand on a small village square, surrounded by little old houses and shops.\r\n");
		strcat(buf,"There are a few villagers wandering here and there, without even noticing your\r\n");
		strcat(buf,"presence. To the north there is a very remarkable inn called the 'Dead Rat Inn'.\r\n");
		strcat(buf,"To the east there is a grocer that buys and sells any kinds of wares.\r\n");
		Object[s1].create_room("The village square.",buf);
		s2=Object.New();
		strcpy(buf,"You are inside the famous 'Dead Rat Inn'. It is small and full with happy busy\r\n");
		strcat(buf,"people.\r\n");
		Object[s2].create_room("The Dead Rat Inn.",buf);
		s3=Object.New();
		strcpy(buf,"This is the village shop, where you can buy and sell things. There are many objects,\r\n");
		strcat(buf,"waiting to be bought.\r\n");
		Object[s3].create_room("The shop.",buf);
		clog << "[Tilkal] Default rooms created.\n";		
	}
	if (Door.Length==0)
	{
		Door.New("north","south");
		Door.New("east","west");  
		Door.New("south-east","north-west");  
		Door[0].add_to_rooms(s1,s2);
		Door[1].add_to_rooms(s1,s3);
		Door[2].add_to_rooms(s2,s3);
		clog << "[Tilkal] Default doors created.\n";
	}

 	if (Player.Length==0)
	{
		int p=Player.New();
		Player[p].create_standard();
		int o=Player[p].object;
		Object[o].set_name("l");		
		strcpy(Player[p].pass,"l");
		Player[p].is_registered=true;
		Player[p].god=true;
		Player[p].lvl=GOD_HIGHEST;	
		clog << "[Tilkal] Default IMM created.\n";	
	}

/*  for (i=0;i<max_rooms;i++)
  {
    room_data[i].in_room=NOTHING;
  }
  for (i=0;i<max_objects;i++)
  {
    if ((Object[i].state!=STATE_UNDEFINED) && (get_room(i)!=NOTHING) &&
         (Object[i].type!=ObjType::Human))
      Object[i].add_to_room(get_room(i));
  } */
	if (Object.Length==4)
	{
		i=Object.New();
		Object[i].create_standard_bot("an old beggar",1);
		Object[i].add_to_obj(s2);

		i=Object.New();
		Object[i].create_standard_bot("a shopkeeper",2);
		Object[i].add_to_obj(s3);

		i=Object.New();
		Object[i].create_standard(3);//3=gold coin
		Object[i].weight=10;
		Object[i].quantity=10;
		Object[i].add_to_obj(s1);

		i=Object.New();
		Object[i].create_standard(6);//6=silver coin
		Object[i].weight=10;
		Object[i].quantity=10;
		Object[i].add_to_obj(s1);
		
		i=Object.New();
		Object[i].create_standard(2);
		Object[i].set_name("a two-handed sword");
		Object[i].weight=15;
		Object[i].Set_Data(0.8f,0.2f,0,0,0);
		Object[i].add_to_obj(s1);

		i=Object.New();
		Object[i].create_standard(4);
		Object[i].set_name("a bag");
		Object[i].weight=4;
		Object[i].add_to_obj(s1);

		i=Object.New();
		Object[i].create_standard(4);
		Object[i].set_name("a handybag");
		Object[i].weight=3;
		Object[i].add_to_obj(s1);

		i=Object.New();
		Object[i].create_standard(1);
		Object[i].set_name("a chainmail");
		Object[i].weight=20;	
		Object[i].Set_Data(0,0,0,0,0); /* Standard chainmail. */
		Object[i].add_to_obj(s1);
		
		i=Object.New();
		Object[i].create_standard(5);
		Object[i].set_name("a bread");
		Object[i].weight=3;
		Object[i].add_to_obj(s1);

	}
}

void Server::shut_down()
{
	Player.remove_all();
	Save();
	printf("[Tilkal] Server shut down.\r\n");
	write_log("[Tilkal] Server shut down.\r\n");
	closesocket(telnetsock);
	closesocket(httpsock);
	exit(0);
}

void Server::write_log(const char *txt)
{
  char file[20];
  char now[200];
  time_t t=time(NULL);
  struct tm *tm=localtime(&t);
  FILE *fichier;
   
  strcpy(file,"text/log.txt");
  fichier=fopen(file,"a+");
  if (fichier == NULL)
  {
    printf("[Tilkal] Unable to open log file.\r\n");
    exit(-1);
  }
  sprintf(now,"[%02d/%02d] %02d:%02d:%02d = %s",tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min,
          tm->tm_sec, txt);
  fprintf(fichier,"%s",now);
}

SOCKET CreateSocket(short Port)
{
	SOCKET sock;
	sock = socket(AF_INET,SOCK_STREAM,0);
#ifndef _WIN32
	int  aut=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&aut,sizeof(int));
	aut=BSIZE;
	setsockopt(sock,SOL_SOCKET,SO_SNDBUF,&aut,sizeof(int));
	setsockopt(sock,SOL_SOCKET,SO_RCVBUF,&aut,sizeof(int));
#endif

	if (sock<0)
	{
		perror("\r\n[Tilkal] socket error");
		printf("\r\n");
		return -1;
	}
	sockaddr_in gameserver;
	gameserver.sin_family = AF_INET;
	gameserver.sin_addr.s_addr = htonl(INADDR_ANY);
	gameserver.sin_port = htons(Port);
	if (bind(sock,(struct sockaddr*)&gameserver,sizeof(gameserver))<0)
	{
		perror("\r\n[Tilkal] bind failed");
		printf("\r\n");
		return  -1;
	}
	if(listen(sock,4)==-1)
	{
		perror("\r\n[Tilkal] listen failed");
		printf("\r\n");
		return  -1;
	}
	printf("[Tilkal] Listening on port %d\n", Port);
	return sock;
}

bool Server::InitNetwork()
{
	signal(SIGINT,handle_signals);
	signal(SIGTERM,handle_signals);

  	NewMutex(mutex);
#ifdef _WIN32
	WSADATA Donnees;
	if (WSAStartup(MAKEWORD( 2, 2 ),&Donnees))
	{
		return false;
	}
#endif

	auto port = getenv("PORT");
	
	telnetsock=CreateSocket(4500);
	httpsock=CreateSocket(port ? atoi(port) : 4580);
	return true;
}

void Server::LoadTexts()
{
	load_file_text("text/welcome.txt",welcome_buf);
	load_file_text("text/motd.txt",motd_buf);
	load_file_text("text/death.txt",death_buf);
	load_file_text("text/rules_start.txt",rules_start_buf);
	load_file_text("text/rules_admin.txt",rules_admin_buf);
	load_file_text("text/rules_authors.txt",rules_authors_buf);
	load_file_text("text/rules_immort.txt",rules_immort_buf);
}

bool Server::InitWorld()
{
	FILE * F;

	printf("\r\n");
	ObjCateg.init();

 	
	if (F=fopen("data/server.dat","rb"))
	{
		fread(&t,sizeof(t),1,F);
		fclose(F);
	}
	Object.LoadAll("data/objects.dat");
	Player.LoadAll("data/players.dat");
	Door.LoadAll("data/doors.dat");
	t=0;

	InitAI();
	SkillProt.Init();
	WWWFile.Init();
	
	printf("[Tilkal] Initializing.\r\n");
	write_log("[Tilkal] Initializing.\r\n");
	DefaultWorld();
	printf("[Tilkal] Loading text files.\r\n");
	write_log("[Tilkal] Loading text files.\r\n");
	LoadTexts();

	Ban.LoadAll("text/bans.txt");
	ForbNick.LoadAll("text/forbnicks.txt");
	return true;
}


void Server::Save()
{
	return;
	FILE * F=fopen("data/server.dat","wb");
	fwrite(&t,sizeof(t),1,F);
	fclose(F);
	Object.SaveAll("data/objects.dat");
	Player.SaveAll("data/players.dat");
	Door.SaveAll("data/doors.dat");
	Ban.SaveAll("text/bans.txt");
	ForbNick.SaveAll("text/forbnicks.txt");
}

void Server::MainLoop()
{
	lastsave=t;
	printf("[Tilkal] Starting main loop.\r\n");
	write_log("[Tilkal] Starting main loop.\r\n");
	fflush(stdout);
	WaitMutex(mutex);
	while(1)
	{
		while (Event.first!=NOTHING && Event.First().time<=t)
		{
			CEvent e=Event.First();
			Event.RemoveFirst();
			switch (e.type)
			{
			case EV_AI:
				if (Object.IsAlive(e.UID))
				  (&Object[IDfromUID(e.UID)]->*AI[Object[IDfromUID(e.UID)].subtype].Event)
				  (e.action,e.data1,e.data2);
				break;
			case EV_OBJECT:
				if (Object.IsAlive(e.UID))
				  Object[IDfromUID(e.UID)].Event(e.action,e.data1,e.data2);
				break;
			default:
				clog << "Event of type "<<e.type<<" is unknown.\n";
			}
		}
		int attente;
		if (Event.first==NOTHING)
			attente=1000;
		else
		{
			attente=int(Event.First().time-t);
			if (attente>1000)
				attente=1000;
		}
		ReleaseMutex(mutex);
#ifdef _WIN32
		Sleep(attente);
#else
		usleep(attente*1000);
#endif
		t+=attente;
		WaitMutex(mutex);
		if ((t-lastsave)>(SAVE_TIME*1000))
		{
			Save();
			lastsave=t;
		}

	}
}
