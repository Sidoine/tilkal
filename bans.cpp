#include "tilkal.h"
#include "bans.h"

using namespace Tilkal;

CBans Ban;

bool CBans::SaveAll(const char * filename)
{
  FILE *fichier=fopen(filename,"w");

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
	FILE *fichier=fopen(filename,"r");

	if (fichier == NULL)
		return false;

	while (fgets(tmp,sizeof(tmp),fichier))
	{
		tmp[strcspn(tmp,"\r\n")]='\0';
		New(tmp);	
	}
	fclose(fichier);
	printf("[Tilkal] Loaded banned IPs.\r\n");
	return true;
}

int CBans::Find(const char *ip)
{
	for (int i=0; i<Length; i++)
		if (IsActive(i) && strcmp(t[i].IP,ip)==0)
			return (int)i;
	return NOTHING;
}