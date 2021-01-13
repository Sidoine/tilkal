#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "xml.h"

using namespace TilXML;

CObjet * CXML::LireFichier(const char * NomFichier)
{
	FILE * F=fopen(NomFichier,"rb");
	if (!F)
		return NULL;
	fseek(F,0,SEEK_END);
	int l=ftell(F);
	fseek(F,0,SEEK_SET);
	char * Texte=new char[l+1];
	fread(Texte,l,1,F);
	fclose(F);
	Texte[l]=0;
	CObjet * ret=LireMemoire(Texte);
	delete Texte;
	return ret;
}

typedef unsigned char BYTE;

inline bool ESPACE(char x)
{
	return BYTE(x)<=32;
}
inline bool ALPHA(char x)
{
	return (( (x)>='A')&&((x)<='Z'))||(((x)>='a')&&((x)<='z'));
}
inline bool CHIFFRE(char x)
{
	return (( (x)>='0')&&((x)<='9'));
}
inline bool ALPHANUM(char x)
{
	return (ALPHA(x)||CHIFFRE(x));
}

CObjet * CXML::LireMemoire(char * Mem)
{
	char * Depart=Mem;
	while (ESPACE(*Mem))
		Mem++;

	if (*Mem=='<')
	{
		const char * NomBalise=Mem;
		int lBalise=0;
		while (ALPHANUM(*Mem))
		{
			Mem++;
			lBalise++;
		}
		if ((strncmp(NomRacine,NomBalise,lBalise)==0)&&(NomRacine[lBalise]==0))
		{
			CObjet * Racine=new CObjet(NULL,this);
			if (Racine->LireBalise(Mem))
				return Racine;
			else
			{
				delete Racine;
				return NULL;
			}			
		}			
	}
	CObjet * Racine=new CObjet(NULL,this);
	Racine->Balise="racine";
	if (Racine->LireContenu(Depart))
		return Racine;

	delete Racine;
	return NULL;
}

int CXML::ChercherId(const char * Nom, int l)
{
	CIdPtr *p=IdPtr.t;
	int s=0;
	while (s<(int)IdPtr.l)
	{
		if (l==p->lId && strncmp(Nom,p->Id,l)==0)
		{
			break;
		}
		p++;
		s++;
	}
	return s;
}

int CXML::AjouterId(const char * Nom, int l, CObjet * o)
{
	int i=ChercherId(Nom,l);
	if (i==(int)IdPtr.l)
	{
		IdPtr.Etendre();
		memcpy(IdPtr[i].Id,Nom,l);
		IdPtr[i].Id[l]=0;
		IdPtr[i].lId=l;
	}
	IdPtr[i].o=o;
	return i;
}

void CObjet::AfficherStruct(int Espaces)
{
	for (int e=0; e<Espaces; e++)
		fputs(" ",stdout);
	fputs(Balise,stdout);
	for (int a=0; a<nClefs; a++)
	{
		printf(" %s=\"%s\"",Clef[a],Valeur[a]);
	}
	puts("");

	if (Fils)
		Fils->AfficherStruct(Espaces+1);
	if (Suivant)
		Suivant->AfficherStruct(Espaces);
}

struct SElement
{
	char Nom[16];
	char Valeur[16];
} Element[]={{"agrave","à"},{"amp","&"},{"eacute","é"},{"egrave","è"},{"gt",">"},{"lt","<"},{"quot","\""}};

int TilXML::EtendreElements(char * v, int l)
{
	int a=0;
	char Nom[16];
	char * dest=v;
	while (l)
	{
	 	if (*v=='&')
	 	{
	 		v++;
	 		l--;
			int i=0;
			while ((*v!=';')&&(l)&&(i<15))
			{
				Nom[i]=*(v++);
				i++;
				l--;
			}
				
			Nom[i]=0;
			if ((l==0)||(i==15))
			{
				fprintf(stderr,"Avertissement : l'entité %s doit se terminer par un point-virgule.",Nom);
				return a;
			}

			SElement * s=(SElement*)bsearch(Nom,Element,sizeof(Element)/sizeof(SElement),sizeof(SElement),(int (*)(const void *, const void *))strcmp);
			if (s)
			{
				const char *src = s->Valeur;
				while (*src) {
					*(dest++) = *(src++);
				}
				a++;
			}
			else
			{
				fprintf(stderr,"Avertissement : l'entité %s n'est pas connue.",Nom);
				*(dest++)='?';
				a++;
			}
			v++;
			l--;
	 	}
	 	else
	 	{
	 		if (v!=dest)
	 			*dest=*v;
	 		a++;
	 		v++;
	 		dest++;
	 		l--;
	 	}	 		
	}
	return a;
}

bool CObjet::LireBalise(char * &Mem)
{
	while (ESPACE(*Mem))
			Mem++;
		
	while (*Mem!='>')
	{
		if (!*Mem)
		{
			printf("Pas de fin pour la balise %s\n",Balise);
			return false;
		}

		if ((*Mem=='/') && (Mem[1]=='>'))
		{
			Mem+=2;
			return true;
		}

		if (!ALPHANUM(*Mem))
		{
			printf("Caractère %c erroné dans la balise %s\n",*Mem,Balise);
			return false;
		}

		char * Attr=Mem;
		int lAttr=0;
		while (ALPHANUM(*Mem))
		{
			Mem++;
			lAttr++;
		}
		while (ESPACE(*Mem))
			Mem++;
		if (*Mem!='=')
		{
			Attr[lAttr]=0;
			printf("Pas de = pour l'attribut %s d'une balise %s\n", Attr, Balise);
			return false;
		}
		Mem++;
		while (ALPHANUM(*Mem))
			Mem++;
		if (*Mem!='"')
		{
			Attr[lAttr]=0;
			printf("Pas de \" pour l'attribut %s d'une balise %s\n",Attr, Balise);
			return false;
		}
		Mem++;

		char * Val=Mem;
		int lVal=0;
		while ((*Mem)&&(*Mem!='"'))
		{
			Mem++;
			lVal++;
		}
		if (*Mem!='"')
		{
			Attr[lAttr]=0;
			printf("Pas de \" fermant pour l'attribut %s d'une balise %s\n",Attr,Balise);
			return false;
		}
		Mem++;
		lVal=EtendreElements(Val,lVal);
		Ajouter(Attr,lAttr,Val,lVal);

		while (ESPACE(*Mem))
			Mem++;
	}
	Mem++;
	return 	LireContenu(Mem);
}

bool CObjet::LireContenu(char * &Mem)
{
	int lTexte=0;
	char * DebutTexte;
	
	while (*Mem)
	{
		if (*Mem=='<')
		{
			if (lTexte)
			{
				CTexteXML * Texte=new CTexteXML(this,XML,DebutTexte,lTexte);
				Texte->Balise=TEXTEXML;
				lTexte=0;
			}
			Mem++;
			if ((*Mem=='!')&&(Mem[1]=='-')&&(Mem[2]=='-'))
			{
				do
				{
					Mem++;
				}
				while ((*Mem)&&((Mem[0]!='-')||(Mem[1]!='-')||(Mem[2]!='>')));
				if (!Mem)
				{
					puts("Un commentaire ne se termine pas.");
					return false;
				}
				Mem+=3;
				continue;
			}
			bool Fermeture;
			if (*Mem=='/')
			{
				Mem++;
				Fermeture=true;
			}
			else
				Fermeture=false;
			char * NomBalise=Mem;
			int lBalise=0;
			while (ALPHANUM(*Mem)||(*Mem==':'))
			{
				Mem++;
				lBalise++;
			}
			if (Fermeture)
			{
				if ((strncmp(NomBalise,Balise,lBalise)==0)&&(Balise[lBalise]==0))
				{
					while ((*Mem)&&(*Mem!='>'))
						Mem++;
					if (*Mem=='>')
						Mem++;
					return true;
				}
				else
				{
					*Mem=0;
					printf("ERREUR : Une balise %s est termin�e par une balise %s !\n",
						Balise,NomBalise);
					return false;
				}
			}
			char * nom=new char[lBalise+1];
			memcpy(nom,NomBalise,lBalise);
			nom[lBalise]=0;

			CObjet * n=XML->CreerBalise(nom,this);//new CObjet(this);
			n->Balise=nom;
			if (!n->LireBalise(Mem))
				return false;
			if (n->Recree)
			{
				if (n->Recreer())
					delete n;
			}
		}
		else if (lTexte)
		{
			lTexte++;
			Mem++;
		}
		else if (ESPACE(*Mem))
			Mem++;
		else
		{
			DebutTexte=Mem++;
			lTexte=1;
		}
	}
	return true;
}

bool CObjet::Recreer()
{
	return false;
}

void CObjet::Attacher(CObjet * _Pere)
{
	Pere=_Pere;
	if (Pere)
	{
		Precedent=Pere->Dernier;
		if (!Precedent)			
			Pere->Fils=Pere->Dernier=this;
		else
			Pere->Dernier=Precedent->Suivant=this;
	}
	else
		Precedent=NULL;
	Suivant=NULL;
}

CObjet::CObjet(CObjet * _Pere, CXML*_XML)
{
	XML=_XML;
	Attacher(_Pere);
	nClefs=aClefs=0;
	Clef=NULL;
	Valeur=NULL;
	Dernier=NULL;
	Fils=NULL;
	Recree=false;
}

void CObjet::Detacher()
{
	if (Pere)
	{
		if (!Precedent)
			Pere->Fils=Suivant;
		if (!Suivant)
			Pere->Dernier=Precedent;
		if (Suivant)
			Suivant->Precedent=Precedent;
		if (Precedent)
			Precedent->Suivant=Suivant;
	}
}

CObjet::~CObjet()
{
	for (int c=0; c<nClefs; c++)
	{
		delete Clef[c];
		delete Valeur[c];
	}
	free(Clef);
	free(Valeur);
	while (Fils)
		delete Fils;
	Detacher();
}

void CObjet::Ajouter(const char * _Clef, int lClef, const char * _Valeur, int lValeur)
{
	if (EGALES2(_Clef,lClef,"id"))
	{
		Id=XML->AjouterId(_Valeur,lValeur,this);
		return;
	}
	
	if (nClefs==aClefs)
	{
		aClefs+=8;
		Clef=(char**)realloc(Clef,aClefs*sizeof(char**));
		Valeur=(char**)realloc(Valeur,aClefs*sizeof(char**));
	}
	Clef[nClefs]=new char[lClef+1];
	memcpy(Clef[nClefs],_Clef,lClef);
	Clef[nClefs][lClef]=0;
	Valeur[nClefs]=new char[lValeur+1];
	memcpy(Valeur[nClefs],_Valeur,lValeur);
	Valeur[nClefs][lValeur]=0;
	nClefs++;
}

CObjet * CObjet::ChercherFils(const char * NomBalise,CObjet * Premier)
{
	if (Premier==NULL)
		Premier=Fils;
	else
		Premier=Premier->Suivant;
	while (Premier)
	{
		if (strcmp(Premier->Balise,NomBalise)==0)
			return Premier;
		Premier=Premier->Suivant;
	}
	return NULL;
}

int CObjet::NombreFils(const char * NomBalise)
{
	int ret=0;
	CObjet * f=Fils;
	while (f)
	{
		if (strcmp(f->Balise,NomBalise)==0)
			ret++;
		f=f->Suivant;
	}
	return ret;
}

struct CXIInclude: public CObjet
{
	char * href;
	void Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur);
	bool Recreer();
	CXIInclude(CObjet * _Pere, CXML * _XML):CObjet(_Pere,_XML)
	{
	 	Recree=true;
	 	href=NULL;
	}
	~CXIInclude()
	{
		if (href)
			delete href;
		Detacher();
	}
};


void CXIInclude::Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur)
{
	if (EGALES2(_Clef,lClef,"href"))
	{
		href=new char[lValeur+1];
		memcpy(href,_Valeur,lValeur);
		href[lValeur]=0;
	}
	else
		CObjet::Ajouter(_Clef,lClef,_Valeur,lValeur);
}

bool CXIInclude::Recreer()
{
	CObjet * Racine=XML->LireFichier(href);
	if (!Racine)
	{
		fprintf(stderr, "Erreur lors de l'inclusion du fichier\n");
		return false;
	}
	CObjet * Fils=Racine->Fils;
	while (Fils)
	{
		CObjet * Suivant=Fils->Suivant;
		Fils->Detacher();
		Fils->Attacher(Pere);
		Fils=Suivant;
	}
	delete Racine;
	return true;
}

CObjet * CXML::CreerBalise(const char * Nom, CObjet * Pere)
{
	if (strcmp(Nom,"xi:include")==0)
		return new CXIInclude(Pere,this);
	else
		return new CObjet(Pere,this);
}
