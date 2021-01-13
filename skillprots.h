#ifndef _SKILLPROTS_INC
#define _SKILLPROTS_INC

#include "skillprot.h"

namespace Tilkal
{
	typedef TMyArray<CSkillProt,128> CSkillProtsBase;

	class CSkillProts:public CSkillProtsBase
	{
	public:
		void Init();
	};
}

#ifdef _DEFINITIONS
Tilkal::CSkillProts SkillProt;
#else
extern Tilkal::CSkillProts SkillProt;
#endif

#endif
