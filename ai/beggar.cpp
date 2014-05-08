#include "../tilkal.h"
#include "beggar.h"
#include "../events.h"
#include "../tools.h"

using namespace Tilkal;
using namespace TilTools;

void AIBeggar::Listen(char *text)
{
	if (strstr(text,"coin"))
	        say("Oh yes, please ! Give me this coin, please.");
}

void AIBeggar::Event(int action, int64 data1, int64 data2)
{
	char beggar_say[][50]={"Please, give me some coin...","Please, noble lord...",
		"Any coin for a poor beggar, my lord ?"};
	switch (action)
	{
		case AC_SPEAK:
			if (opponent==NOTHING) /* Don't beg for money while fighting. */
				say(beggar_say[randnum(3)]);
			else if (randnum(10)==0)
				say("You will regret it !");
			NewEventRelat(EV_AI,randnum(10000)+5000,AC_SPEAK);
			break;
		case AC_ATTACK:
		  if (Object.IsAlive(opponent))
		    Attack(IDfromUID(opponent));
		  break;
	}
}

void AIBeggar::Spawn()
{
	opponent=NOTHING;
	NewEventRelat(EV_AI,randnum(6000)+4000,AC_SPEAK);
}

void AIBeggar::React(ETypeReact action, int64 data1, int64 data2)
{
	switch (action) {
		case	RE_ATTACKED:
		  if (opponent==NOTHING)
		  {
		    say("Hey ! Why do you attack a poor beggar ?");
		    opponent=data1;
		    NewAction(3000,AC_ATTACK,data1);
		  }
		  break;
	
	}

}

void AIBeggar::Register(CAInt & ai)
{
	ai.Listen=ENFONCTIONLISTEN(AIBeggar::Listen);
	ai.Event=ENFONCTIONEVENT(AIBeggar::Event);
	ai.Spawn=ENFONCTIONSPAWN(AIBeggar::Spawn);
	ai.React=ENFONCTIONREACT(AIBeggar::React);
}
