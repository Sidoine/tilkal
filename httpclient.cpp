#include "tilkal.h"
#include "httpclient.h"
#include "tools.h"
#include "wwwfiles.h"
#include "clients.h"

using namespace Tilkal;
using namespace TilTools;

namespace HTTPOutils
{
	inline void SauterL(char * &x)
	{
		if (*x=='\n')
			x++;
		else
			x+=2;
	}

	inline void CouperL(char * &x)
	{
		if (*x=='\n')
			*(x++)=0;
		else
		{
			*x=0;
			x+=2;
		}
	}

	inline void CouperPL(char * &x)
	{
		if (*(x-2)=='\r')
			*(x-2)=0;
		else
			*(x-1)=0;
	}

	//Extrait la valeur du meta sans l'option
	char * ExtraitValeurMeta(char * &Meta)
	{
		char * ret=Meta;
		if (*Meta=='"')
		{
			Meta++;
			ret++;
			while (*Meta && *Meta!='"')
				Meta++;
			if (*Meta)
				*(Meta++)=0;
			while (*Meta && *Meta!=';')
				Meta++;
		}
		else
		{
			while (*Meta && *Meta!=';')
				Meta++;
			if (*Meta)
				*(Meta++)=0;
		}
		return ret;
	}

	//Extrait la prochaine option du meta
	void ExtraitOptionMeta(char * &Meta, char * &OptionType , const char * &Option)
	{
		while (*Meta==' ')
			Meta++;

		OptionType=Meta;
		Option="";

		while (*Meta!='=')
			Meta++;
		if (*Meta==0)
			return;

		*(Meta++)=0;

		if (*Meta=='"')
		{
			Meta++;
			Option=Meta;
			while (*Meta && *Meta!='"')
				Meta++;
		}
		else
		{
			Option=Meta;
			while (*Meta && *Meta!=';')
				Meta++;
		}
		if (*Meta)
			*(Meta++)=0;
	}

	//Extrait un m�ta du contenu
	void ExtraitMeta(char * & contenu, char * & metaNom, char * & metaVal)
	{
		metaNom=contenu;

		//On cherche les :
		while ((*contenu) && (*contenu!='\r') && (*contenu!='\n') && (*contenu!=':'))
			contenu++;

		//On les trouve => suivi par la valeur
		if (*contenu==':')
		{
			*(contenu++)=0;//On termine le nom du m�ta

			//On saute les espaces
			while (*contenu==' ')
				contenu++;

			//C'est la valeur
			metaVal=contenu;
			while ((*contenu!='\r') && (*contenu!='\n') && (*contenu))
				contenu++;
		}
		else
			metaVal=NULL;
		CouperL(contenu);
	}

	char * cherche(char *a, const char *b, size_t l)
	{
		int i;
		while (l)
		{
   			i=0;
			while (((a[i]==b[i]) || (b[i]==0)) && (l-i))
			{
				if (b[i]==0)
					return a;
				i++;
			}
			a++;
			l--;
		}
		return NULL;
	}
}

using namespace HTTPOutils;

int HTTPClient::Send(const char * c, size_t l)
{
	while (l)
	{
		int sent=send(sock,c,(int)l,MSG_NOSIGNAL);
		if (sent<0)
			return sent;
		c+=sent;
		l-=sent;
	}
	return 0;
}

void HTTPClient::Fail(size_t Error,const char * msg)
{
	int l;
	if (msg)
	{
		l=sprintf(outbuffer,"HTTP/1.1 %d Z\r\n\r\n"
			"<html><head><title>Error %d</title></head>"
			"<body>%s</body></html>",Error,Error,msg);
	}
	else
	{
		l=sprintf(outbuffer,"HTTP/1.1 %d Z\r\n\r\n"
			"<html><head><title>Error %d</title></head>"
			"<body>Error %d</body></html>",Error,Error,Error);
	}
	Send(outbuffer,l);
}

bool HTTPClient::Do()
{
	size_t linbuffer=0;
	size_t ainbuffer=BUFSIZE;
	
	do
	{
		int n=recv(sock,&inbuffer[linbuffer],int(BUFSIZE-linbuffer-1),0);
		if (n<=0)
		{
			return false;
		}
		linbuffer+=n;
		inbuffer[linbuffer]=0;
		if (linbuffer==BUFSIZE-1)
		{
			Fail(400);
			return false;
		}
	}
	while (!strstr(inbuffer,"\r\n\r\n"));
	//On a besoin d'au moins r�cup�rer l'ent�te complet
	//(qui se termine par deux retours � la ligne)
	
	char * pos=inbuffer;
	char * Methode=pos;
	
	while (*pos && *pos!=' ')
		pos++;
	if (*pos)
		*(pos++)=0;
	
	//On ne supporte rien d'autre qu'un GET
	if (strcmp(Methode,"GET") && strcmp(Methode,"POST"))
	{
		Fail(405);
		return false;
	}
	
	//On cherche le nom du fichier
	
	//On saute la base
	if (*pos=='/')
		pos++;
	else
	{
		//Chemin absolu ?
		if (memcmp(pos,"http://",7))
		{
			Fail(405);
			return false;
		}
		pos+=7;
		while (*pos && *pos!='/')
			pos++;
		if (*pos)
			pos++;
	}
	
	char *file=pos;

	//On termine le nom de fichier
	while (*pos && *pos!=' ')
		pos++;
	if (*pos)
		*(pos++)=0;

	//We look for the parameters
	char *queryString=file;
	while (*queryString && *queryString!='?')
		queryString++;
	
	if (*queryString)
		*(queryString++)=0;
	
	//We cut the filename at the ,
	char *_id=file;
	
	while (*_id && *_id!=',')
		_id++;
	if (*_id)
		*(_id++)=0;

	int id=atoi(_id);
		
	//On limite le nom de fichier � 31 caract�res
	char filename[32];
	strncpy(filename,file,31);
	
	//Maintenant on regarde les lignes d'apr�s
	while (*pos!='\n')
		pos++;
	
	size_t contentLength=0;
	char * contentType=NULL;
	
	while (*pos)
	{
		if (*pos=='\r' && pos[1]=='\n')
		{
			pos+=2;
			break;
		}
		
		char *metaNom,*metaVal;
		ExtraitMeta(pos,metaNom,metaVal);
		strlwr(metaNom);
		
		if (strcmp(metaNom,"content-type")==0)
		{
			contentType=metaVal;
		}
		else if (strcmp(metaNom,"content-length")==0)
		{
			contentLength=atoi(metaVal);
		}
	}
	
	char * content=pos;
			
	if (contentLength)
	{
		//On lit tout tout tout !
		size_t Lu=linbuffer-(content-inbuffer);
		if (contentLength+(content-inbuffer)>BUFSIZE)
		{
			tinbuffer=contentLength+(content-inbuffer)+1;
			inbuffer=(char*)realloc(inbuffer,tinbuffer);
		}
		
		while (Lu<contentLength)
		{
			int n=recv(sock,&inbuffer[linbuffer],int(tinbuffer-linbuffer),0);
			if (n<=0)
				return false;
			linbuffer+=n;
			Lu+=n;
		}
		inbuffer[linbuffer]=0;
	}
	
	if ((contentType==NULL) || (memcmp(contentType,"multipart/form-data",19)))
		Param_URL(strcmp(Methode,"POST")?queryString:content);
	else
		Param_MultiPart("",content,contentLength,"",contentType);

	if (*filename==0)
		strcpy(filename,"index.html");

	if (strcmp(filename,"jouer")==0)
	{
		if (!Champ.Existe("nom")||!Champ.Existe("mdp"))
		{
			Fail(402,"Param�tres manquants");
			return false; 
		}
		int id=rand();
		if (id==0)
			id=1;//Parce qu'on consid�re qu'un id=0 signifie qu'on n'en a pas
		sprintf(outbuffer,
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\nCache-Control: no-cache\r\n\r\n"
			"<html><head><title>Tilkal</title></head>\r\n"
			"<frameset rows=\"*,0\"><frame src=\"b?nom=%s&amp;mdp=%s\"/>"
			"<frame src=\"z\" name=\"z\"/></frameset></html>\r\n",
			Champ["nom"]->Texte,Champ["mdp"]->Texte);
		Send(outbuffer,(int)strlen(outbuffer));
	}
	else
	if (strcmp(filename,"nouveau")==0)
	{
		if (!Champ.Existe("nom")||!Champ.Existe("mdp"))
		{
			Fail(402,"Param�tres manquants");
			return false; 
		}
		char * nom=Champ["nom"]->Texte;
		char * mdp=Champ["mdp"]->Texte;
		if (!nom || !mdp)
		{
			Fail(402,"Param�tres manquants");
			return false; 
		}
		if (Player.Find(nom)==NOTHING)
		{
			int p=Player.New();
			Player[p].create_standard();
			printf("[Tilkal] New player %s created", nom);
			strcpy(Player[p].pass,mdp);
			strcpy(Player[p].obj().name,nom);
			Player[p].is_registered=true;
			sprintf(outbuffer,
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\nCache-Control: no-cache\r\n\r\n"
				"<html><head><title>Tilkal</title></head>\r\n"
				"<frameset rows=\"*,0\"><frame src=\"b?nom=%s&amp;mdp=%s\"/>"
				"<frame src=\"z\" name=\"z\"/></frameset></html>\r\n",
					nom,mdp);
			Send(outbuffer,(int)strlen(outbuffer));
		}
		else
			Fail(402,"Ce nom est d�j� utilis�");
	}
	else 
	if (strcmp(filename,"b")==0)
	{
		if (!Champ.Existe("nom")||!Champ.Existe("mdp"))
		{
			Fail(402,"Param�tres manquants");
			return false; 
		}

		int u=Client.New(sock,Champ["nom"]->Texte,Champ["mdp"]->Texte);
		
		if (u==NOTHING)
		{
			Fail(401,"Impossible de cr�er le client");
			return false;
		}
		strcpy(outbuffer,"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n\r\n");
		Send(outbuffer,(int)strlen(outbuffer));
		sprintf(outbuffer,WWWFile["interface.html"]->data,u,u);
		Send(outbuffer,(int)strlen(outbuffer));
		Client[u].WriteInit();
		sock=NOTHING;//We don't want it to be closed
		return true;
	}
	else if (strcmp(filename,"c")==0)
	{
		sockaddr_in client;
		socklen_t client_len;
		getpeername(sock,(struct sockaddr *)&client,(socklen_t *)&client_len);

		if (!Client.IsActive(id))// || strcmp(inet_ntoa(client.sin_addr),Client[id].ip))
		{
			//TODO: enlever
			puts("Accès interdit :");
			puts(inet_ntoa(client.sin_addr));
			if (Client.Length > id)
				puts(Client[id].ip);
			Fail(401);
			return false;
		}

		if (Champ.Existe("t"))
		{
			Client[id].ExecuterCommande(Champ["t"]->Texte);
			strcpy(outbuffer,"HTTP/1.1 200 OK\r\n\r\n");
			Send(outbuffer,(int)strlen(outbuffer));
		}
		else
			Fail(400);
	}
	else
	{
		CWWWFile * file=WWWFile[filename];
		if (!file)
		{
			Fail(404);
		}
		else
		{
			strcpy(outbuffer,"HTTP/1.1 200 OK\r\nContent-Type: ");
			switch(file->type)
			{
				case ContentType::text_css:
					strcat(outbuffer,"text/css");
					break;
				case ContentType::image_jpeg:
					strcat(outbuffer,"image/jpeg");
					break;
				case ContentType::image_png:
					strcat(outbuffer,"image/png");
					break;
				case ContentType::text_html:
					strcat(outbuffer,"text/html");
					break;
				default:
					strcat(outbuffer,"text/plain");
			}
			strcat(outbuffer,"\r\n\r\n");
			Send(outbuffer,(int)strlen(outbuffer));
			Send(file->data,(int)file->length);
		}
	}
	return false;
}




bool HTTPClient::Param_MultiPart(const char * NomVariable,
						   char * contenu, size_t longueur, 
						   const char * NomFichier,
						   char * contentType)
{
	char * suite;
	char * metaNom,*metaVal;
	char * Depart;
	char * Temp;

	if (contentType==NULL)
	{
	#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Nom de variable :");
		HTML_Erreur(NomVariable);
		HTML_Erreur("NOT Valeur de la variable :");
		HTML_Erreur(contenu);
	#endif
		Variable_Creer(NomVariable,contenu,longueur,NomFichier,contentType);
	#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Variable cr��e");
	#endif
		return true;
	}

	Temp=ExtraitValeurMeta(contentType);
	if (!*Temp)
		return false;//M�ta mal form�

    if (strcmp(Temp,"multipart/form-data")&&
		strcmp(Temp,"multipart/mixed"))
	{
		//Type non-support�
		return false;
	}

	const char* boundary=NULL;
	while (*contentType)
	{
		char *Option;
		const char *OptionVal;
		ExtraitOptionMeta(contentType,Option,OptionVal);
		if (strcmp(Option,"boundary")==0)
			boundary=OptionVal;
	}


#ifdef DEBUG_PARAM
	HTML_Message("NOT Multipart:");
#endif
	if (!boundary || !*boundary)
		return false;//HTML_Erreur("Le d�limiteur des donn�es du formulaire n'est pas d�fini.");

#ifdef DEBUG_PARAM
	HTML_Message("NOT Boudary :");
	HTML_Message(boundary);
#endif

	
	do
	{
		contentType=NULL;
		NomFichier=NULL;

		Depart=contenu;
		//On saute le boundary pr�c�d� de deux -
		contenu+=2+strlen(boundary);
		//Si le boundary est suivi d'un tiret on met fin � la boucle
		if (*contenu=='-')
			break;
		SauterL(contenu);
		suite=cherche(contenu,boundary,longueur-(contenu-Depart));
		if (suite==NULL)
			break;
		suite-=2; //On positionne la suite au premier tiret (--boundary)
		do
		{
			ExtraitMeta(contenu, metaNom, metaVal);
			//Si la ligne n'est pas vide
			if (*metaNom)
			{
				strlwr(metaNom);
				
#ifdef DEBUG_PARAM
				HTML_Message("NOT -- Meta :");
				HTML_Message(metaNom);
				if (metaVal)
					HTML_Message(metaVal);
#endif
				if (strcmp(metaNom,"content-type")==0)
				{
					contentType=metaVal;
#ifdef DEBUG_PARAM
					HTML_Message("NOT Content-type d�fini");
#endif
				}
				else if (strcmp(metaNom,"content-disposition")==0)
				{
#ifdef DEBUG_PARAM
					HTML_Message("NOT Content-disposition d�fini");
#endif
					while (*metaVal)
					{
						char *Option;
						const char *OptionVal;
						ExtraitOptionMeta(metaVal,Option,OptionVal);
						if (strcmp(Option,"name")==0)
							NomVariable=OptionVal;
						else if(strcmp(Option,"filename")==0)
							NomFichier=OptionVal;
					}
				}
#ifdef DEBUG_PARAM
				HTML_Message("NOT -- Analyse du m�ta termin�e");
#endif
			}
		}
		while (*metaNom);
		CouperPL(suite);
		Param_MultiPart(NomVariable,contenu,suite-contenu-2,NomFichier,
			contentType);
		contenu=suite;
		longueur-=int(suite-Depart);
	}
	while (longueur>0);
	return true;
}

bool HTTPClient::Variable_Creer(const char * Nom, const char * Contenu, size_t Longueur, const char * _NomFichier,
								const char * _ContentType)
{
	if (Nom==NULL)
		Nom="";

	CFormVar *var=Champ.Ajouter(Nom);
	
	if (Longueur)
	{
		var->Texte=new char[Longueur+1];
		memcpy(var->Texte,Contenu,Longueur);
		var->Texte[Longueur]=0;
	}
	else
		var->Texte=NULL;

	var->Longueur=Longueur;
		
	if (_ContentType)
	{
		size_t l=strlen(_ContentType);
		if (l)
		{
			var->ContentType=new char[l+1];
			memcpy(var->ContentType,_ContentType,l);
			var->ContentType[l]=0;
		}
		else
			var->ContentType=NULL;
	}
	else
		var->ContentType=NULL;

	if (_NomFichier)
	{
		size_t l=strlen(_NomFichier);
		if (l)
		{
			var->NomFichier=new char[l+1];
			memcpy(var->NomFichier,_NomFichier,l);
			var->NomFichier[l]=0;
		}
		else
			var->NomFichier=NULL;
	}
	else
		var->NomFichier=NULL;

	return true;
}

inline void PlusesToSpaces(char *Str)
// Convertit les + en espaces
{
    if (Str != NULL)
	{
		while (*Str != '\0')
        {
			if (*Str == '+')
				*Str = ' ';
			Str++;
		}
	}
}

inline int HexVal(char c)
// Renvoie la valeur d'un caract�re correspondant � un chiffre hexad�cimal
{
	if ((c>='0') && (c<='9')) return (c-'0');
	if ((c>='a') && (c<='f')) return (c-'a'+10);
	if ((c>='A') && (c<='F')) return (c-'A'+10);
	return 0;
}

inline void TranslateEscapes(char *Str)
// Convertis les %XX en caract�res dont le XX est la valeur ASCII
{
	char *NextEscape;
	int AsciiValue;

	NextEscape = strchr(Str, '%');
	while (NextEscape != NULL)
	{
		AsciiValue = (16 * HexVal(NextEscape[1])) + HexVal(NextEscape[2]);
		*NextEscape = (char) AsciiValue;
		memmove(&NextEscape[1], &NextEscape[3], strlen(&NextEscape[3]) + 1);
		NextEscape = strchr(&NextEscape[1], '%');
	}
}

bool HTTPClient::Param_URL(char * Input)
//D�codage des param�tres cod�s URL
{
	// les variables sont s�par�es par le caract�re "&" 

	char *pToken;
	char *NomVar,*ValVar;

	pToken = Input;
	while (*pToken)
	{
		NomVar=pToken;
		while ((*pToken) && (*pToken!='=') && (*pToken!='&'))
			pToken++;
		if (*pToken=='=')
		{
			if (*pToken) *(pToken++)=0;
			ValVar=pToken;
			while ((*pToken) && (*pToken!='&'))
				pToken++;
			if (*pToken) *(pToken++)=0;
			PlusesToSpaces(ValVar);
			TranslateEscapes(ValVar);
#ifdef DEBUG_PARAM
			HTML_Erreur("NOT Nouvelle variable :");
			HTML_Erreur(NomVar);
			HTML_Erreur(ValVar);
#endif
			Variable_Creer(NomVar,ValVar,strlen(ValVar),"","");
		}
		else
		{
			if (*pToken) *(pToken++)=0;
			Variable_Creer(NomVar,"",0,"","");
		}
	}

	return true;
}
/*
void HTTPClient::Init()
//Lecture des param�tres
{
#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Lecture des param�tres :");
#endif
	char *contentType=NULL;
	size_t argLength;
	char *Input;

	char *requestMethod =getenv("REQUEST_METHOD");

	if (requestMethod && stricmp(requestMethod,"POST")==0)
	{
		// M�thode POST
		//On lit les donn�es envoy�es dans l'en-t�te
		argLength=atoi(getenv("CONTENT_LENGTH"));

	#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Longueur des donn�es");
	#endif
		
		if (argLength == 0)
			return;
		
		Input=(char*)malloc(argLength+1);
#ifdef _WIN32
		_setmode( _fileno( stdin ), _O_BINARY );
#endif
		fread(Input,argLength,1,stdin);
		Input[argLength]=0;
	}
	else
	{
		// M�thode GET
		char * tp=getenv("QUERY_STRING");
		if (tp==NULL)
		{
			Input=(char*)malloc(sizeof("a=12"));
			strcpy(Input,"a=12");
		}
		else
		{
			argLength=strlen(tp);
			Input=(char*)malloc(argLength+1);
			memcpy(Input,tp,argLength+1);
		}
	}
    
#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Donn�es lues");
#endif

	contentType=getenv("CONTENT_TYPE");

	if (!contentType ||*contentType==0)
	{
#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Content-type ind�fini");
#endif
	}
	else
	{
#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Content-type:");
		HTML_Erreur(contentType);
#endif
	}	

	if ((contentType==NULL) || (memcmp(contentType,"multipart/form-data",19)))
		Param_URL(Input);
	else
		Param_MultiPart("",Input,argLength,"",contentType);

#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Op�ration termin�e");
#endif
	free(Input);
}*/
