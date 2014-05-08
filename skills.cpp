#include "tilkal.h"
#include "skills.h"

using namespace Tilkal;

int CSkills::New(int type, int val)
{
	int n=CSkillsBase::New();
	t[n].type=type;
	t[n].val=val;
	return n;
}

