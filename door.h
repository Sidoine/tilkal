#ifndef _DOOR_INC
#define _DOOR_INC

namespace Tilkal
{

enum EDoorType
{
	T_DOOR_PASSAGE=0,
	T_DOOR_HOLE=1,
	T_DOOR_WOODEN_DOOR=2,
	T_DOOR_IRON_DOOR=3,
	T_DOOR_IRON_GRID=4,
	T_DOOR_GATE=5,
	MAX_DOOR_TYPES=6
};

class CDoor:public CMyArrayMember
{
public:
	char name1[TMAXDOORNAME+1];
	char name2[TMAXDOORNAME+1];
	int prev_d1,next_d1,r1;
	int prev_d2,next_d2,r2;
	EDoorType type;
	bool closed;
	unsigned short resist;
	
	void create(const char* name1, const char * name2);
	int next_door_of_room(int r)
	{
		if (r1==r)
			return next_d1;
		else
			return next_d2;
	}
	void add_to_rooms(int r1,int r2);
	int OtherSide(int r)
	{
		if (r1==r)
			return r2;
		else
			return r1;
	}
	char * name(int r)
	{
		if (r==r1)
			return name1;
		else
			return name2;
	}
	void Destroy();
	void remove_from_rooms();
};

}

#endif
