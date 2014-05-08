#ifndef _BANS_INC
#define _BANS_INC

#include "ban.h"

namespace Tilkal
{
	typedef TMyArray<CBan,128> CBansBase;

	class CBans:public CBansBase
	{
	public:
		static bool SaveAll(const char * filename);
		static bool LoadAll(const char * filename);		
		static int Find(const char *ip);
		static int New(const char *ip)
		{
			int ret=CBansBase::New();
			strcpy(t[ret].IP,ip);
			return ret;
		}
	};
}

#ifdef _DEFINITIONS
DEFINE_MYARRAY(Tilkal::CBan)
Tilkal::CBans Ban;
#else
extern Tilkal::CBans Ban;
#endif

#endif
