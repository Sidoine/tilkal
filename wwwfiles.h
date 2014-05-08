#ifndef _WWWFILES_INC
#define _WWWFILES_INC

namespace Tilkal
{
	namespace ContentType
	{
		enum EContentType
		{
			text_plain,
			text_html,
			text_css,
			image_png,
			image_jpeg
		};
	}

	class CWWWFile:public CBaseHacheValue
	{
	public:
		ContentType::EContentType type;
		char * data;
		size_t length;
	};

	typedef THachage<CWWWFile> CBaseWWWFiles;

	class CWWWFiles:public CBaseWWWFiles
	{
	public:
		void Init();
	};
}

#ifdef _DEFINITIONS
Tilkal::CWWWFiles WWWFile;
#else
extern Tilkal::CWWWFiles WWWFile;
#endif

#endif