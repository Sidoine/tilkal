#include "tilkal.h"
#include "skillprots.h"
#include "server.h"

using namespace Tilkal;

void CSkillProts::Init()
{
	printf("[Tilkal] Initiating skills.\r\n");
	Server::write_log("[Tilkal] Initiating skills.\r\n");

	New();
	/* Move */
	SkillProt[SKILL_MOVE].Init();
	strcpy(SkillProt[SKILL_MOVE].name,"move");
	SkillProt[SKILL_MOVE].AllocRaces(1);
	SkillProt[SKILL_MOVE].AddRace(ALL_RACES);
	SkillProt[SKILL_MOVE].AllocCaracs(1);
	SkillProt[SKILL_MOVE].AddCarac(CARAC_CONSTIT,1);
	SkillProt[SKILL_MOVE].progress=5; // One chance over 'progress' to make a percent test
	SkillProt[SKILL_MOVE].desc="Move: ability to move without getting tired.\r\n";

	New();
	/* Hand fight */
	SkillProt[SKILL_HAND_FIGHT].Init();
	strcpy(SkillProt[SKILL_HAND_FIGHT].name,"hand fight");
	SkillProt[SKILL_HAND_FIGHT].AllocRaces(1);
	SkillProt[SKILL_HAND_FIGHT].AddRace(ALL_RACES);
	SkillProt[SKILL_HAND_FIGHT].AllocCaracs(1);
	SkillProt[SKILL_HAND_FIGHT].AddCarac(CARAC_DEXT,1);
	SkillProt[SKILL_HAND_FIGHT].progress=5;
	SkillProt[SKILL_HAND_FIGHT].desc="Hand fight: fight with your bare hands.\r\n";

	New();
	/* Sword fight */
	SkillProt[SKILL_SWORD_FIGHT].Init();
	strcpy(SkillProt[SKILL_SWORD_FIGHT].name,"sword fight");
	SkillProt[SKILL_SWORD_FIGHT].AllocRaces(1);
	SkillProt[SKILL_SWORD_FIGHT].AddRace(ALL_RACES);
	SkillProt[SKILL_SWORD_FIGHT].AllocCaracs(1);
	SkillProt[SKILL_SWORD_FIGHT].AddCarac(CARAC_DEXT,1);
	SkillProt[SKILL_SWORD_FIGHT].progress=5;
	SkillProt[SKILL_SWORD_FIGHT].desc="Sword fight: fight with a sword.\r\n";

	New();
	/* Axe fight */
	SkillProt[SKILL_AXE_FIGHT].Init();
	strcpy(SkillProt[SKILL_AXE_FIGHT].name,"axe fight");
	SkillProt[SKILL_AXE_FIGHT].AllocRaces(1);
	SkillProt[SKILL_AXE_FIGHT].AddRace(ALL_RACES);
	SkillProt[SKILL_AXE_FIGHT].AllocCaracs(1);
	SkillProt[SKILL_AXE_FIGHT].AddCarac(CARAC_DEXT,1);
	SkillProt[SKILL_AXE_FIGHT].progress=5;
	SkillProt[SKILL_AXE_FIGHT].desc="Axe fight: fight with an axe.\r\n";

	New();
	/* Block */
	SkillProt[SKILL_BLOCK].Init();
	strcpy(SkillProt[SKILL_BLOCK].name,"block");
	SkillProt[SKILL_BLOCK].AllocRaces(1);
	SkillProt[SKILL_BLOCK].AddRace(ALL_RACES);
	SkillProt[SKILL_BLOCK].AllocCaracs(1);
	SkillProt[SKILL_BLOCK].AddCarac(CARAC_DEXT,1);
	SkillProt[SKILL_BLOCK].progress=5;
	SkillProt[SKILL_BLOCK].desc="Block: block all kinds of attacks.\r\n";
}
