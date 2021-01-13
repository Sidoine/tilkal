#ifndef _BANS_INC
#define _BANS_INC

#include "ban.h"

namespace Tilkal
{
	typedef TMyArray<CBan,128> CBansBase;

	class CBans:public CBansBase
	{
	public:
		bool SaveAll(const char * filename);
		bool LoadAll(const char * filename);		
		int Find(const char *ip);
		int New(const char *ip)
		{
			int ret=CBansBase::New();
			strcpy(t[ret].IP,ip);
			return ret;
		}
	};
}

#ifdef _DEFINITIONS
Tilkal::CBans Ban;
#else
extern Tilkal::CBans Ban;
#endif

#endif
