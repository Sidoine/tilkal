#include <stdio.h>
#include <stdlib.h>

class CBaseMaillon
{
public:
	size_t Precedent,Suivant;
	
};

#define RIEN ~0

template<class T, size_t Extension>
class TListeChainee
{
	T * t;
	size_t a;
	size_t l;
	size_t Vide;
	
	TTableauV()
	{
		Vide=l=a=0;
		t=NULL;
	}
	void Init()
	{
		Vide=l=a=0;
		t=NULL;
	}

public:
	void Supprimer(size_t i, size_t& Aine)
	{
		if (t[i].Suivant!=RIEN)
			t[t[i].Suivant].Precedent=t[i].Precedent;
	
		if (t[i].Precedent!=RIEN)	
			t[t[i].Precedent].Suivant=t[i].Suivant;
		if (Aine==i)
			Aine=t[i].Suivant;
		t[i].Suivant=Vide;
		Vide=i;
	}
	
	void Nouveau(size_t &Aine)
	{
		size_t ret=Vide;
		if (Vide==l)
		{
			if (l==a)
			{
				a+=Extension;
				t=(T*)realloc(t,a*sizeof(T));
			}
			l++;
			Vide++;
		}
		else
			Vide=t[Vide].Suivant;
		t[ret].Suivant=Aine;
		t[ret].Precedent=RIEN;
		if (Aine!=RIEN)
			t[Aine].Precedent=ret;
		Aine=ret;
	}

	T & operator[](size_t i)
	{
		return t[i];
	}	
};

class SExemple:public CBaseMaillon
{
public:
	int i;	
};

TListeChainee<SExemple,16> Test;

int main()
{
	size_t Aine,ac;
	
	Aine=RIEN;
	Test.Nouveau(Aine);
	Test[Aine].i=18;
	Test.Nouveau(Aine);
	Test[Aine].i=15;
	Test.Nouveau(Aine);	
	Test[Aine].i=3;
	
	for (ac=Aine;ac!=RIEN;ac=Test[ac].Suivant)
		printf("%d,",Test[ac].i);
	puts("");
	
	for (ac=Aine;ac!=RIEN;ac=Test[ac].Suivant)
	{
		if (Test[ac].i==18)
		{
			Test.Supprimer(ac,Aine);
			break;
		}
	}
	
	for (ac=Aine;ac!=RIEN;ac=Test[ac].Suivant)
		printf("%d,",Test[ac].i);
	puts("");
	return 0;
}
