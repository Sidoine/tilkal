#ifndef _FORBNICKS_INC
#define _FORBNICKS_INC

#include "forbnick.h"

namespace Tilkal
{
	typedef TMyArray<CForbNick,128> CForbNicksBase;
	class CForbNicks:public CForbNicksBase
	{
	public:
		bool SaveAll(const char * filename);
		bool LoadAll(const char * filename);	
		int Find(const char *nick);
		int New(const char *nick)
		{
			int ret=CForbNicksBase::New();
			strcpy(t[ret].name,nick);
			return ret;
		}
	};
}

#ifdef _DEFINITIONS
Tilkal::CForbNicks ForbNick;
#else
extern Tilkal::CForbNicks ForbNick;
#endif

#endif
