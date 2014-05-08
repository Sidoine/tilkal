#ifndef _AISHOPKEEP_INC
#define _AISHOPKEEP_INC

#include "../object.h"
#include "../ai.h"

namespace Tilkal
{

class AIShopkeep:public CObject
{
	void Listen(char *text);
	void Event(int action, int64 data1, int64 data2);
	void Spawn();
	void React(ETypeReact action, int64 data1, int64 data2);
public:
	static void Register(CAInt &ai);
};

}

#endif
