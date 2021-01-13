#include "tilkal.h"
#include "forbnicks.h"

using namespace Tilkal;

bool CForbNicks::SaveAll(const char * filename)
{
  int i;
  FILE *fichier;

  fichier=fopen(filename,"w");

  if (fichier == NULL)
  	return false;
  
  for (i=0;i<Length;i++)
    if (t[i].Id==i)
      fprintf(fichier,"%s\n",t[i].name);
  fclose(fichier);
  return true;
}

bool CForbNicks::LoadAll(const char * filename)
{
 char tmp[50];
  FILE *fichier;

  if ((fichier=fopen(filename,"r"))!=NULL)
  {
    while (fgets(tmp,BUFSIZ,fichier))
    {
      tmp[strlen(tmp)-1]='\0';
		New(tmp);	
    }
    fclose(fichier);
    printf("[Tilkal] Loaded forbiden nicks.\r\n");
  }
  else 
	  return false;
  return true;
}

int CForbNicks::Find(const char *nick)
{
	for (int f=0; f<Length; f++)
		if (t[f].Id==f && strcmp(t[f].name,nick)==0)
			return (int)f;
	return NOTHING;
}