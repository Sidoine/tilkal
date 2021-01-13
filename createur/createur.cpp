#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <iostream>
#include <string>
#include "../tableaux.h"
#include "../tilkal.h"
#include "../doors.h"
#include "../objects.h"
#include "../xml.h"
#include "createur.h"
#define RIEN -1

using namespace std;
using namespace Createur;
using namespace TilXML;
using namespace Tilkal;

#ifdef _WIN32
double rint(double f)
{
	if (f>0)
	{
		if (f-floor(f)>0.5)
			return floor(f)+1;
		else
			return floor(f);
	}
	else
	{
		if (ceil(f)-f>0.5)
			return ceil(f)-1;
		else
			return ceil(f);
	}
}
#endif

namespace Createur
{

class CConfigMonde:public CXML
{
public:
	CObjet * CreerBalise(char * Nom, CObjet * Pere);
	CConfigMonde():CXML()
	{
		strcpy(NomRacine,"genese");
		LireFichier("test.xml");
	}	
} ConfigMonde;

class CGenese:public CObjet
{
public:
	void Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur);
	CGenese(CObjet * _Pere, CXML * _XML):CObjet(_Pere,_XML)
	{
	}
	~CGenese()
	{
	}
};
void CGenese::Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur)
{
	CObjet::Ajouter(_Clef,lClef,_Valeur,lValeur);
}

class CSalle:public CObjet
{
public:
	char * Titre;
	void Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur);
	CSalle(CObjet * _Pere, CXML * _XML):CObjet(_Pere,_XML)
	{
		Titre=NULL;
	}
	~CSalle()
	{
		if (Titre)
			delete Titre;
		//CObjet::~CObjet();
	}
};

void CSalle::Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur)
{
	if (EGALES2(_Clef,lClef,"titre"))
	{
		Titre=new char[lValeur+1];
		memcpy(Titre,_Valeur,lValeur);
		Titre[lValeur]=0;
	}
	else
		CObjet::Ajouter(_Clef,lClef,_Valeur,lValeur);

}

class CAvecSalle:public CObjet
{
public:
	int Quoi;
	void Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur);
	CAvecSalle(CObjet * _Pere, CXML * _XML):CObjet(_Pere,_XML)
	{
	}
	~CAvecSalle()
	{
	}
};
	
CObjet * CConfigMonde::CreerBalise(char * Nom, CObjet * Pere)
{
	return CXML::CreerBalise(Nom,Pere);
}

struct SPoint
{
	float x,y;
	float operator*(const SPoint & p)
	{
		return x*p.x+y*p.y;
	}
	SPoint operator-(const SPoint & p)
	{
		SPoint ret;
		ret.x=x-p.x;
		ret.y=y-p.y;
		return ret;
	}
	void operator/=(float f)
	{
		x/=f;y/=f;
	}
	float Norme()
	{
		return sqrtf(x*x+y*y);
	}
};

struct SSalle:public SPoint
{
	int X,Y;
	int Valeur;
	int Porte;
	int nPortes;
	int Id;
	char nDirec[8];
	int Altitude;
	int Type;
};

struct SPorte
{
	int a,b;
	SPoint Direction;
	float Longueur;
	int aSuiv,aPrec,bSuiv,bPrec;
	int aAngle,bAngle;
	bool Existe;
	int Type;
	
	int Suivante(int s)
	{
		if (a==s)
			return aSuiv;
		else
			return bSuiv;
	}
	void SauverM();
	string Decrire(int s);
};
}

TTableau<SPorte> Porte;
TTableau<SSalle> Salle;

string DirecTxt[]=
{"ouest","sud-ouest","sud","sud-est",
"est","nord-est","nord","nord-ouest"};

string SPorte::Decrire(int s)
{
	string Nom;
	int t=(s==a?b:a);
	bool Feminin=true,Pluriel=false;
	if (abs(Salle[s].Altitude-Salle[t].Altitude)>50)
	{
		Nom="escaliers";
		Feminin=false;
		Pluriel=true;
	}
	else
	{
		switch(Type & 3)
		{
		case 0:
			Nom="petite ruelle";
			break;
		case 1:
			Nom="ruelle";
			break;
		case 2:
			Nom="rue";
			break;
		case 3:
			Nom="large rue";
			break;
		}
	}
	string Pente;
	if (abs(Salle[s].Altitude-Salle[t].Altitude)<10)
		Pente=" va";
	else if (Salle[t].Altitude>Salle[s].Altitude+20)
		Pente=" descend en pente raide";
	else if (Salle[t].Altitude>Salle[s].Altitude)
		Pente=" descend";
	else if (Salle[t].Altitude<Salle[s].Altitude-20)
		Pente=" monte";
	else if (Salle[t].Altitude<Salle[s].Altitude)
		Pente=" monte en pente raide";
	string Direc;
	if (a==s)
		Direc=DirecTxt[aAngle];
	else
		Direc=DirecTxt[bAngle];
	string ret=Nom+Pente+" vers "+Direc+". ";
	if (ret[0]>='a' && ret[0]<='z')
		ret[0]+='A'-'a';
	return ret;
}


class CCreateur
{
public:
	int nMaxPortesParSalle;
	int SurPercolation; //En pourcentage
	int nSalles;
	int Ici;
	int NombrePerc;
	int pSalle,pPorte,nPortes;
	int nMailles;
	
	CCreateur()
	{
		nMaxPortesParSalle=4;
		SurPercolation=110;
		nSalles=30;
		Ici=RIEN;
		NombrePerc=0;
		pSalle=pPorte=nPortes=0;
		nMailles=10;
	}
	
	bool CreerZone();
	bool SauverM();
	void Percole(int i);
};

#define PI 3.14159

void AjouterPorteDansSalles(int d, int a, int b)
{
	int aSuiv,bSuiv;
	Porte[d].a=a;
	aSuiv=Salle[a].Porte;
	if (aSuiv!=RIEN)
	{
		if (Porte[aSuiv].a==a)
			Porte[aSuiv].aPrec=d;
		else
			Porte[aSuiv].bPrec=d;
	}
	Porte[d].aSuiv=aSuiv;
	Porte[d].aPrec=RIEN;
	Salle[a].Porte=d;
	Salle[a].nPortes++;

	Porte[d].b=b;
	bSuiv=Salle[b].Porte;
	if (bSuiv!=RIEN)
	{
		if (Porte[bSuiv].b==b)
			Porte[bSuiv].bPrec=d;
		else
			Porte[bSuiv].aPrec=d;
	}
	Porte[d].bSuiv=bSuiv;
	Porte[d].bPrec=RIEN;
	Salle[b].Porte=d;
	Salle[b].nPortes++;
	
	Porte[d].aAngle=(int)rint((atan2(Porte[d].Direction.y,Porte[d].Direction.x)+PI)*4/PI);
	if (Porte[d].aAngle>7)
		Porte[d].aAngle-=8;
	Salle[a].nDirec[Porte[d].aAngle]++;
	Porte[d].bAngle=(int)rint((atan2(-Porte[d].Direction.y,-Porte[d].Direction.x)+PI)*4/PI);
	if (Porte[d].bAngle>7)
		Porte[d].bAngle-=8;
	Salle[b].nDirec[Porte[d].bAngle]++;	
}

void CCreateur::Percole(int i)
{
	int a=Porte[i].a;
	int b=Porte[i].b;
	if (Salle[a].Valeur && !Salle[b].Valeur)
	{
		NombrePerc--;
		Salle[b].Valeur=1;
		//On percole chaque Porte
		int d=Salle[b].Porte;
		while (d!=RIEN)
		{
			if (d!=i)
				Percole(d);
			d=Porte[d].Suivante(b);
		}
	}
	
	if (Salle[b].Valeur && !Salle[a].Valeur)
	{
		NombrePerc--;
		Salle[a].Valeur=1;
		//On percole chaque Porte
		int d=Salle[a].Porte;
		while (d!=RIEN)
		{
			if (d!=i)
				Percole(d);
			d=Porte[d].Suivante(a);
		}
	}
}

/*

	Crée une zone
	
*/

bool CCreateur::CreerZone()
{
	printf("Salle.l=%d\nPorte.l=%d\n",Salle.l,Porte.l);
	pSalle=(int)Salle.l;
	
	Salle.Redim(nSalles);
			
	NombrePerc=nSalles-1;
	
	int AltMoy=0,AltMax=100;
	
	for (int i=pSalle; i<pSalle+nSalles; i++)
	{
		
		bool ok;
		do
		{
			Salle[i].X=rand()%nMailles;
			Salle[i].Y=rand()%nMailles;
			//On vérifie que le Salle n'existe pas déjà
			ok=true;
			for (int j=pSalle;j<i; j++)
			{
				if (Salle[i].X==Salle[j].X && Salle[i].Y==Salle[j].Y)
				{
					ok=false;
					break;
				}
			}
		}
		while (!ok);
		Salle[i].x=float(Salle[i].X)/nMailles;
		Salle[i].y=float(Salle[i].Y)/nMailles;
		Salle[i].Porte=RIEN;
		Salle[i].nPortes=0;
		Salle[i].Altitude=AltMoy;
		Salle[i].Type=rand();
		Salle[i].Valeur=0;
	}
	Salle.l+=nSalles;
	
	int Eminence[4];
	//On crée des éminences
	for (int e=0; e<3; e++)
	{
		int i=Eminence[e]=rand()%nSalles+pSalle;
		Salle[i].Altitude=AltMoy+rand()%(AltMax-AltMoy);
	}
	
	for (int i=pSalle; i<pSalle+nSalles; i++)
	{
		for (int e=0; e<3; e++)
		{
			int j=Eminence[e];
			SPoint Diff=Salle[j]-Salle[e];
			float dist=Diff.Norme();
			int ef=int(Salle[j].Altitude-AltMoy-dist*100);
			if (ef>0)
				Salle[i].Altitude+=ef+AltMoy;
		}
		Salle[i].Altitude/=5;
	}
	
	Salle[pSalle].Valeur=1;
	
	nPortes=0;
	int i=(int)Porte.l;
	int nEssais=0;
	float Longueur=0.1f;
	pPorte=(int)Porte.l;
#define	SURPERC 12
#define NMAXESSAIS 1000

	while (nEssais<NMAXESSAIS && (NombrePerc || i<nPortes))
	{
		printf("%d : ",i);
		bool ok;
		nEssais=0;
		Porte.Etendre();
			
		do
		{
			nEssais++;
			ok=true;
			int a,b;
			do
			{
				Porte[i].a=a=rand()%nSalles+pSalle;
			}
			while (Salle[a].nPortes==nMaxPortesParSalle);
			
			do
			{
				Porte[i].b=b=rand()%nSalles+pSalle;
			}
			while (Salle[b].nPortes==nMaxPortesParSalle);
				
			if (a==b)
			{
				ok=false;
				continue;
			}
			Porte[i].Direction=Salle[b]-Salle[a];
			Porte[i].Longueur=Porte[i].Direction.Norme();
			Porte[i].Direction/=Porte[i].Longueur;
			if (Porte[i].Longueur>Longueur)
			{
				//printf("%d-%d trop long\n",a,b);
				Longueur*=1.01f;
				ok=false;
				continue;
			}

			//On vérifie qu'aucun Salle n'appartient à la Porte
			SPoint orig=Salle[Porte[i].a];
			SPoint fin=Salle[Porte[i].b];
			if (fabsf(orig.x-fin.x)<0.00001)
			{
				float x=orig.x;
				float ymin=orig.y;
				float ymax=fin.y;
				if (ymin>ymax)
				{
					ymax=ymin;
					ymin=fin.y;
				}
				for (int p=pSalle; p<nSalles; p++)
				{
					if (a==p || b==p)
						continue;
					if (fabsf(Salle[p].x-x)<0.01
						&& Salle[p].y>=ymin && Salle[p].y<=ymax)
					{
						ok=false;
						break;
					}
				}
			}
			else
			{
				float pente=(fin.x-orig.x)/(fin.y-orig.y);
				float oo=orig.y-pente*orig.x;
				float xmin=orig.x;
				float xmax=fin.x;
				if (xmin>xmax)
				{
					xmax=xmin;
					xmin=fin.x;
				}
				for (int p=pSalle; p<pSalle+nSalles; p++)
				{
					if (a==p || b==p)
						continue;
					if (fabsf(Salle[p].x*pente+oo-Salle[p].y)<0.0001
						&& Salle[p].x>=xmin && Salle[p].x<=xmax)
					{
						ok=false;
						break;
					}
				}
			}
			if (!ok)
				continue;
				
			//ON vérifie qu'on croise pas une autre Porte			
			for (int j=pPorte; j<i; j++)
			{
				float ab=Porte[j].Direction*Porte[i].Direction;

				float baa=(Salle[Porte[j].a]-Salle[Porte[i].a])*Porte[i].Direction;
				float bab=(Salle[Porte[j].a]-Salle[Porte[i].a])*Porte[j].Direction;
				
				float div=ab*ab-1;
				if (fabsf(div)<0.0001)
				{
					//Colinéaires
					if ((Porte[j].a==Porte[i].a
						&& Porte[i].b==Porte[j].b)
						||(Porte[j].b==Porte[i].a
						&& Porte[i].a==Porte[j].b))
					{
						ok=false;
						break;
					}
					else
						continue;
				}
				float beta=(bab-baa*ab)/div;
				float alpha=beta*ab+baa;
				if ((alpha>0.001 && alpha<0.9999*Porte[i].Longueur)
				 && (beta>0.001 && beta<0.9999*Porte[j].Longueur))
				{
					//printf("%d,%d se croisent\n",i,j);
					ok=false;
					break;
				}
			}
		}
		while ((!ok)&&(nEssais<NMAXESSAIS));
		if (nEssais<NMAXESSAIS)
		{
			Porte[i].aPrec=Porte[i].aSuiv=Porte[i].bPrec=
					Porte[i].bSuiv=RIEN;
			Porte[i].Existe=true;
			Porte[i].Type=rand();
			AjouterPorteDansSalles(i,Porte[i].a,Porte[i].b);
			//On regarde si ça percole
			if (NombrePerc)
			{
				Percole(i);
				nPortes=(i-pPorte)*SurPercolation/100+pPorte;
			}
			printf("%d\n",NombrePerc);
			i=i+1;
		}
	}
	Porte.l+=nPortes;
	printf("\n");
	if (nEssais==NMAXESSAIS)
	{
		printf("Raté\n");
		return false;
	}
	SauverM();
	return true;
}

bool CCreateur::SauverM()
{
	FILE * F=fopen("/home/pccronos/sidoine/temp/test.m","w");
	if (F==NULL)
	{
		printf("erreur");
		return false;
	}
	fprintf(F,"hold on\n");
	for (int i=pSalle; i<pSalle+nSalles ;i++)
	{
		fprintf(F,"plot([%f],[%f],'*b-')\n",
				Salle[i].x,Salle[i].y);
	}
	for (int i=pSalle; i<pPorte+nPortes; i++)
	{
		fprintf(F,"plot([%f, %f],[%f, %f],'*r-')\n"
				,Salle[Porte[i].a].x,Salle[Porte[i].b].x,
				Salle[Porte[i].a].y,Salle[Porte[i].b].y);
	}
	fclose(F);
	return true;
}



		/*{"est","nord-est","nord","nord-ouest",
		"ouest","sud-ouest","sud","sud-est"};*/

void Createur::CreerUnivers(int Debut)
{
	//Auberge : rat mort
	//Ville : canard borgne
	//Plaine : castor à trois pattes
	
	CCreateur Createur;
	if (!Createur.CreerZone())
	{
		Salle.Detruire();
		Porte.Detruire();
		return;
	}

	char Desc[512],Desc2[512];
	int premiere=-1;
	for (int s=0; s<(int)Salle.l; s++)
	{
		if (!Salle[s].Valeur)
			continue;
		
		string desc;
		
		sprintf(Desc,"En (%f,%f). Altitude : %d m.\r\n",Salle[s].x,Salle[s].y,
				Salle[s].Altitude);
		desc=Desc;
		for(int p=Salle[s].Porte; p!=RIEN; p=Porte[p].Suivante(s))
		{
			desc+=Porte[p].Decrire(s);
		}
		desc+="\r\n";
		sprintf(Desc,"Place %d",s);
		Salle[s].Id=Object.New(Desc,desc.c_str());
		if (premiere==-1)
			premiere=Salle[s].Id;
	}
	
	for (int p=0; p<(int)Porte.l; p++)
	{
		if (!Porte[p].Existe)
			continue;
		strcpy(Desc,DirecTxt[Porte[p].aAngle].c_str());
		size_t l=DirecTxt[Porte[p].aAngle].length();
		if (Salle[Porte[p].a].nDirec[Porte[p].aAngle]>1)
		{
			Desc[l++]='-';
			Desc[l++]=--Salle[Porte[p].a].nDirec[Porte[p].aAngle]+'a';
			Desc[l++]=0;
		}
		strcpy(Desc2,DirecTxt[Porte[p].bAngle].c_str());
		l=DirecTxt[Porte[p].bAngle].length();
		if (Salle[Porte[p].b].nDirec[Porte[p].bAngle]>1)
		{
			Desc2[l++]='-';
			Desc2[l++]=--Salle[Porte[p].b].nDirec[Porte[p].bAngle]+'a';
			Desc2[l++]=0;
		}
		int id=Door.New(Desc,Desc2);
		Door[id].add_to_rooms(Salle[Porte[p].a].Id,Salle[Porte[p].b].Id);
		
	}
	int id=Door.New("entrée","sortie");
	Door[id].add_to_rooms(Debut,premiere);
	Salle.Detruire();
	Porte.Detruire();
}

/*
int main()
{
	srand((int)time((long*)NULL));
	
	CreerZone(RIEN);
	return 0;	
}*/
