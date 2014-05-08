#include "tilkal.h"
#include "skills.h"
#include "objects.h"
#include "skillprots.h"
#include "tools.h"
#include "clients.h"

using namespace Tilkal;
using namespace TilTools;

float CSkill::Compute()
{
  int i,o,nb=0,sk;
  float val=0;

  sk=type;
  o=owner;
  for (i=0;i<SkillProt[sk].nb_caracs;i++)
  {
	val+=Object[o].carac[SkillProt[sk].carac[i].carac]*
			SkillProt[sk].carac[i].val;
	nb+=SkillProt[sk].carac[i].val;
  }
  val=val/nb;
  val=val/2+randfloat(val/2);
  return val;
}

void CSkill::Remove()
{
	int p;
	if (owner==NOTHING)
		return;
	p=Object[owner].skill;
	assert(p!=NOTHING);
  
	if (next!=NOTHING)
		Skill[next].prev=prev;
	if (prev!=NOTHING)
		Skill[prev].next=next;
	if (p==Id)
		Object[owner].skill=next;
	prev=next=NOTHING;
	owner=NOTHING;
}

void CSkill::Test_Progress()
{
	char buf[300];

	if ((Object[owner].type==ObjType::Bot) && (BOT_PROGRESS==0))
		return;
	int pro=SkillProt[type].progress;
	if ((randnum((float)pro)==1) && (randnum(100)>val))
	{
		val++;
		int cl;
		if ((cl=Object[owner].Has_A_Client())!=NOTHING)
		{
			sprintf(buf,"\r\n*** Your skill [%s] is now %d%% ***\r\n",SkillProt[type].name,val);
			Client[cl].WriteEvent(buf);
		}
		if ((Object[owner].type==ObjType::Human) && (Object[owner].player!=NOTHING))
			Player[Object[owner].player].Compute_Level();
	}
}


