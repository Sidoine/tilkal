#ifndef _OBJECT_INC
#define _OBJECT_INC

#include "objcateg.h"
#include "event.h"

namespace Tilkal
{

	namespace ObjType
	{
		enum EObjectType
		{
			Human=0,
			Bot=1,
			Obj=2,
			Room=3
		};
	}

struct SBodyPart
{
	float health,max_health,resist,size,height;
	float dam[3];
	int dam_type;
	void reset(SBodyPartTemp &bp)
	{
		resist=bp.resist;
		health=max_health=bp.health;
		size=bp.size;
		height=bp.height;
		memcpy(dam,bp.dam,sizeof(dam));
		dam_type=bp.dam_type;
	}
};

class CPlayer;
class CClient;

union Caracs 
{
	struct SCaracs {
		float strength,wisdom,dext,constit,charisma,courage,sight,hear,touch,taste,smell;
	} c;
	float i[MAX_CARACS];
};

class CObject:public CMyArrayMember
{
public:
	char name[TMAXNAME+1],*desc;
	short int desc_size;
	int64 creation_time;
	char type,subtype;
	int categ;
	int player;
	int door;
	int alt,inside;
	short int race;
	float weight,max_weight,max_containweight;
	Caracs caracs;
	SBodyPart bodypart[MAX_BODYPARTS];
	float tot_size;
	int skill;
	int container,in_object,prev_o,next_o;
	int quantity;
	int where; // head ? hand ? (it's a bit field)
	bool held; // by hand
	bool clothes;
	int image;
	int64 act_time;
	int64 opponent;
	float data[MAX_DATA_PER_OBJ];

	void set_name(const char *_name)
	{
		strncpy(name,_name,TMAXNAME);
	}
	void set_desc(const char *_desc)
	{
		if (desc)
			free(desc);
		desc=strdup(_desc);
		desc_size=(int)strlen(desc)+1;
	}
	void Set_Data(float a, float b, float c, float d, float e);
	void compute_tot_size();
	float compute_health();
	float compute_max_health();
	void create_standard(int categ);
	int duplicate();
	void create_standard_bot(const char *name, int type);
	void send_text(const char *text);
	void Listen(const char *text, int from);
	void ListenYourself(const char *text);
	void send_prompt();
	int obj_is_in_obj(char *name);
	void use_door(int d);
	int can_wear_object(int o);
	int is_holding();
	bool add_quantity(int nb);
	bool AddToObj2(int c,int number,int who,bool hold);
	bool add_to_obj(int c,int who,int number,bool hold=false);
	bool add_to_obj(int c,int who=NOTHING,bool hold=false)
	{
		return add_to_obj(c,who,quantity,hold);
	}
	void AddToObjEx(int c, bool _held, int _where, bool _clothes, int who);
	void remove_from_obj();
/*	void add_to_room(int r, int nb);
	void add_to_room(int r)
	{
		add_to_room(r,0);
	}
	void remove_from_room(); */
	int has_skill(int type);
	void NewSkill(int type, int val);
	void AddSkill(int s);
	void give_standard_skills();
	void say(const char *text);
	void listen_bot_ai(char *text);
	void Init();
	void NewEvent(ETypeEvent typ, int64 t);
	void NewEvent(ETypeEvent typ, int64 t,int action);
	void NewEvent(ETypeEvent typ, int64 t,int action, int64 data1);
	void NewEvent(ETypeEvent typ, int64 t,int action, int64 data1, int64 data2);
	void NewEventRelat(ETypeEvent typ, int64 t);
	void NewEventRelat(ETypeEvent typ, int64 t, int action);
	void NewEventRelat(ETypeEvent typ, int64 t, int action, int64 data1);
	void NewEventRelat(ETypeEvent typ, int64 t, int action, int64 data1, int64 data2);
	void NewAction(int64 dur, int action);
	void NewAction(int64 dur, int action, int64 data1);
	void NewAction(int64 dur, int action, int64 data1, int64 data2);
	void Event(int action, int64 data1, int64 data2);
	int Find_Skill(int t);
	int Highest_Bodypart_Damage();
	bool Check_Bodypart(int part);
	bool Attack(int vict);
	float Compute_Weapon_Damage(int part);
	int Object_On_Bodypart(int part);
	float Compute_Bodypart_Protection(int part);
	int Has_A_Client();
	void Die();

	/* Room stuff */
	void send_text_to_room_from(const char *text, int object);
	void send_text_to_room(const char *text);
	int is_an_exit(char *cmd);
	int is_an_obj_in_room(char *name);
	void create_room(const char *_name, const char *_desc);
	void Destroy();


	inline CPlayer & play();
	inline CClient & client();
};

}

#include "players.h"

namespace Tilkal
{

inline CPlayer &CObject::play()
{
	return Player[player];
}

}
#endif
