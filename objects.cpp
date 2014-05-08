#include "tilkal.h"
#include "objects.h"
#include "server.h"

using namespace Tilkal;

bool CObjects::SaveAll(const char * filename)
{
	FILE *fichier=fopen(filename,"wb");
	if (fichier == NULL)
		return false;
	fwrite(&Length,sizeof(size_t),1,fichier);
	fwrite(&Free,sizeof(size_t),1,fichier);
	for (int i=0; i<Length; i++)
	{
		fwrite(&t[i],sizeof(CObject),1,fichier);
		fwrite(t[i].desc,sizeof(char),t[i].desc_size,fichier);
	}
	fclose(fichier);	
	return true;
}

bool CObjects::LoadAll(const char * filename)
{
	FILE *fichier=fopen(filename,"rb");
	if (fichier == NULL)
		return false;
	fread(&Length,sizeof(size_t),1,fichier);
	fread(&Free,sizeof(size_t),1,fichier);
	Allocated=Length;
	t=(CObject*)malloc(Allocated*sizeof(CObject)); 
	for (int i=0; i<Length; i++)
	{
		fread(&t[i],sizeof(CObject),1,fichier);
		t[i].desc=(char*)malloc(t[i].desc_size);
		fread(&t[i].desc,sizeof(char),t[i].desc_size,fichier);
	}
	fclose(fichier);	
	printf("[Tilkal] Objects loaded.\r\n");
	Server::write_log("[Tilkal] Objects loaded.\r\n");
	return true;
}

int CObjects::New()
{
	int o=CObjectsBase::New();
	t[o].Init();
	t[o].create_standard(-1);
	return o;
}

int CObjects::New(const char *name)
{
	int o=New();
	strcpy(t[o].name,name);
	return o;
}

int CObjects::New(const char *name, const char *desc)
{
	int o=New(name);
	t[o].desc=strdup(desc);
	return o;
}

int CObjects::New(const char *name, const char *desc, int inside)
{
	int o=New(name,desc);
	t[o].inside=inside;
	return o;
}

