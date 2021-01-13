#include "tilkal.h"
#include "players.h"
#include "tools.h"

using namespace Tilkal;

bool CPlayers::LoadAll(const char * filename)
{
	bool ret=CPlayersBase::LoadAll(filename);
	if (!ret)
		return false;
	for (int i=0; i<Length; i++)
		t[i].client=NOTHING;
	return true;
}

void CPlayers::remove_all()
{
  int i;

  for (i=0;i<Length;i++)
    if (t[i].Id==i && t[i].object!=NOTHING)
		  t[i].obj().remove_from_obj();
}

int CPlayers::Find(const char * name)
{
	for (int i=0;i<Length;i++)
	{
		if (t[i].Id==i && t[i].is_registered)
		{
			int o=t[i].object;
			if (Object[o].type==ObjType::Human
			&& strcasecmp(Object[o].name,name)==0)
				return (int) i;
		}
	}
	return NOTHING;
}

int CPlayers::New()
{
	int p=CPlayersBase::New();
	int o;
	t[p].object=o=Object.New();
	Object[o].player=p;
//	clog << "object="<<o<<" object.player="<<Object[o].player<<endl;
	return p;
}
	
