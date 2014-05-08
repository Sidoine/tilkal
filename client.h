#ifndef _CLIENT_INC
#define _CLIENT_INC

namespace Tilkal
{
	enum EClientState
	{
		STATE_UNDEFINED=-1,
		STATE_ALL=0,
		STATE_LOGIN=20,
		STATE_REGISTER=30,
		STATE_READ=100,
		STATE_EDIT_OBJ=200,
		STATE_WRITE_OBJ_NAME=201,
		STATE_WRITE_OBJ_DESC=202,
		STATE_PLAY=500
	};

	class CBuffer
	{
		char b[BUFSIZE];
		size_t l;
	public:
		operator char*()
		{
			return b;
		}
		bool IsEmpty()
		{
			return l==0;
		}
        CBuffer()
		{
			l=0;
		}
		void Clear()
		{
			l=0;
		}
		void Add(const char *c)
		{
			char *d=&b[l];
			while (l<BUFSIZE-1 && *c)
			{
				*(d++)=*(c++);
				l++;
			}
			*d=0;
		}
		void AddQuoted(const char *c)
		{
			char *d=&b[l];
			while (l<BUFSIZE-1 && *c)
			{
				switch (*c)
				{
				case '\n':
					*(d++)='\\';
					l++;
					*d='n';
					break;
				case '\r':
					*(d++)='\\';
					l++;
					*d='r';
					break;
				case '\'':
					*(d++)='\\';
					l++;
					*d='\'';
					break;
				default:
					*d=*c;
				}
				d++;
				c++;
				l++;
			}
			*d=0;
		}
		void Add(const char *c, size_t m)
		{
			if (m>BUFSIZE-1-l)
				m=BUFSIZE-1-l;
			memcpy(&b[l],c,m);
			l+=m;
			b[l]=0;
		}
		void Add(int i)
		{
			if (i<0)
			{
				b[l++]='-';
				i=-i;
			}
			size_t nchiffres=1;
			int j=i;
			while (j>9)
			{
				j/=10;
				nchiffres++;
			}
			if (nchiffres>BUFSIZE-1-l)
				nchiffres=BUFSIZE-1-l;
			char *c=&b[l+nchiffres-1];
			l+=nchiffres;
			while (nchiffres)
			{
                *(c--)=(i%10)+'0';
				i/=10;
				nchiffres--;
			}
			b[l]=0;
		}
	};

	class CClient:public CMyArrayMember
	{
	public:
		friend struct SDefCommande;
		friend class CObject;
		friend class CPlayer;

		int httpid;
		SOCKET Socket;
		char Tampon[512]; //Tampon des commandes
 		int lTampon; //Taille du tampon
		int player;
		char *name;
		char last_cmd[512]; //Dernière commande reçue
		char ip[20];
		EClientState state;
		union
		{
			int EditedObject;
		};
		CBuffer buff;
		static char buffer[BUFSIZE];
		char *reading;

		static SDefCommande DefCommande[200];
		static size_t nCommandes;

		void WriteRoom(const char *name, const char * desc, int image, int num);
		void WriteExit(const char *text, int num);
		void WriteAddObject(int what, int who, int howmany);
		void WriteRemoveObject(int num);
		void WriteEvent(const char *text);
		void WriteError(const char *text);
		void WriteDialogue(const char *who, const char *text);
		void WriteTuDis(const char *text);
		void WriteChangeQuantity(int id);
		void WriteObjectId();
		bool WriteBegin();
		void WriteFinish();
		void WriteInit();
		void WriteInventory(int container);	

		void Do();
		bool ExecuterCommande(char * DebutLigne);
		SDefCommande * ChercherPlusProche(char * Commande);
		void Write(char *buffer);
		int read_socket(char *buffer);
		void send_prompt();
		void send_desc();
		void send_buffer_part();
		void read_book(char *text);
		void send_refresh();
		bool can_use_command(int cmd);
		void disp_help();
		void disp_help_intervalle();
		void echo_off();
		void echo_on();
		void send_olc_edit_object();
		void send_olc_edit_object_name();
		void send_olc_edit_object_desc();
		int DisplayRecursiveInventory(int o, int nspaces, bool recurs);
		void Destroy();
		void disp_help_intervalle(int min, int max, int god);

		inline CObject & obj();
		inline int obj_num();
		inline CPlayer & play();
	};

	class Writer
	{
		bool Started;
		CClient * client;
	public:
		Writer(CClient * c)
		{
			client=c;
			Started=c->WriteBegin();
		}
		~Writer()
		{
			if (Started)
				client->WriteFinish();
		}
	};

	typedef void (CClient::*FonctionCommande)(int nb_args, char ** arg);

	struct SDefCommande
	{
		char Nom[TMAXNOMCOMMANDE+1];
		int state,god,lvl;
		int nParams;
		bool Texte;
		FonctionCommande Fonction;
	};
}

#ifdef _DEFINITIONS
//Tilkal::CBuffer Tilkal::CClient::buff;
char Tilkal::CClient::buffer[BUFSIZE];
size_t Tilkal::CClient::nCommandes;
#endif

#include "players.h"
#include "objects.h"

namespace Tilkal
{
	inline int CClient::obj_num()
	{
		return Player[player].object;
	}

	inline CObject & CClient::obj()
	{
		return Object[Player[player].object];
	}
	inline CPlayer & CClient::play()
	{
		return Player[player];
	}
}


#endif //_CLIENT_INC
