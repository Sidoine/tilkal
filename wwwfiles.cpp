#include "tilkal.h"
#include "wwwfiles.h"
#include "tools.h"

using namespace Tilkal;
using namespace TilTools;

void CWWWFiles::Init()
{
	CRepertoire Directory("www");
	char FileName[TMAXCLEFHACHAGE];
	char LongFileName[MAX_PATH];

	while (!Directory.Fini())
	{
		if (!Directory.LireEstRepertoire())
		{
			strncpy(FileName,Directory.LireNom(),TMAXCLEFHACHAGE-1);
			char * pos=FileName;
			char * ext=NULL;
			while (*pos)
			{
				if (*(pos++)=='.')
					ext=pos;
			}
			
			if (!Existe(FileName))
			{
				sprintf(LongFileName,"www/%s",FileName);
				FILE * F=fopen(LongFileName,"rb");
				if (F)
				{
					CWWWFile * f=Ajouter(FileName);	
					if (strcmp(ext,"css")==0)
						f->type=ContentType::text_css;
					else if (strcmp(ext,"jpg")==0)
						f->type=ContentType::image_jpeg;
					else if (strcmp(ext,"png")==0)
						f->type=ContentType::image_png;
					else if (strcmp(ext,"txt")==0)
						f->type=ContentType::text_plain;
					else if (strcmp(ext,"html")==0)
						f->type=ContentType::text_html;
					else
						f->type=ContentType::text_plain;
					f->length=(size_t)Directory.LireTaille();
					f->data=new char[f->length+1];
					fread(f->data,f->length,1,F);
					f->data[f->length]=0;
					fclose(F);
				}
			}
		}
		Directory.ChercherSuivant();
	}
}