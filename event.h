#ifndef _EVENT_INC
#define _EVENT_INC

namespace Tilkal
{

enum ETypeEvent
{
	EV_OBJECT=1,
	EV_AI=2
};

enum ETypeAction
{
	AC_SPEAK=1,
	AC_ATTACK=2
};

class CEvent:public CMyArrayMember
{
public:
	int next;
	int64 time;
	ETypeEvent type;
	int action;
	int64 data1,data2;
	int64 UID;
};

}

#endif
