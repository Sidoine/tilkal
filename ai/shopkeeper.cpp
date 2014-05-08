#include "../tilkal.h"
#include "shopkeeper.h"
#include "../events.h"
#include "../tools.h"

using namespace Tilkal;
using namespace TilTools;

void AIShopkeep::Listen(char *text)
{
//	if (strstr(text,"coin"))
//	        say("Oh yes, please ! Give me this coin, please.");
}

void AIShopkeep::Event(int action, int64 data1, int64 data2)
{
	char shopkeep_say[][50]={"I buy your stuff !","Come and take a look at my wares !",
		"The best adventurers stuff here !"};
	switch (action)
	{
		case AC_SPEAK:
			if (opponent==NOTHING)
				say(shopkeep_say[randnum(3)]);
			NewEventRelat(EV_AI,randnum(20000)+10000,AC_SPEAK);
			break;
		case AC_ATTACK:
		  if (Object.IsAlive(opponent))
		    Attack(IDfromUID(opponent));
		  break;
	}
}

void AIShopkeep::Spawn()
{
	opponent=NOTHING;
	NewEventRelat(EV_AI,randnum(3000)+4000,AC_SPEAK);
}

void AIShopkeep::React(ETypeReact action, int64 data1, int64 data2)
{
	switch (action) {
		case	RE_ATTACKED:
		  if (opponent==NOTHING)
		  {
		    say("Now you are gonna die.");
		    opponent=data1;
		    NewAction(3000,AC_ATTACK,data1);
		  }
		  break;
	
	}

}

void AIShopkeep::Register(CAInt & ai)
{
	ai.Listen=ENFONCTIONLISTEN(AIShopkeep::Listen);
	ai.Event=ENFONCTIONEVENT(AIShopkeep::Event);
	ai.Spawn=ENFONCTIONSPAWN(AIShopkeep::Spawn);
	ai.React=ENFONCTIONREACT(AIShopkeep::React);
}
