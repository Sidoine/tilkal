#include "tilkal.h"
#include "doors.h"
#include "objects.h"

using namespace Tilkal;

void CDoor::create(const char *_name1, const char *_name2)
{
    type=T_DOOR_PASSAGE;
    resist=0;
    closed=false;
    strcpy(name1,_name1);
    strcpy(name2,_name2);
    prev_d1=prev_d2=NOTHING;
    next_d1=next_d2=NOTHING;
    r1=r2=NOTHING;
}

void CDoor::Destroy()
{
	remove_from_rooms();
	strcpy(name1,"");
	strcpy(name2,"");
}

void CDoor::add_to_rooms(int _r1, int _r2)
{
	int n1,n2;

	r1=_r1;
	n1=Object[r1].door;
	if (n1!=NOTHING)
	{
		if (Door[n1].r1==r1)
			Door[n1].prev_d1=Id;
		else
			Door[n1].prev_d2=Id;
	}
	next_d1=n1;
	prev_d1=NOTHING;
	Object[r1].door=Id;

	r2=_r2;
	n2=Object[r2].door;
	if (n2!=NOTHING)
	{
		if (Door[n2].r2==r2)
			Door[n2].prev_d2=Id;
		else
			Door[n2].prev_d1=Id;
	}
	next_d2=n2;
	prev_d2=NOTHING;
	Object[r2].door=Id;
}

void CDoor::remove_from_rooms()
{
	int r,nd,pd;
	r=r1;
	if (r==NOTHING)
		return;
	nd=next_d1;
	pd=prev_d1;
	if (nd!=NOTHING)
	{
		if (Door[nd].r1==r)
			Door[nd].prev_d1=pd;
		else
			Door[nd].prev_d2=pd;
	}
	if (pd!=NOTHING)
	{
		if (Door[pd].r1==r)
			Door[pd].next_d1=nd;
		else
			Door[pd].next_d2=nd;
	}
	
	if (Id==Object[r].door)
		Object[r].door=nd;
	prev_d1=next_d1=r1=NOTHING;

	
	r=r2;
	if (r==NOTHING)
		return;
	nd=next_d2;
	pd=prev_d2;
	if (nd!=NOTHING)
	{
		if (Door[nd].r1==r)
			Door[nd].prev_d1=pd;
		else
			Door[nd].prev_d2=pd;
	}
	if (pd!=NOTHING)
	{
		if (Door[pd].r1==r)
			Door[pd].next_d1=nd;
		else
			Door[pd].next_d2=nd;
	}
	
	if (Id==Object[r].door)
		Object[r].door=nd;
	prev_d2=next_d2=r2=NOTHING;

}

