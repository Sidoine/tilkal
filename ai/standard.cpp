#include "../tilkal.h"
#include "standard.h"
#include "../events.h"

using namespace Tilkal;

void AIStandard::Listen(char *text)
{
}

void AIStandard::Event(int action, int64 data1, int64 data2)
{
}

void AIStandard::Spawn()
{
}

void AIStandard::React(ETypeReact action, int64 data1, int64 data2)
{


}

void AIStandard::Register(CAInt & ai)
{
	ai.Listen=ENFONCTIONLISTEN(AIStandard::Listen);
	ai.Event=ENFONCTIONEVENT(AIStandard::Event);
	ai.Spawn=ENFONCTIONSPAWN(AIStandard::Spawn);
	ai.React=ENFONCTIONREACT(AIStandard::React);
}
