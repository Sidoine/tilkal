#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif
#include "form.h"


THachage<CFormVar> Form::Champ;

inline void SauterL(const char * &x)
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
void ExtraitOptionMeta(char * &Meta, char * &OptionType , char * &Option)
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

//Extrait un méta du contenu
void ExtraitMeta(char * & contenu, char * & metaNom, char * & metaVal)
{
	metaNom=contenu;

	//On cherche les :
	while ((*contenu) && (*contenu!='\r') && (*contenu!='\n') && (*contenu!=':'))
		contenu++;

	//On les trouve => suivi par la valeur
	if (*contenu==':')
	{
		*(contenu++)=0;//On termine le nom du méta

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

char * cherche(char *a, char *b, size_t l)
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

bool Form::Param_MultiPart(char * NomVariable,
						   char * contenu, size_t longueur, 
						   char * NomFichier,
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
		HTML_Erreur("NOT Variable créée");
	#endif
		return true;
	}

	Temp=ExtraitValeurMeta(contentType);
	if (!*Temp)
		return false;//Méta mal formé

    if (strcmp(Temp,"multipart/form-data")&&
		strcmp(Temp,"multipart/mixed"))
	{
		//Type non-supporté
		return false;
	}

	char* boundary=NULL;
	while (*contentType)
	{
		char * Option,*OptionVal;
        ExtraitOptionMeta(contentType,Option,OptionVal);
		if (strcmp(Option,"boundary")==0)
			boundary=OptionVal;
	}


#ifdef DEBUG_PARAM
	HTML_Message("NOT Multipart:");
#endif
	if (!boundary || !*boundary)
		return false;//HTML_Erreur("Le délimiteur des données du formulaire n'est pas défini.");

#ifdef DEBUG_PARAM
	HTML_Message("NOT Boudary :");
	HTML_Message(boundary);
#endif

	
	do
	{
		contentType=NULL;
		NomFichier=NULL;

		Depart=contenu;
		//On saute le boundary précédé de deux -
		contenu+=2+strlen(boundary);
		//Si le boundary est suivi d'un tiret on met fin à la boucle
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
					HTML_Message("NOT Content-type défini");
#endif
				}
				else if (strcmp(metaNom,"content-disposition")==0)
				{
#ifdef DEBUG_PARAM
					HTML_Message("NOT Content-disposition défini");
#endif
					while (*metaVal)
					{
						char * Option,*OptionVal;
						ExtraitOptionMeta(metaVal,Option,OptionVal);
						if (strcmp(Option,"name")==0)
							NomVariable=OptionVal;
						else if(strcmp(Option,"filename")==0)
							NomFichier=OptionVal;
					}
				}
#ifdef DEBUG_PARAM
				HTML_Message("NOT -- Analyse du méta terminée");
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

bool Form::Variable_Creer(char * Nom, char * Contenu, size_t Longueur, char * _NomFichier,
								 char * _ContentType)
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
// Renvoie la valeur d'un caractère correspondant à un chiffre hexadécimal
{
	if ((c>='0') && (c<='9')) return (c-'0');
	if ((c>='a') && (c<='f')) return (c-'a'+10);
	if ((c>='A') && (c<='F')) return (c-'A'+10);
	return 0;
}

inline void TranslateEscapes(char *Str)
// Convertis les %XX en caractères dont le XX est la valeur ASCII
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

bool Form::Param_URL(char * Input)
//Décodage des paramètres codés URL
{
	// les variables sont séparées par le caractére "&" 

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
			Variable_Creer(NomVar,ValVar,strlen(ValVar),"","text/plain");
		}
		else
		{
			if (*pToken) *(pToken++)=0;
			Variable_Creer(NomVar,"",0,"","text/plain");
		}
	}

	return true;
}

void Form::Init()
//Lecture des paramètres
{
#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Lecture des paramètres :");
#endif
	char *contentType=NULL;
	size_t argLength;
	char *Input;

	char *requestMethod =getenv("REQUEST_METHOD");

	if (requestMethod && stricmp(requestMethod,"POST")==0)
	{
		// Méthode POST
		//On lit les données envoyées dans l'en-tête
		argLength=atoi(getenv("CONTENT_LENGTH"));

	#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Longueur des données");
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
		// Méthode GET
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
	HTML_Erreur("NOT Données lues");
#endif

	contentType=getenv("CONTENT_TYPE");

	if (!contentType ||*contentType==0)
	{
#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Content-type indéfini");
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
	HTML_Erreur("NOT Opération terminée");
#endif
	free(Input);
}
