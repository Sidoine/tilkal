#include "tilkal.h"
#include "objcategs.h"

using namespace Tilkal;

void CObjCategs::init()
{
	New();
	/* Name, type, link to,
	   resistance (1=human skin),
	   health (1=human chest),
	   size (m^2),
	   height (m) */
	strcpy(ObjCateg[0].name,"human");
	ObjCateg[0].add_bodypart("chest",BODY_CHEST,NOTHING, 1,1,0.5f,1.4f);//0
	ObjCateg[0].last_bodypart().vital=true;
	ObjCateg[0].add_bodypart("neck",BODY_NECK,0, 1,0.3f,0.015f,1.6f);//1
	ObjCateg[0].add_bodypart("right arm",BODY_ARM,0, 1,0.8f,0.1f,1.4f);//2
	ObjCateg[0].add_bodypart("right hand",BODY_HAND,2, 1.2f,0.3f,0.01f,1.4f);//3
	/* Type, dam1,dam2,dam3 */
	ObjCateg[0].add_bodypart_damage(DAM_MACE,0.2f,0.03f,0);
	ObjCateg[0].last_bodypart().canhold=true;	
	ObjCateg[0].add_bodypart("left arm",BODY_ARM,0, 1,0.8f,0.1f,1.4f);//4
	ObjCateg[0].add_bodypart("left hand",BODY_HAND,4, 1.2f,0.3f,0.01f,1.4f);//5
	/* Type, dam1,dam2,dam3 */
	ObjCateg[0].add_bodypart_damage(DAM_MACE,0.15f,0.02f,0);
	ObjCateg[0].last_bodypart().canhold=true;
	ObjCateg[0].add_bodypart("head",BODY_HEAD,1, 1.3f,0.4f,0.03f,1.7f);//6
	ObjCateg[0].last_bodypart().vital=true;	
	ObjCateg[0].add_bodypart("left leg",BODY_LEG,0, 1.1f,0.9f,0.2f,0.6f);//7
	ObjCateg[0].add_bodypart("left foot",BODY_FOOT,7, 1.2f,0.3f,0.012f,0.05f);//8
	ObjCateg[0].add_bodypart("right leg",BODY_LEG,0, 1.1f,0.9f,0.2f,0.6f);//9
	ObjCateg[0].add_bodypart("right foot",BODY_FOOT,9, 1.2f,0.3f,0.012f,0.05f);//10
	
	New();
	strcpy(ObjCateg[1].name,"chainmail");
	ObjCateg[1].add_bodypart("mail",BODY_CHEST,NOTHING, 5,1,0.5f,0);//0
	ObjCateg[1].add_coveredpart(BODY_CHEST);
	ObjCateg[1].add_coveredpart(BODY_ARM);	
	ObjCateg[1].add_coveredpart(BODY_ARM);
	ObjCateg[1].add_coveredpart(BODY_HEAD);
	ObjCateg[1].add_coveredpart(BODY_NECK);
	ObjCateg[1].clothes=true;
	
	New();
	strcpy(ObjCateg[2].name,"sword");
	ObjCateg[2].add_bodypart("blade",BODY_CHEST,NOTHING, 30,1,0.15f,0);//0
	ObjCateg[2].add_bodypart_damage(DAM_SWORD,1,0.2f,0);
	
	New();
	strcpy(ObjCateg[3].name,"gold coin");
	ObjCateg[3].numberable=true;
	
	New();
	strcpy(ObjCateg[4].name,"bag");
	ObjCateg[4].add_coveredpart(BODY_CHEST);
	ObjCateg[4].max_containweight=100;
	
	New();
	strcpy(ObjCateg[5].name,"bread");

	New();
	strcpy(ObjCateg[6].name,"silver coin");
	ObjCateg[6].numberable=true;
	
}
