#include "tilkal.h"
#include "events.h"
#include "server.h"

using namespace Tilkal;

int CEvents::New(ETypeEvent type, int64 UID, int64 tps, int action, int64 data1, int64 data2)
{
	int e=CEventsBase::New();
	int f=first;
	int p=NOTHING;
	
	while (f!=NOTHING && t[f].time<tps)
	{
		p=f;
		f=t[f].next;
	}
	if (p!=NOTHING)
		t[p].next=e;
	t[e].next=f;
	t[e].type=type;
	t[e].UID=UID;
	t[e].time=tps;
	t[e].action=action;
	t[e].data1=data1;
	t[e].data2=data2;
	if (p==NOTHING)
		first=e;
	return e;
}
