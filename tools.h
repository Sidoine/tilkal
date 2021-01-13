#ifndef _TOOLS_INC
#define _TOOLS_INC

namespace TilTools
{
	void make_bar_from_values(char *bar, float val, float max_val, int nb_char);
	void load_file_text(const char *fich,char *buf);
	bool strstrnocase(const char * stack, const char * needle);

	#ifdef _WIN32
	#define strncasecmp _strnicmp
	#define strcasecmp _stricmp
	#endif
	
#ifndef _WIN32
	inline void strlwr(char *c)
	{
		while (*c)
			*(c++)=char(tolower(*c));
	}
#endif
	inline int randnum(float max)
	{
		int j;

		j=(int) (max*rand()/(RAND_MAX+1.0));
		return j;
	}

	inline float randfloat(float max)
	{
		return (float)(rand()*max/RAND_MAX);
	}

	inline int dice(int nb)
	{
		return nb/2+randnum(float(nb/2));
	}

	#ifndef max
	inline float max(float a, float b)
	{
		if (a>b)
			return a;
		return b;
	}
	#endif

	#ifndef min
	inline float min(float a, float b)
	{
		if (a<b)
			return a;
		return b;
	}
	#endif

	inline int mean(int a, int b)
	{
		return (int)((a+b)/2);
	}

	class CRepertoire
	{
	public:
	#ifdef _WIN32
		HANDLE hFind;
		WIN32_FIND_DATA fd;
	#else
		char Nom[512];
		DIR * Dir;
		dirent * de;
		struct stat Stat;
		bool StatLue;
		void LireStat()
		{
			if (StatLue)
				return;
			static char Temp[512];
			if (strlen(Nom)+strlen(de->d_name)>510)
				throw "nom de fichier trop long";

			sprintf(Temp,"%s/%s",Nom,de->d_name);
			if (stat(Temp,&Stat))
				throw "le fichier qu'on voulait examiner a disparu";
			StatLue=true;
		}
	#endif
		~CRepertoire();
		CRepertoire();
		CRepertoire(const char * Filtre) : CRepertoire()
		{
			ChercherPremier(Filtre);
		}
		void ChercherPremier(const char * Filtre);
		void ChercherSuivant();
		const char * LireNom();
		bool LireEstRepertoire();
		int64 LireDateModif();
		bool Fini();
		int64 LireTaille();
	};
}

#endif
