#ifndef _HTTPCLIENT_INC
#define _HTTPCLIENT_INC

namespace Tilkal
{
	class CFormVar:public CBaseHacheValue
	{
	public:
		char * Texte;
		size_t Longueur;
		char * ContentType;
		char * NomFichier;
		void Detruire()
		{
			if (ContentType)
				delete ContentType;
			if (Texte)
				delete Texte;
			if (NomFichier)
				delete NomFichier;
		}
	};

	class HTTPClient
	{
		SOCKET sock;
		static char *inbuffer;
		static size_t tinbuffer;
		static char *outbuffer;
		static size_t toutbuffer;
		THachage<CFormVar> Champ;

		int Send(const char * c, size_t l);
		void Fail(size_t Error, const char * msg=NULL);
		bool Param_MultiPart(const char * NomVariable,
						   char * contenu, size_t longueur, 
						   const char * NomFichier,
						   char * contentType);
		bool Variable_Creer(const char * Nom, const char * Contenu, size_t Longueur, const char * NomFichier,
								 const char * ContentType);
		bool Param_URL(char * Input);

	public:	
		HTTPClient(SOCKET s)
		{
			sock=s;
			if (!inbuffer)
			{
				inbuffer=(char*)malloc(BUFSIZE);
				tinbuffer=BUFSIZE;
			}
			if (!outbuffer)
			{
				outbuffer=(char*)malloc(BUFSIZE);
				toutbuffer=BUFSIZE;
			}
		}
		
		~HTTPClient()
		{
			if (sock>=0)
				closesocket(sock);
			if (tinbuffer!=BUFSIZE)
			{
				inbuffer=(char*)realloc(inbuffer,BUFSIZE);
				tinbuffer=BUFSIZE;
			}
			if (toutbuffer!=BUFSIZE)
			{
				outbuffer=(char*)realloc(outbuffer,BUFSIZE);
				toutbuffer=BUFSIZE;
			}
		}
		
		bool Do();
	};
}

#ifdef _DEFINITIONS
char *Tilkal::HTTPClient::inbuffer;
size_t Tilkal::HTTPClient::tinbuffer;
char *Tilkal::HTTPClient::outbuffer;
size_t Tilkal::HTTPClient::toutbuffer;
#endif
#endif
