#ifndef _SKILLPROT_INC
#define _SKILLPROT_INC

#define	ALL_RACES		-1

namespace Tilkal
{
struct SSkillProtCarac
{
	ECarac carac;
	int val;
};

class CSkillProt:public CMyArrayMember
{
public:
	char name[TMAXSKILLNAME+1];
	int nb_races,nb_caracs;
	int cost_magic,cost_health;
	int progress;
	int *race;
	SSkillProtCarac *carac;
	const char *desc;

	void AddCarac(ECarac _carac, int _val)
	{
		carac[nb_caracs].carac=_carac;
		carac[nb_caracs++].val=_val;
	}
	void AllocCaracs(int nb)
	{
		carac=new SSkillProtCarac[nb];
	}
	void AllocRaces(int nb)
	{
		race=new int[nb];
	}
	void AddRace(int i)
	{
		race[nb_races++]=i;
	}
	void Remove()
	{
		if (carac)
			delete carac;
		if (race)
			delete race;
	}
	void Init()
	{
		nb_races=0;
		nb_caracs=0;
	}
};
}
#endif
