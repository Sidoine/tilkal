#include "tilkal.h"
#include "tools.h"

using namespace TilTools;

void TilTools::make_bar_from_values(char *bar, float val, float max_val, int nb_char)
{
  int i;
  float n;

  for (i=0;i<nb_char;i++)
  {
    n=(max_val/nb_char)*i;
    if (n<=val)
      bar[i]='#';
    else
      bar[i]=' ';
  }
  bar[i]='\0';
}

/*void TilTools::create_random_name(char *nm)
{
  int i,j,max,nmt[5];
  int tst;
  char *syllab[]={
  "a","e","o","i","u",
  "ra","re","ro","ri","ru",
  "ka","ke","ko","ki","ku",
  "ma","me","mo","mi","mu",
  "la","le","lo","li","lu"};

  max=randnum(3)+2;
  nmt[0]=randnum(5);
  for (i=1;i<max;i++)
  {
    do {
      tst=0;
      nmt[i]=randnum(5);
      for (j=0;j<i;j++)
      {
        if (nmt[j]==nmt[i])
          tst=1;
      }
    } while (tst==1);
  }  
  nm[0]='\0';
  for (i=0;i<max;i++)
    strcat(nm,syllab[5*nmt[i]+randnum(5)]);
  nm[0]='A'+(nm[0]-'a'); 
}*/

void TilTools::load_file_text(char *fich,char *buf)
{
  int len;
  char buffer[BUFSIZ];
  FILE *fichier;

  strcpy(buf,"\0");
  fichier=fopen(fich,"r");
  if (fichier == NULL)
  {
    printf("[Tilkal] Unable to open file: %s\r\n",fich);
    exit(-1);
  }
  while (fgets(buffer,BUFSIZ,fichier))
  {
    len=(int)strlen(buffer);
    buffer[len-1]='\r';
    buffer[len]='\n';
    buffer[len+1]='\0';
    strcat(buf,buffer);
  }
  fclose(fichier);
}

bool TilTools::strstrnocase(const char * stack, const char * needle)
{
	while (*stack)
	{
		const char * q=stack;
		const char * p=needle;
		while (*p && toupper(*q)==toupper(*p))
		{
			p++;
			q++;
		}
		if (!*p)
			return true;			
		stack++;
	}
	return false;
}


CRepertoire::~CRepertoire()
{
#ifdef _WIN32
	if (hFind)
		FindClose(hFind);
#else
	if (Dir)
		closedir(Dir);
#endif
	//clog << "Répertoire fermé.\n";
}

CRepertoire::CRepertoire()
{
#ifdef _WIN32
	hFind=NULL;
	memset(&fd,0,sizeof(WIN32_FIND_DATA));
#else
	Dir=NULL;
#endif
	//clog << "On crée un répertoire.\n";
}

void CRepertoire::ChercherPremier(const char * Filtre)
{
#ifdef _WIN32
	char Temp[MAX_PATH];
	sprintf(Temp,"%s\\*.*",Filtre);
	hFind=FindFirstFile(Temp,&fd);
	if (hFind)
	{
		while (strcmp(fd.cFileName,".")==0 ||
			strcmp(fd.cFileName,"..")==0)
		{
			if (!FindNextFile(hFind,&fd))
			{
				FindClose(hFind);
				hFind=NULL;
				return;
			}
		}
	}
#else
	strncpy(Nom,Filtre,511);
	Dir=opendir(Filtre);
	if (Dir)
	{
		de=readdir(Dir);
		while (strcmp(de->d_name,".")==0 ||
			strcmp(de->d_name,"..")==0)
		{
			de=readdir(Dir);
			if (!de)
			{
				closedir(Dir);
				Dir=NULL;
				return;
			}
		}
	}
	StatLue=false;
#endif
}

void CRepertoire::ChercherSuivant()
{
#ifdef _WIN32
	do
	{
		if (!FindNextFile(hFind,&fd))
		{
			FindClose(hFind);
			hFind=NULL;
			return;
		}
	}
	while (strcmp(fd.cFileName,".")==0 ||
		strcmp(fd.cFileName,"..")==0);
#else
	do
	{
		if (!(de=readdir(Dir)))
		{
			closedir(Dir);
			Dir=NULL;
			return;
		}
	}
	while (strcmp(de->d_name,".")==0 ||
		strcmp(de->d_name,"..")==0);
	StatLue=false;
#endif
}


const char * CRepertoire::LireNom()
{
#ifdef _WIN32
	return fd.cFileName;
#else
	return de->d_name;
#endif
}

bool CRepertoire::LireEstRepertoire()
{
#ifdef _WIN32
	return (int(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))!=0;
#else
	LireStat();
	return (S_ISDIR(Stat.st_mode))!=0;
#endif
}

int64 CRepertoire::LireDateModif()
{
#ifdef _WIN32
	return ((int64) (*(int64*)&fd.ftLastWriteTime));
#else
	LireStat();
	return((int64) Stat.st_mtime);
#endif
}

int64 CRepertoire::LireTaille()
{
#ifdef _WIN32
	return int64(fd.nFileSizeLow)+(int64(fd.nFileSizeHigh)<<32);
#else
	LireStat();
	return Stat.st_size;
#endif
}

bool CRepertoire::Fini()
{
#ifdef _WIN32
	return (hFind==NULL);
#else
	return (Dir==NULL);
#endif
}