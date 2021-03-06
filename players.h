#include "player.h"

#ifndef _PLAYERS_INC
#define _PLAYERS_INC

namespace Tilkal
{
	typedef TMyArray<CPlayer,128> CPlayersBase;

	class CPlayers:public CPlayersBase
	{
	public:
		int New();
		bool LoadAll(const char * filename);
		void remove_all();
		int Find(const char *name);
	};
}

#ifdef _DEFINITIONS
Tilkal::CPlayers Player;
#else
extern Tilkal::CPlayers Player;
#endif

#endif
