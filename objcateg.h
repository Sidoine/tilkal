#ifndef _OBJCATEG_INC
#define _OBJCATEG_INC

namespace Tilkal
{
	enum EBodyPartType
	{
		BODY_HEAD,
		BODY_NECK,
		BODY_HAND,
		BODY_ARM,
		BODY_LEG,
		BODY_FOOT,
		BODY_WING,
		BODY_TAIL,
		BODY_CHEST,
		BODY_PAW,
		BODY_CLAW,
		BODY_MOUTH,
		BODY_HORN,
		BODY_TRUNK
	};

	enum EDamType {
		DAM_SWORD=1,
		DAM_AXE=2,
		DAM_MACE=3,
		DAM_FLAIL=4,
		DAM_ARROW=5
	};

	struct SBodyPartTemp
	{
		bool canhold;
		char name[32];
		EBodyPartType type;
		int linked_to;
		bool vital;
		float size,height,resist,health;
		float dam[3];
		int dam_type;
	};

	class CObjCateg:public CMyArrayMember
	{
	public:
		char name[32];
		bool numberable;
		float max_containweight;
		SBodyPartTemp bodypart[MAX_BODYPARTS];
		EBodyPartType coveredpart[MAX_BODYPARTS];
		int ncoveredparts,nbodyparts;
		bool clothes;
		void add_coveredpart(EBodyPartType type)
		{
			coveredpart[ncoveredparts++]=type;
		}
		void add_bodypart(char * name, EBodyPartType type, int linked_to, float resist,
		float health, float size, float height)
		{
			strncpy(bodypart[nbodyparts].name,name,32);
			bodypart[nbodyparts].linked_to=linked_to;
			bodypart[nbodyparts].dam_type=0;
			bodypart[nbodyparts].dam[0]=0;
			bodypart[nbodyparts].dam[1]=0;
			bodypart[nbodyparts].dam[2]=0;
			bodypart[nbodyparts].resist=resist;
			bodypart[nbodyparts].health=health;
			bodypart[nbodyparts].size=size;
			bodypart[nbodyparts].height=height;
			bodypart[nbodyparts].canhold=false;
			bodypart[nbodyparts++].type=type;
		}
		void add_bodypart_damage(int type, float dam1, float dam2, float dam3)
		{
			bodypart[nbodyparts-1].dam_type=type;
			bodypart[nbodyparts-1].dam[0]=dam1;
			bodypart[nbodyparts-1].dam[1]=dam2;
			bodypart[nbodyparts-1].dam[2]=dam3;
		}
		SBodyPartTemp & last_bodypart()
		{
			return bodypart[nbodyparts-1];
		}
	};
}

#endif
