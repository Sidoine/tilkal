#ifndef _SKILLS_INC
#define _SKILLS_INC


#include "skill.h"
namespace Tilkal
{
	typedef TMyArray<CSkill,1024> CSkillsBase;

	class CSkills:public CSkillsBase
	{
	public:
			int New(int type, int val);
	};
}

#ifdef _DEFINITIONS
Tilkal::CSkills Skill;
#else
extern Tilkal::CSkills Skill;
#endif

#endif
