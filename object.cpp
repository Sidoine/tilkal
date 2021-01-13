#include "tilkal.h"
#include "objects.h"
#include "doors.h"
#include "clients.h"
#include "objcategs.h"
#include "skills.h"
#include "tools.h"
#include "events.h"
#include "server.h"
#include "ai.h"

using namespace Tilkal;
using namespace TilTools;

void CObject::create_standard_bot(const char *_name, int _type)
{
	create_standard(0);//human
	type=ObjType::Bot;
	subtype=_type;
	strcpy(name,_name);
	give_standard_skills();
	if (AI[subtype].Spawn)
		(this->*AI[subtype].Spawn)();
}

void CObject::Init()
{
	player=NOTHING;
	
	for (int i=0;i<MAX_CARACS;i++)
		caracs.i[i]=0;

	race=0;
	door=NOTHING;
	prev_o=NOTHING;
	next_o=NOTHING;
	skill=NOTHING;
	container=NOTHING;
	in_object=NOTHING;
	prev_o=NOTHING;
	next_o=NOTHING;
	weight=1;
	max_containweight=0;
	max_weight=0;  
	opponent=NOTHING;
	act_time=0;
	desc=NULL;
	image=NOTHING;
}

void CObject::create_standard(int _categ)
{
	int i;

	creation_time=Server::t;
	categ=_categ;
	if (categ!=NOTHING)
	{
		if (ObjCateg[categ].numberable)
			quantity=1;
		else
			quantity=0;
		max_containweight=ObjCateg[categ].max_containweight;
		set_name(ObjCateg[categ].name);
		for (i=0;i<ObjCateg[categ].nbodyparts;i++)
			bodypart[i].reset(ObjCateg[categ].bodypart[i]);
	}
	compute_tot_size();
	for (i=0;i<MAX_CARACS;i++)
	  caracs.i[i]=20.0f+rand()*30.0f/RAND_MAX; // %
	type=ObjType::Obj;
}

int CObject::duplicate()
{
  int o=Object.New();

  if (o!=NOTHING)
  {
    int64 UIDsav = Object[o].UID;
    memcpy(&Object[o],&Object[Id],sizeof(Object[Id]));
    Object[o].creation_time=Server::t;
    Object[o].Id=o;
    Object[o].UID=UIDsav;
  }
  return o;
}

void CObject::compute_tot_size()
{
  tot_size=0;
  for (int i=0;i<ObjCateg[categ].nbodyparts;i++)
    if (bodypart[i].health>=0)
      tot_size+=bodypart[i].size;
}

float CObject::compute_health()
{
	float nb=0;

	for (int i=0;i<ObjCateg[categ].nbodyparts;i++)
		if (bodypart[i].health>0)
			nb+=bodypart[i].health;
	return nb;
}

float CObject::compute_max_health()
{
	float nb=0;

	for (int i=0;i<ObjCateg[categ].nbodyparts;i++)
		if (bodypart[i].max_health>0)
			nb+=bodypart[i].max_health;
	return nb;
}

void CObject::Destroy()
{
	int s,next;

	for (int o=in_object; o!=NOTHING; o=Object[o].next_o)
		Object[o].remove_from_obj();
	if (desc)
	{
		free(desc);
		desc=NULL;
	}
	remove_from_obj();
	s=skill;
	while (s!=NOTHING)
	{
		next=Skill[s].next;
		Skill.Remove(s);
		s=next;
	}
	strcpy(name,"");
	Id=NOTHING;
}

void CObject::Listen(const char *text, int from)
{
	assert(from!=NOTHING);
	if (type==ObjType::Human && play().client!=NOTHING &&
		Client[play().client].state==STATE_PLAY)
	{
		Client[play().client].WriteDialogue(Object[from].name,text);
	}
	if (type==ObjType::Bot && AI[subtype].Listen)
		(this->*AI[subtype].Listen)(text);
}

void CObject::ListenYourself(const char *text)
{
	if (type==ObjType::Human && play().client!=NOTHING &&
		Client[play().client].state==STATE_PLAY)
	{
		Client[play().client].WriteTuDis(text);
	}
}

void CObject::send_text(const char *text)
{
	if (type==ObjType::Human && play().client!=NOTHING &&
		Client[play().client].state==STATE_PLAY)
	{
		Client[play().client].WriteEvent(text);
	}
	/*L'IA s'en fout, non ?
	if (type==ObjType::Bot && AI[subtype].Listen)
		(this->*AI[subtype].Listen)(text);*/
}

void CObject::send_prompt()
{
	if (type==ObjType::Human && play().client!=NOTHING)
	{
		Client[play().client].send_prompt();
	}
}

void CObject::say(const char *text)
{
	if (container==NOTHING)
		return;
	Object[container].send_text_to_room_from(text,Id);
}

int CObject::obj_is_in_obj(char *name)
{
  int o;

  o=in_object;
  while (o!=NOTHING)
  {
    if (strstrnocase(Object[o].name,name))
      return o;
    o=Object[o].next_o;
  }
  return NOTHING;
}

void CObject::use_door(int d)
{
	static char buffer[BUFSIZ],nms[TMAXDOORNAME+1],nmd[TMAXDOORNAME+1];
	int dest;
	int r=container;
	int c=NOTHING;
	if (player!=NOTHING)
		c=Player[player].client;
	if (Door[d].closed)
	{
		if (c!=NOTHING)
		{
			sprintf(buffer,"It is closed.\r\n");
			Client[c].WriteError(buffer);
			return;
		}  
	}
	if (Door[d].r1==r)
	{
		dest=Door[d].r2;
		strcpy(nmd,Door[d].name1);
		strcpy(nms,Door[d].name2);
	}
	else
	{
		dest=Door[d].r1;
		strcpy(nmd,Door[d].name2);
		strcpy(nms,Door[d].name1);
	}
//	sprintf(buffer,"\r\n%s goes to the %s.\r\n",name,nmd);
//	Object[container].send_text_to_room_from(buffer,Id);
//	remove_from_obj();
	add_to_obj(dest);
//	sprintf(buffer,"\r\n%s arrives from the %s.\r\n",name,nms);
//	Object[dest].send_text_to_room_from(buffer,Id);
  	if (c!=NOTHING)
		Client[c].send_desc();
}

int CObject::can_wear_object(int o)
{
	int cc=categ;
	if (cc==NOTHING ||ObjCateg[cc].nbodyparts==0)
		return 0;

	int where=0;//Where can we wear it?
	int co=Object[o].categ;
	if (co==NOTHING)
		return 0;

/*	clog <<"C de l'object � mettre : "<<co <<'('<< ObjCateg[co].name <<") qui couvre "
			 << ObjCateg[co].ncoveredparts <<" parties"<<endl; */
	bool clothes=ObjCateg[co].clothes;
	for (int i=0; i<ObjCateg[co].ncoveredparts; i++)
	{
		EBodyPartType bpt=ObjCateg[co].coveredpart[i];
		int bj=1;
		int j;
		for (j=0; j<ObjCateg[cc].nbodyparts; j++)
		{
			if (ObjCateg[cc].bodypart[j].type==bpt && 
					((where & bj)==0))//Pas d�j� utilis� par ce m�me objet
			{
				//On v�rifie qu'il n'y a pas d�j� quelque
				//chose � cet endroit
				int f=in_object;
				if (clothes)
				{
					while (f!=NOTHING)
					{
						if (Object[f].clothes && 
							(Object[f].where & bj))
							break;//already used
						f=Object[f].next_o;
					}
				}
				else
				{
					while (f!=NOTHING)
					{
						if (!Object[f].clothes && 
							(Object[f].where & bj))
							break;//already used
						f=Object[f].next_o;
					}
				}
				if (f==NOTHING) // found !
				{
					where|=bj;
					break;
				}
			}
			bj<<=1;
		}
		if (j==ObjCateg[cc].nbodyparts) // no free body part
			return 0;
	}
	return where;
}

/* WARNING: at the present time, it returns the first weapon held. So if both hands hold
   a weapon, one of them is useless (depends on the declaration in objcategs.cpp) */
int CObject::is_holding()
{
	int i,f;
	for (f=in_object; f!=NOTHING; f=Object[f].next_o)
	{
		if (Object[f].held)
			for (i=0;i<ObjCateg[Object[f].categ].nbodyparts;i++)
				if (ObjCateg[Object[f].categ].bodypart[i].dam_type!=0)
					return f;
	}
	return NOTHING;
}

/* Adds 'nb' to the quantity of a numberable object and tests if it must be destroyed.
   Returns TRUE if the object was destroyed. */
bool CObject::add_quantity(int nb)
{
	weight+=(weight/quantity)*nb;
	quantity+=nb;
	if (quantity<=0)
	{
		Object.Remove(Id);
		return true;
	}
	return false;
}

bool CObject::add_to_obj(int c, int who, int number, bool hold)
{
	//On v�rifie qu'on s'ajoute pas dans soi-m�me
	int p=c;
	do
	{
		if (p==Id)
			return false;
		p=Object[p].container;	
	}
	while (p!=NOTHING);
	
	return AddToObj2(c,who,number,hold);
}

bool CObject::AddToObj2(int c, int who, int number, bool hold)
{
	bool _held=false;
	int _where=0;
	bool _clothes=false;
	
	float _weight;
	
	
	
	if (number<quantity)
	{
		if (number==0)
			number=1;
		_weight=weight*number/quantity;
	}
	else
		_weight=weight;

	// How much can I carry ?
	int f=c;
	while (Object[f].container!=NOTHING && Object[f].max_weight==0)
		f=Object[f].container;
	if (!(_weight<=Object[f].max_weight-Object[f].weight))
	{
		return false; // Too heavy.
	}
	
	if (hold)
	{
		// Find a free hand.
		int cc=Object[c].categ;
		if (ObjCateg[cc].nbodyparts==0)
		{
			return false; // No bodypart.
		}
		int jb=1;

		for (int j=0; j<ObjCateg[cc].nbodyparts; j++)
		{
			if (ObjCateg[cc].bodypart[j].canhold)
			{
				// Bodypart is full ?
				int f;
				for (f=Object[c].in_object; f!=NOTHING; f=Object[f].next_o)
					if (Object[f].held && (Object[f].where & jb))
						break;
				if (f==NOTHING) // We found an available hand.
				{
					_where=jb;
					_held=true;
					break;
				}
			}
			jb<<=1;
		}
		if (!_held)
		{
			return false;
		}
	}
	else
	{
		_where=Object[c].can_wear_object(Id);
		if (_where)
		{
			_clothes=ObjCateg[categ].clothes;
		}
		else
		{
//			clog <<"ne peut v�tir l'objet\n";
			//Can we contain it (not too heavy, not too big)?
			if ((_weight<= Object[c].max_containweight-Object[c].weight)
				&&(max_containweight<=Object[c].max_containweight))
			{
			}
			else
			{
//				clog <<"ne peut contenir l'objet\n";
				//look recursively
				int f=Object[c].in_object;
				while (f!=NOTHING)
				{
					if (f!=Id && add_to_obj(f,number,false))
						return true;
					f=Object[f].next_o;
				}
				return false;
			}
		}
	}
	
	if (number<quantity)
	{
		weight-=_weight;
		int f=container;
		quantity-=number;
		while (f!=NOTHING)
		{	
			Object[f].weight-=_weight;
			if (Object[f].container==NOTHING)
			{
				for (int o=Object[f].in_object; o!=NOTHING; o=Object[o].next_o)
				{
					int p,c;
					if ((p=Object[o].player)!=NOTHING && (c=Player[p].client)!=NOTHING)
					{
						Client[c].WriteChangeQuantity(Id);
					}
				}
			}	
			f=Object[f].container;
		}

		int z=Object.New();
		Object[z].create_standard(categ);
		Object[z].quantity=number;
		Object[z].weight=_weight;
		Object[z].AddToObjEx(c,_held,_where,_clothes,who);	
	}
	else
		AddToObjEx(c,_held,_where,_clothes,who);	
	return true;
}

void CObject::AddToObjEx(int c, bool _held, int _where, bool _clothes, int who)
{
	if (container!=NOTHING)
		remove_from_obj();
	
	int hmadded=quantity;
	
	held=_held; 
	where=_where;  
	clothes=_clothes;

	container=c;
	
	if (quantity>0)
	{
		for (int o=Object[c].in_object;o!=NOTHING;o=Object[o].next_o)
			if (Object[o].categ==categ)
			{
				quantity+=Object[o].quantity;
				weight+=Object[o].weight;
				Object.Remove(o);
				break;
			}
	}

	int p=Object[c].in_object;
	if (p!=NOTHING)
		Object[p].prev_o=Id;
	next_o=p;
	prev_o=NOTHING;
	Object[c].in_object=Id;

	//On augmente le poids
	int f=c;
	int last=f;
	int dansqui=NOTHING;
	while (f!=NOTHING)
	{	
		Object[f].weight+=weight;
		if (Object[f].type==ObjType::Human)
			dansqui=f;
		//On envoie l'info � tout le monde
		if (Object[f].container==NOTHING)
		{
			for (int o=Object[f].in_object; o!=NOTHING; o=Object[o].next_o)
			{
				int p,c;
				if ((p=Object[o].player)!=NOTHING && (c=Player[p].client)!=NOTHING)
				{
					Client[c].WriteAddObject(Id,who,hmadded);
					if (o==dansqui)
						Client[c].WriteInventory(Id);
				}
			}
		}		
		f=Object[f].container;
	}
}

void CObject::remove_from_obj()
{
  int p,c;

  c=container;
  if (c==NOTHING)
    return;
  p=Object[c].in_object;
  assert(p!=NOTHING);
  
  if (next_o!=NOTHING)
    Object[next_o].prev_o=prev_o;
  if (prev_o!=NOTHING)
    Object[prev_o].next_o=next_o;
  if (p==Id)
    Object[c].in_object=next_o;
  prev_o=next_o=NOTHING;
  container=NOTHING;
  int f=c;
  while (f!=NOTHING)
  {
    Object[f].weight-=weight;
		//On envoie l'info � tout le monde
		if (Object[f].container==NOTHING)
		{
			for (int o=Object[f].in_object; o!=NOTHING; o=Object[o].next_o)
			{
				int p,c;
				if ((p=Object[o].player)!=NOTHING && (c=Player[p].client)!=NOTHING)
				{
					Client[c].WriteRemoveObject(Id);
				}
			}
		}		
    f=Object[f].container;
  }
  	
}

/*
void CObject::add_to_room(int r, int nb)
{
	if (room!=NOTHING)
		remove_from_room();
	
	if (quantity>0)
	{
		for (int o=Object[r].in_object;o!=NOTHING;o=Object[o].next_o)
			if (Object[o].categ==categ)
			{
				quantity+=Object[o].quantity;
				weight+=Object[o].weight;
				Object.Remove(o);
				break;
			}
	}
	room=r;
	int p=Object[r].in_object;
	if (p!=NOTHING)
		Object[p].prev_o=Id;
	next_o=p;
	prev_o=NOTHING;
	Object[r].in_object=Id;	
} */

/*
void CObject::remove_from_room()
{
  int p,r;

  r=room;
  if (r==NOTHING)
    return;
  p=Object[r].in_object;
   room=NOTHING;

  if (p==NOTHING)
    return;

    if (next_o!=NOTHING)
	  Object[next_o].prev_o=prev_o;
  
  if (prev_o!=NOTHING)
	  Object[prev_o].next_o=next_o;
  if (p==Id)
	 Object[r].in_object= next_o;
  prev_o=next_o=NOTHING;
} */

int CObject::has_skill(int type)
{
	for (int p=skill; p!=NOTHING; p=Skill[p].next)
		if (Skill[p].type==type)
			return p;
	return NOTHING;
}

void CObject::NewSkill(int type, int val)
{
	int s=Skill.New(type,val);
	AddSkill(s);
}

void CObject::AddSkill(int s)
{
	int p;

	if ((p=has_skill(Skill[s].type))!=-1)
	{
		Skill[p].val=Skill[s].val;
		if (p!=s)
			Skill.Remove(s);
	}
	else
	{
		Skill[s].owner=Id;
		p=skill;
		if (p!=NOTHING)
			Skill[p].prev=s;
		Skill[s].next=p;
		Skill[s].prev=NOTHING;
		skill=s;
	}
}

void CObject::give_standard_skills()
{
	NewSkill(SKILL_MOVE,10);
	NewSkill(SKILL_HAND_FIGHT,18);
	NewSkill(SKILL_SWORD_FIGHT,12);
	NewSkill(SKILL_AXE_FIGHT,1);
	NewSkill(SKILL_BLOCK,30);
}

void CObject::NewEvent(ETypeEvent typ, int64 t)
{
	::Event.New(typ,UID,t);
}

void CObject::NewEvent(ETypeEvent typ, int64 t, int action)
{
	::Event.New(typ,UID,t,action);
}

void CObject::NewEvent(ETypeEvent typ, int64 t, int action, int64 data1)
{
	::Event.New(typ,UID,t,action,data1);
}

void CObject::NewEvent(ETypeEvent typ, int64 t, int action, int64 data1, int64 data2)
{
	::Event.New(typ,UID,t,action,data1,data2);
}

void CObject::NewEventRelat(ETypeEvent typ, int64 t)
{
	::Event.New(typ,UID,Server::t+t);
}

void CObject::NewEventRelat(ETypeEvent typ, int64 t, int action)
{
	::Event.New(typ,UID,Server::t+t,action);
}

void CObject::NewEventRelat(ETypeEvent typ, int64 t, int action, int64 data1)
{
	::Event.New(typ,UID,Server::t+t,action,data1);
}

void CObject::NewEventRelat(ETypeEvent typ, int64 t, int action, int64 data1, int64 data2)
{
	::Event.New(typ,UID,Server::t+t,action,data1,data2);
}

void CObject::NewAction(int64 dur, int action, int64 data1, int64 data2)
{
	if (act_time<Server::t)
		act_time=Server::t+dur;
	else
		act_time+=dur;
	::Event.New(EV_OBJECT,UID,act_time,action,data1,data2);
}

void CObject::NewAction(int64 dur, int action, int64 data1)
{
	NewAction(dur,action,data1,0);
}

void CObject::NewAction(int64 dur, int action)
{
	NewAction(dur,action,0,0);
}

int CObject::Find_Skill(int t)
{
  int sk;
  for (sk=skill;sk!=NOTHING;sk=Skill[sk].next)
    if (Skill[sk].type==t)
      return sk;
  return NOTHING;
}

int CObject::Highest_Bodypart_Damage()
{
  int i,bp=-1;
  float v,val=0;
  int nbparts=ObjCateg[categ].nbodyparts;
  for (i=0;i<nbparts;i++)
    if ((bodypart[i].health>0) && (ObjCateg[categ].bodypart[i].dam_type!=0))
    {
      v=ObjCateg[categ].bodypart[i].dam[0]+ObjCateg[categ].bodypart[i].dam[1];
      if (v>val)
      {
        bp=i;
        val=v;
      }
    }
  if (val>0)
    return bp;
  else
    return NOTHING;
}

void CObject::Set_Data(float a, float b, float c, float d, float e)
{
	data[0]=a;
	data[1]=b;
	data[2]=c;
	data[3]=d;
	data[4]=e;
}

/* Compute the damage for a bodypart of the weapon. */
float CObject::Compute_Weapon_Damage(int part)
{
	int nb=ObjCateg[categ].nbodyparts;
	float dam=0,bonus;

	if (nb<1)
		return 0;
	if (part>(nb-1))
		return 0;
	switch (ObjCateg[categ].bodypart[part].dam_type) {
		case DAM_SWORD:
		case DAM_AXE:
		case DAM_MACE:
			/* Basic weapon damage + damage specific of the object. */
			dam=ObjCateg[categ].bodypart[part].dam[0]+data[0];
			bonus=ObjCateg[categ].bodypart[part].dam[1]+data[1];
			dam = dam/2+randfloat(dam/2)+randfloat(bonus);
			break;
	}
	return dam;
}

int CObject::Object_On_Bodypart(int part)
{
	int o;
	for (o=in_object;o!=NOTHING;o=Object[o].next_o)
		if ((Object[o].where & (1<<part)) && ObjCateg[Object[o].categ].clothes)
			return o;
	return NOTHING;
}

float CObject::Compute_Bodypart_Protection(int part)
{
	int i,o;
	if ((o=Object_On_Bodypart(part))!=NOTHING)
	{
		float val=0;
		for (i=0;i<ObjCateg[Object[o].categ].nbodyparts;i++)
			val+= ObjCateg[Object[o].categ].bodypart[i].resist+Object[o].data[0];
		return val;
	}
	else
		return 0;
}

void CObject::Die()
{
	int i,sk;
	char buf[300];

	if ((type!=ObjType::Bot) && (type!=ObjType::Human))
		return;

	opponent=NOTHING;
	NewUID();
	if (type==ObjType::Human)
	{
		int o=duplicate();
		Player[player].object=o;
		Object[o].container=Object[o].prev_o=Object[o].next_o=NOTHING;
		Object[o].container=Object[o].in_object=Object[o].prev_o=Object[o].next_o=NOTHING;
		for (sk=skill;sk!=NOTHING;sk=Skill[sk].next)
			Skill[sk].owner=o;
		skill=NOTHING;
		for (i=0;i<MAX_BODYPARTS;i++)
			Object[o].bodypart[i].reset(ObjCateg[categ].bodypart[i]);
		for (i=in_object;i!=NOTHING;i=Object[i].next_o)
			Object[o].weight-=Object[i].weight;
		//    Object[o].add_to_room(START_ROOM);
		Object[o].send_text(death_buf);
		Client[play().client].state=STATE_LOGIN;
		Object[o].send_prompt();
	}
	sprintf(buf,"\r\n%s is dead. RIP",name);
	Object[container].send_text_to_room_from(buf,Id);
	strcpy(buf,"the corpse of ");
	strcat(buf,name);
	set_name(buf);
	type=ObjType::Obj;
}

/* Returns true if the bodypart is destroyed and object is dead. */
bool CObject::Check_Bodypart(int part)
{
	char buf[500];
	
	if (bodypart[part].health<0)
	{
		sprintf(buf,"\r\n%s's %s is destroyed.\r\n",name,ObjCateg[categ].bodypart[part].name);
		Object[container].send_text(buf);
		int p;
		bool Deleted;
		int nbparts=ObjCateg[categ].nbodyparts;
		do
		{
			Deleted=false;
			for (int i=0; i<nbparts; i++)
			{
			    if (bodypart[i].health>=0 
				    && (p=ObjCateg[categ].bodypart[i].linked_to)!=NOTHING
				    && bodypart[p].health<0)
			    {
				    bodypart[i].health=-1;
				    Deleted=true;
			    } 
			}
		}
		while (Deleted);
		compute_tot_size();
		for (int i=0;i<nbparts;i++)
		  if (bodypart[i].health<0 && ObjCateg[categ].bodypart[i].vital)
		  {
		    Die();
		    return true;
		  }
	}
	return false;
}

bool CObject::Attack(int e)
{
	int i,sk,blo,weap,memb_att,memb_def;
	float nb,sum,dam,success;
	char buf[500];

	/* Disable distant attack. */
	if (Object[e].container != container)
	  return true;

	/* What member do we target ? */
	sum=0;
	memb_def=-1;
	nb=randfloat(Object[e].tot_size);
	do {
	  if (Object[e].bodypart[++memb_def].health>=0)
	    sum+=Object[e].bodypart[memb_def].size;
	} while (nb>sum);

	/* *** What kind of attack ? *** */
	if ((weap=is_holding())!=NOTHING) /* Holding a weapon ? */
	{
		int cat=Object[weap].categ;
		for (i=0;i<ObjCateg[cat].nbodyparts;i++)
		{
			switch (ObjCateg[cat].bodypart[i].dam_type) {
				case DAM_SWORD:
					if ((sk=Find_Skill(SKILL_SWORD_FIGHT))==NOTHING)
						return true; /* Cannot fight. */
					break;
				case DAM_AXE:
					if ((sk=Find_Skill(SKILL_AXE_FIGHT))==NOTHING)
						return true; /* Cannot fight. */
					break;
				case DAM_MACE:
					if ((sk=Find_Skill(SKILL_MACE_FIGHT))==NOTHING)
						return true; /* Cannot fight. */
					break;
				break;
			}
			Skill[sk].Test_Progress();
			success=Skill[sk].Compute();
			if ((blo=Object[e].Find_Skill(SKILL_BLOCK))!=NOTHING) /* Victim can block ? */
				success-=Skill[blo].Compute();
			Skill[blo].Test_Progress();
			if (success<=0) /* The attack has failed. */
			{
				sprintf(buf,"\r\n%s attacks %s with %s but is blocked.\r\n",
					name,Object[e].name,Object[weap].name);
				Object[container].send_text_to_room(buf);
				return false;
			}
			dam=Object[weap].Compute_Weapon_Damage(i);
			dam*=caracs.c.strength/10;
			dam-=Object[e].Compute_Bodypart_Protection(memb_def) + Object[e].bodypart[memb_def].resist/10;
			if (dam<0) /* The bodypart resisted completely. */
			{
				sprintf(buf,"\r\n%s attacks %s with %s but the %s resists.\r\n",
					name,Object[e].name,Object[weap].name,
				ObjCateg[Object[e].categ].bodypart[memb_def].name);
				Object[container].send_text_to_room(buf);
				return false;
			}
			else /* Bodypart is damaged. */
			{
				sprintf(buf,"\r\n%s attacks %s with %s and makes %f damage to the %s.\r\n",
					name,Object[e].name,Object[weap].name,dam,
				ObjCateg[Object[e].categ].bodypart[memb_def].name);
				Object[container].send_text_to_room(buf);
			}
		}
	}
	else /* Bare hand fight. */
	{
	  if ((sk=Find_Skill(SKILL_HAND_FIGHT))==NOTHING)
	    return true; /* Cannot fight. Should flee, maybe. */

	  memb_att=Highest_Bodypart_Damage();
	  if (memb_att==NOTHING) /* No valid member. */
	    return true;

	  Skill[sk].Test_Progress();
	  success=Skill[sk].Compute();
	  if ((blo=Object[e].Find_Skill(SKILL_BLOCK))!=NOTHING) /* Victim can block ? */
	    success-=Skill[blo].Compute();
	  Skill[blo].Test_Progress();
	  if (success<=0) /* The attack has failed. */
	  {
	    sprintf(buf,"\r\n%s attacks %s with his %s but is blocked.\r\n",
	      name,Object[e].name,ObjCateg[categ].bodypart[memb_att].name);
	    Object[container].send_text_to_room(buf);
	    return false;
	  }

	  dam=(bodypart[memb_att].dam[0]+randfloat(bodypart[memb_att].dam[1])) * caracs.c.strength / 10;
	  dam-=Object[e].Compute_Bodypart_Protection(memb_def) + Object[e].bodypart[memb_def].resist/10;
	  if (dam<0) /* The bodypart resisted completely. */
	  {
	    sprintf(buf,"\r\n%s attacks %s with his %s but the %s resists.\r\n",
	      name,Object[e].name,ObjCateg[categ].bodypart[memb_att].name,
	      ObjCateg[Object[e].categ].bodypart[memb_def].name);
	    Object[container].send_text_to_room(buf);
		return false;
	  }
	  else /* Bodypart is damaged. */
	  {
	    sprintf(buf,"\r\n%s attacks %s with his %s and makes %f damage to the %s.\r\n",
	      name,Object[e].name,ObjCateg[categ].bodypart[memb_att].name,dam,
	      ObjCateg[Object[e].categ].bodypart[memb_def].name);
	    Object[container].send_text_to_room(buf);
	  }
	}

	Object[e].bodypart[memb_def].health -= dam; /* Remove health from the body part. */
	if (Object[e].Check_Bodypart(memb_def))
		return true;
	return false;
}

void CObject::Event(int action, int64 data1, int64 data2)
{
	switch (action) {
		case	AC_ATTACK:
			int w=IDfromUID(data1);
			if (!Object.IsAlive(data1)) /* If disappeared, don't consider as an opponent anymore. */
			{
				opponent=NOTHING;
				break;
			}
			if (!Attack(w)) /* Shall continue. */
			{
				int atime=(int)max(100,(101-caracs.c.dext)*30+400)+randnum(max(1,101-caracs.c.dext));
				NewAction(atime,AC_ATTACK,data1);
				if (Object[w].type==ObjType::Bot)
				  (&Object[w]->*AI[Object[w].subtype].React)(RE_ATTACKED,UID,0);
			}
			else /* Shall stop (probably dead). */
				opponent=NOTHING;
			break;
	}

}

int CObject::Has_A_Client()
{
	if ((type==ObjType::Human) && (player!=NOTHING))
		return Player[player].client;
	return NOTHING;
}


