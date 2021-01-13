#ifndef _PLAYER_INC
#define _PLAYER_INC

//#include "objects.h"

namespace Tilkal
{

enum EPlLevel
{
	GOD_GHOST=0,
	GOD_SPIRIT=1,
	GOD_MESSIAH=2,
	GOD_ANGEL=3,
	GOD_ARCHANGEL=4,
	GOD_LESSERGOD=5,
	GOD_GOD	=6,
	GOD_HIGHGOD=7,
	GOD_HIGHEST=8 // Warning: the god level will be displayed with one digit
};

class CObject;

enum EReportLevel
{
	REPORT_NOTHING=0,
	REPORT_CONNECT=5,
	REPORT_DISCONNECT=5,
	REPORT_LOGIN=5,
	REPORT_PROMOTE=5,
	REPORT_DEMOTE=5,
	REPORT_KILLPLAYER=5,
	REPORT_REHASH=3,
	REPORT_BAN=5,
	REPORT_UNBAN=5,
	REPORT_BAN_CONNECT=4,
	REPORT_FORBID=5,
	REPORT_UNFORBID=5,
	REPORT_DESTROY=3,
	REPORT_CONNECTION_LOST=6
};

class CPlayer:public CMyArrayMember
{
public:
	char pass[TMAXPASS+1];
	int object;
	char god,is_registered;
	int lvl;
	EReportLevel lvl_report;
	int client;
	char ip[20];

	
	void create_standard();
	void Destroy();
	void Compute_Level();
	inline CObject & obj();
};

}

#include "objects.h"

namespace Tilkal
{

inline CObject & CPlayer::obj()
{
	return Object[object];
}

}
#endif
