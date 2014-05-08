#ifndef _OBJCATEGS_INC
#define _OBJCATEGS_INC

#include "objcateg.h"


namespace Tilkal
{
	typedef TMyArray<CObjCateg,128> CObjCategsBase;


	class CObjCategs:public CObjCategsBase
	{
	public:
		void init();
		static int New()
		{
			int n=CObjCategsBase::New();
			t[n].numberable=false;
			t[n].max_containweight=0;
			t[n].nbodyparts=0;
			t[n].ncoveredparts=0;
			return n;
		}
	};
}

#ifdef _DEFINITIONS
DEFINE_MYARRAY(Tilkal::CObjCateg)
Tilkal::CObjCategs ObjCateg;
#else
extern Tilkal::CObjCategs ObjCateg;
#endif

#endif
