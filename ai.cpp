#include "tilkal.h"
#include "ai.h"
#include "ai/standard.h"
#include "ai/beggar.h"
#include "ai/shopkeeper.h"

using namespace std;
using namespace Tilkal;

void Tilkal::InitAI()
{
	AIStandard::Register(AI[0]);
	AIBeggar::Register(AI[1]);
	AIShopkeep::Register(AI[2]);
	clog << "[Tilkal] AI initialized.\n";
}
