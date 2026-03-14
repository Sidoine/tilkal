module;

#include "tilkal.h"
#include "myarray.h"

export module tilkal.bans;

export namespace Tilkal
{
	class CBan:public CMyArrayMember
	{
	public:
		char IP[20];
		void Destroy()
		{
		}
	};

	using CBansBase = TMyArray<CBan,128>;

	class CBans:public CBansBase
	{
	public:
		bool SaveAll(const char * filename);
		bool LoadAll(const char * filename);
		int Find(const char *ip);
		int New(const char *ip)
		{
			const int ret=CBansBase::New();
			strcpy(t[ret].IP,ip);
			return ret;
		}
	};

}

export inline Tilkal::CBans Ban;

namespace Tilkal
{
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
}