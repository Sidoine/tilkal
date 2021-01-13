#ifndef _AI_INC
#define _AI_INC

#include "object.h"

namespace Tilkal
{

enum ETypeReact {
	RE_ATTACKED=1
};

typedef void (CObject::*ObjectListen)(const char * text);
typedef void (CObject::*ObjectEvent)(int action, int64 data1, int64 data2);
typedef void (CObject::*ObjectSpawn)();
typedef void (CObject::*ObjectReact)(ETypeReact action, int64 data1, int64 data2);

#define NMAXAI 64
class CAInt
{
public:
	ObjectListen Listen;
	ObjectEvent Event;
	ObjectSpawn Spawn;
	ObjectReact React;
};

void InitAI();

#define ENFONCTIONLISTEN(x) (ObjectListen)ADRESSEFONC(x)
#define ENFONCTIONEVENT(x) (ObjectEvent)ADRESSEFONC(x)
#define ENFONCTIONSPAWN(x) (ObjectSpawn)ADRESSEFONC(x)
#define ENFONCTIONREACT(x) (ObjectReact)ADRESSEFONC(x)

#ifdef _DEFINITIONS
CAInt AI[NMAXAI];
#else
extern CAInt AI[NMAXAI];
#endif
}

#endif
