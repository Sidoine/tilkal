#include "tilkal.h"
#include "doors.h"

using namespace Tilkal;

int CDoors::New(const char *name1,const char*name2)
{
	int d=CDoorsBase::New();
	t[d].create(name1,name2);
	return d;
}
