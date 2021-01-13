#include "tilkal.h"
#include "player.h"
#include "clients.h"
#include "skills.h"

using namespace Tilkal;

void CPlayer::create_standard()
{
	obj().create_standard(0); //human
	obj().type=ObjType::Human;
	obj().weight=80;
	obj().max_weight=200;
	obj().set_name(DEFAULT_PLAYER_NAME);
	obj().give_standard_skills();
	client=NOTHING;
	strcpy(pass,"");
	god=false;
	lvl=1;
	lvl_report=REPORT_NOTHING;
	is_registered=false;
	Compute_Level();
}

void CPlayer::Destroy()
{
	if (client!=NOTHING)
		Client.Remove(client);
	if (object!=NOTHING)
		Object.Remove(object);
}

void CPlayer::Compute_Level()
{
	int sk,l,val=0;
	char buf[300];

	if (god) // Never ever recalculate the level of a god from his skills !!!
		return;
	for (sk=Object[object].skill;sk!=NOTHING;sk=Skill[sk].next)
		val+=Skill[sk].val;
	l=(int)(val*MORT_HIGHEST/PERCENT_TO_HIGHEST);
	if ((l>lvl) && (client!=NOTHING))
	{
		sprintf(buf,"\r\n*** Your general level increases ! ***\r\n");
		Client[client].WriteEvent(buf);
	}
	if ((l<lvl) && (client!=NOTHING))
	{
		sprintf(buf,"\r\n*** Your general level decreases... ***\r\n");
		Client[client].WriteEvent(buf);
	}
	lvl=l;
}

