#ifndef _FORM_INC
#define _FORM_INC
#include "Tableau.h"

class CFormVar
{
public:
	char * Texte;
	size_t Longueur;
	char * ContentType;
	char * NomFichier;
	char Nom[TMAXCLEFHACHAGE];
};

class Form
{
public:
	static THachage<CFormVar> Champ;

private:
	static bool Param_MultiPart(char * NomVariable,
						   char * contenu, size_t longueur, 
						   char * NomFichier,
						   char * contentType);
	static bool Variable_Creer(char * Nom, char * Contenu, size_t Longueur, char * NomFichier,
								 char * ContentType);
	static bool Param_URL(char * Input);
public:
	static void Init();
};

#endif