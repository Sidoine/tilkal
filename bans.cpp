#include "tilkal.h"
#include "bans.h"

using namespace Tilkal;

bool CBans::SaveAll(const char * filename)
{
  FILE *fichier;

  fichier=fopen(filename,"w");

  if (fichier == NULL)
  	return false;
  
  for (int i=0;i<Length;i++)
    if (t[i].Id==i)
      fprintf(fichier,"%s\n",t[i].IP);
  fclose(fichier);
  return true;
}

bool CBans::LoadAll(const char * filename)
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
		printf("[Tilkal] Loaded baned IPs.\r\n");
	}
	else 
		return false;
	return true;
}

int CBans::Find(const char *ip)
{
	for (int i=0; i<Length; i++)
		if (Ban.IsActive(i) && strcmp(Ban[i].IP,ip)==0)
			return (int)i;
	return NOTHING;
}