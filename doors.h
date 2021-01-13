#ifndef _DOORS_INC
#define _DOORS_INC

#include "door.h"

namespace Tilkal
{
	typedef TMyArray<CDoor,256> CDoorsBase;

	class CDoors:public CDoorsBase
	{
	public:
		int New(const char *name1,const char*name2);
	};
}

#ifdef _DEFINITIONS
Tilkal::CDoors Door;
#else
extern Tilkal::CDoors Door;
#endif

#endif
