#include "object.h"

#ifndef _OBJECTS_INC
#define _OBJECTS_INC

namespace Tilkal
{
	typedef TMyArray<CObject,128> CObjectsBase;

	class CObjects:public CObjectsBase
	{
	public:
		static void Remove(int s)
		{
			t[s].Destroy();
			CObjectsBase::Remove(s);
		}
		static int New();
		static int New(const char *name);
		static int New(const char *name, const char *desc);
		static int New(const char *name, const char *desc, int inside);
		static bool SaveAll(const char * filename);
		static bool LoadAll(const char * filename);			
	};
}

#ifdef _DEFINITIONS
DEFINE_MYARRAY(Tilkal::CObject)
Tilkal::CObjects Object;
#else
extern Tilkal::CObjects Object;
#endif

#endif
