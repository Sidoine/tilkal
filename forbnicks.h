#ifndef _FORBNICKS_INC
#define _FORBNICKS_INC

#include "forbnick.h"

namespace Tilkal
{
	typedef TMyArray<CForbNick,128> CForbNicksBase;
	class CForbNicks:public CForbNicksBase
	{
	public:
		static bool SaveAll(const char * filename);
		static bool LoadAll(const char * filename);	
		static int Find(const char *nick);
		static int New(const char *nick)
		{
			int ret=CForbNicksBase::New();
			strcpy(t[ret].name,nick);
			return ret;
		}
	};
}

#ifdef _DEFINITIONS
DEFINE_MYARRAY(Tilkal::CForbNick)
Tilkal::CForbNicks ForbNick;
#else
extern Tilkal::CForbNicks ForbNick;
#endif

#endif
