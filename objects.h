#include "object.h"

#ifndef _OBJECTS_INC
#define _OBJECTS_INC

namespace Tilkal
{
	typedef TMyArray<CObject,128> CObjectsBase;

	class CObjects:public CObjectsBase
	{
	public:
		void Remove(int s)
		{
			t[s].Destroy();
			CObjectsBase::Remove(s);
		}
		int New();
		int New(const char *name);
		int New(const char *name, const char *desc);
		int New(const char *name, const char *desc, int inside);
		bool SaveAll(const char * filename);
		bool LoadAll(const char * filename);			
	};
}

#ifdef _DEFINITIONS
Tilkal::CObjects Object;
#else
extern Tilkal::CObjects Object;
#endif

#endif
