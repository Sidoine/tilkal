#ifndef TMAXNOM
#define TMAXNOM 32
#endif
#include "tableaux.h"

namespace TilXML
{

int EtendreElements(char * v, int l);
	
class CXML;

class CObjet
{
public:
	char * Balise;
	char ** Clef;
	char ** Valeur;
	int nClefs,aClefs;
	CObjet * Fils;
	CObjet * Suivant, * Precedent;
	CObjet * Pere;
	CObjet * Dernier;
	bool Recree;
	CXML * XML;
	int Id;

	void Detacher();
	void Attacher(CObjet * _Pere);
	CObjet(CObjet * _Pere,CXML* XML);
	
	virtual void Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur);
	void AfficherStruct(int Espaces);
	bool LireBalise(char * &Mem);
	bool LireContenu(char * &Mem);
	CObjet * ChercherFils(char * NomBalise,CObjet * Premier);
	int NombreFils(char * NomBalise);
	virtual bool Recreer();

	virtual ~CObjet();
};

class CTexteXML:public CObjet
{
public:
	char * Texte;
	int lTexte;
	CTexteXML(CObjet * _Pere, CXML * _XML):CObjet(_Pere,_XML){Texte=NULL;}
	CTexteXML(CObjet * _Pere, CXML * _XML, char * _Texte, int _lTexte):CObjet(_Pere,_XML)
	{
		lTexte=_lTexte;
		Texte=new char[lTexte+1];
		memcpy(Texte,_Texte,lTexte);
		lTexte=EtendreElements(Texte,lTexte);
		Texte[lTexte]=0;
	}
	~CTexteXML()
	{
		if (Texte)
			delete Texte;
	}
};

class CIdPtr
{
public:
	int lId;
	char Id[TMAXNOM];
	CObjet * o;
};

class CXML
{
public:
	char NomRacine[TMAXNOM];

	TTableau<CIdPtr> IdPtr;

	CObjet * LireFichier(char * Fichier);
	CObjet * LireMemoire(char * Mem);
	
	int ChercherId(char * Nom, int l);
	int AjouterId(char * Nom, int l, CObjet * o);
	
	virtual CObjet * CreerBalise(char * Nom, CObjet * Pere);

	CXML()
	{
		*NomRacine=0;
	}
};

//Compare deux chaînes a de longueur al avec b (zéro terminal)
#define EGALES(a,al,b) ((strncmp(a,b,al)==0)&&(b[al]==0))
#define EGALES2(a,al,b) ((sizeof(b)-1==al) && (memcmp(a,b,al)==0))

inline bool ValeurABool(char * Valeur, size_t lValeur)
{
	if (EGALES2(Valeur,lValeur,"true"))
		return true;
	else if (EGALES2(Valeur,lValeur,"vrai"))
		return true;
	else if (EGALES2(Valeur,lValeur,"1"))
		return true;
	else
		return false;
}

#define TEXTEXML "$"

template<class T>
class Interv
{
public:
	T min,max;
	bool amin,amax;
	void MinMax(T _min, T _max)
	{
		min=_min;amin=true;
		max=_max;amax=true;
	}
	void Max(T _max)
	{
		amin=false;
		max=_max;amax=true;
	}
	void Min(T _min)
	{
		min=_min;amin=true;
		amax=false;
	}
	void NoLimit()
	{
		amin=amax=false;
	}
	bool Contient(T a)
	{
		return ((!amin || min<=a)&&(!amax || max>=a));
	}
};

}
