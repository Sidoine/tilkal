#include "tilkal.h"
#include "all.h"

using namespace Tilkal;
using namespace Createur;
using namespace TilTools;

namespace Tilkal
{
	struct FoundObject
	{
		int o;
		int Number;
	};

	namespace GMF
	{
		enum GeneralMoveFlags
		{
			FromGround=1,
			FromMe=2,
			ToGround=4,
			ToMe=8,
			Hold=16	
		};
	}
}

class CCommandes:public CClient
{
public:
	void GetObjectList(AutoArray<FoundObject> &ret, int nb_args, char ** arg, int where);
	void GeneralGive(int nb_args, char **arg);
	void GeneralMove(int nb_args, char **arg, int Flags);

	void AddBan(int nb_args, char ** arg);
	void Attack(int nb_args, char ** arg);
	void Banlist(int nb_args, char ** arg);
	void Change(int nb_args, char ** arg);
	void Close(int nb_args, char ** arg);
	void Demote(int nb_args, char ** arg);
	void Destroy(int nb_args, char ** arg);
	void Drop(int nb_args, char ** arg);
	void Edit(int nb_args, char ** arg);
	void Finish(int nb_args, char ** arg);
	void Forbid(int nb_args, char ** arg);
	void Forblist(int nb_args, char ** arg);
	void Genesis(int nb_args, char ** arg);
	void Get(int nb_args, char ** arg);
	void Go(int nb_args, char ** arg);
	void Goto(int nb_args, char ** arg);
	void Help(int nb_args, char ** arg);
	void Hold(int nb_args, char ** arg);
	void Inventory(int nb_args, char ** arg);
	void KillPlayer(int nb_args, char ** arg);
	void Last_Cmd(int nb_args, char ** arg);
	void Login(int nb_args, char ** arg);
	void Look(int nb_args, char ** arg);
	void Move(int nb_args, char ** arg);
	void Open(int nb_args, char ** arg);
	void Promote(int nb_args, char ** arg);
	void Quit(int nb_args, char ** arg);
	void Register(int nb_args, char ** arg);
	void Rehash(int nb_args, char ** arg);
	void Rules(int nb_args, char ** arg);
	void Say(int nb_args, char ** arg);
	void Score(int nb_args, char ** arg);
	void Setreport(int nb_args, char ** arg);
	void Shutdown(int nb_args,char **arg);
	void Skills(int nb_args, char ** arg);
	void Stats(int nb_args, char ** arg);
	void Take(int nb_args, char ** arg);
	void Tell(int nb_args, char ** arg);
	void Unban(int nb_args, char ** arg);
	void Unforbid(int nb_args, char ** arg);
	void Version(int nb_args, char ** arg);
	void Who(int nb_args, char ** arg);
};


void CCommandes::Register(int nb_args, char ** arg)
{
	switch (state) {
	case STATE_LOGIN:
		state=STATE_REGISTER;
		break;
	default:
		break;
	}
}


void CCommandes::Attack(int nb_args, char ** arg)
{
	int tst;
	int m,p;
	p=player;
	m=obj_num();

	if (nb_args<1)
	{
		WriteError("Syntax: attack <nick>\r\n");
		return;
	}
	
	if (!strcasecmp(Object[m].name,arg[1]))
	{
		WriteError("Do you really want to kill yourself ?\r\n");
		return;
	}

	tst=Object[Object[m].container].is_an_obj_in_room(arg[1]);
	if ((tst==NOTHING) || !Object.IsActive(tst))
	{
		WriteError("This character is not here.\r\n");
		return;
	}

	if (Object[tst].type==ObjType::Obj)
	{
		WriteError("You can not destroy an object this way.\r\n");
		return;
	}

	Object[m].opponent=Object[tst].UID;
	Object[m].NewAction(1000,AC_ATTACK,Object[tst].UID);

}

void CCommandes::Finish(int nb_args, char ** arg)
{
  int m,p;
  p=player;
  m=Player[p].object;

  switch (state)
	{
    case STATE_REGISTER:
      if ((!strcmp(Object[m].name,DEFAULT_PLAYER_NAME)) || (strlen(Object[m].name)<5))
      {
        WriteError("You must enter a valid name.");
        return;
      }
      if (strlen(Player[p].pass)<5)
      {
        WriteError("You must enter a valid password.");
        return;
      }
      state=STATE_LOGIN;
//      Object[m].type=HUMAN;
      Player[p].is_registered=true;
      /* Race caracteristics. */

      /* End of race caracteristics. */
      WriteEvent("Your character has been succesfuly registred.");
      break;
	case STATE_EDIT_OBJ:
		EditedObject=NOTHING;
		state=STATE_PLAY;
		break;
    default:
		break;
	}
	WriteEvent("You stop editing.");
}

void CCommandes::Login(int nb_args, char ** arg)
{
	int p,nb=0;

	if (nb_args<2)
	{
		WriteError("Syntax: login <nick> <password>\r\n");
		return;
	}
	
	if (ForbNick.Find(arg[1])!=NOTHING)
	{
		WriteError("This nick is forbidden !\r\n");
		return;
	}

	p=Player.Find(arg[1]);
	if (p==NOTHING)
	{
		WriteError("Unknown login name.\r\n");
		return;
	}
	
    if (strcmp(Player[p].pass,arg[2]))
	{
		WriteError("Wrong password.\r\n");
		return;
	}

	if ((Player[p].client!=NOTHING) && (Client[Player[p].client].state>=STATE_PLAY))
		Client.Remove(Player[p].client);

	if (player!=NOTHING)
	{
		Player[player].client=NOTHING;
		if (!Player[player].is_registered)
			Player.Remove(player);
	}

	Player[p].client=Id;
	player=p; 
	state=STATE_PLAY;
	obj().add_to_obj(START_ROOM);
	WriteEvent(motd_buf);
	send_desc();
	sprintf(buffer,"%s has logged in.",obj().name);
	Client.report(buffer,REPORT_LOGIN);
}

void CCommandes::Change(int nb_args, char ** arg)
{
  int i;
  int m,p,v;
  char *buf;
  m=obj_num();
				
  switch (state) {
    case STATE_REGISTER:
      if (nb_args<1)
      {
        WriteError("Syntax: change <field> <data>");
        return;
      }
      if (!strcmp(arg[1],"name"))
      {
        if (nb_args<2)
        {
          WriteError("Syntax: change name <your name>");
          return;
        }
        for (i=0;i<(int)strlen(arg[2]);i++)
          if ((arg[2][i]>='0') && (arg[2][i]<='9'))
          {
            WriteError("The name must NOT contain numbers.");
            return;
          }
        if (strlen(arg[2])<5)
        {
          WriteError("The name must be at least 5 characters.");
          return;
        }
        if (strlen(arg[2])>TMAXPLAYERNAME)
        {
          sprintf(buffer,"The name must be less than %d characters.",TMAXPLAYERNAME);
          WriteError(buffer);
          return;
        }
		if ((p=Player.Find(arg[2]))!=NOTHING)
        {
          sprintf(buffer,"\r\nThere is already someone with that name.\r\n");
          WriteError(buffer);
          return;
        }
        strcpy(Object[m].name,arg[2]);
      }
      if (!strcmp(arg[1],"pass"))
      {
        if (nb_args<2)
        {
          WriteError("\r\nSyntax: change pass <your password>\r\n");
          return;
        }
        if (strlen(arg[2])<5)
        {
          WriteError("\r\nThe password must be at least 5 characters.\r\n");
          return;
        }
        if (strlen(arg[2])>TMAXPASS)
        {
          sprintf(buffer,"\r\nThe password must be less than %d characters.\r\n",TMAXPASS);
          WriteError(buffer);
          return;
        }
        strcpy(Player[player].pass,arg[2]);
      }
      if (!strcmp(arg[1],"race"))
      {
        if (nb_args<2)
        {
			Writer Start(this);
	          WriteError("\r\nSyntax: change race <name of race>\r\n");
          WriteError("Available races are:\r\n");
	  for (i=0;i<MAX_RACES;i++)
	  {
	    buf=&buffer[0];
	    buf+=sprintf(buf,"%*s : %s\r\n",15,race_name[i],race_desc[i]);
            WriteError(buffer);
	  }
          return;
        }
        for (i=0;i<MAX_RACES;i++)
          if (!strncasecmp(arg[2],race_name[i],strlen(arg[2])))
            Object[m].race=i;
      }
      break;
    case STATE_EDIT_OBJ:
      if (nb_args<1)
      {
        WriteError("\r\nSyntax: change <field> <data>\r\n");
        return;
      }
      p=EditedObject;
      if (!strcmp(arg[1],"name"))
        state=STATE_WRITE_OBJ_NAME;
      if (!strcmp(arg[1],"desc"))
        state=STATE_WRITE_OBJ_DESC;
	  if ((!strcmp(arg[1],"alt")) && (Object[p].type==ObjType::Room))
      {
        if (nb_args<2)
        {
          WriteError("\r\nSyntax: change alt <altitude>\r\n");
		  return;
		}
		v=atoi(arg[2]);
		if ((v<MIN_ALT) || (v>MAX_ALT))
		{
          sprintf(buffer,"\r\nPlease provide a number between %d and %d.\r\n",MIN_ALT,MAX_ALT);
          WriteError(buffer);
		  return;
		}
        Object[p].alt=v;
      }
      break;
    default:
      WriteError("This command can not be used in this state.\r\n");
      break;
  }  
  
}

void CCommandes::Promote(int nb_args, char ** arg)
{
	int tst;
	int m,p;
	p=player;
	m=obj_num();

	if (nb_args<2)
	{
		WriteError("Syntax: promote <nick> <god level>\r\n");
		return;
	}
	
	if (!strcmp(Object[m].name,arg[1]))
	{
		WriteError("Not on yourself.\r\n");
		return;
	}

	EPlLevel l=(EPlLevel)atoi(arg[2]);
	tst=Player.Find(arg[1]);
	if (tst==NOTHING)
	{
		WriteError("This person does not exist.\r\n");
		return;
	}
	
	if (l>=Player[p].lvl)
	{
		WriteError("You cannot promote to a lever higher than yours - 1.\r\n");
		return;
	}

	if (!Player[tst].god)
		Player[tst].god=true;
	else if (Player[tst].lvl>=Player[p].lvl)
	{
		WriteError("You cannot promote this person.\r\n");
		return;
	}

	Player[tst].lvl=l;
	Player[tst].lvl_report=REPORT_NOTHING;
	sprintf(buffer,"%s promoted to the godly title: %s (lvl %d) !\r\n",
		Player[tst].obj().name,god_title[l],l);

	Client.report(buffer,REPORT_PROMOTE);
	
	if (Player[tst].client!=-1)
	{
		sprintf(buffer,"You have been promoted to the godly title: %s (lvl %d).\r\n",
			god_title[l],l);
		Client[Player[tst].client].WriteEvent(buffer);
	}
}

void CCommandes::Demote(int nb_args, char ** arg)
{
	int tst;
	int m,p;
	p=player;
	m=obj_num();

	if (nb_args<1)
	{
		WriteError("Syntax: demote <nick>\r\n");
		return;
	}

	if (!strcmp(Object[m].name,arg[1]))
	{
		WriteError("Not on yourself.\r\n");
		return;
	}

	tst=Player.Find(arg[1]);
	if (tst==NOTHING)
	{
		WriteError("This person does not exist.\r\n");
		return;
	}

	if (!Player[tst].god)
	{
		WriteError("You cannot demote a mortal one.\r\n");
		return;
	}

	if (Player[tst].lvl>=Player[p].lvl)
	{
		WriteError("You cannot demote this person.\r\n");
		return;
	}
	Player[tst].god=false;
	Player[tst].lvl=1;
	Player[tst].lvl_report=REPORT_NOTHING;

	sprintf(buffer,"%s has been demoted !\r\n",Player[tst].obj().name);
	Client.report(buffer,REPORT_DEMOTE);

	if (Player[tst].client!=NOTHING)
	{
		sprintf(buffer,"You have been demoted from your godly state !\r\n");
		Client[Player[tst].client].WriteEvent(buffer);
	}
}

void CCommandes::KillPlayer(int nb_args, char ** arg)
{
	int tst,i;
	int lvl;
	int m,mm;
	
	mm=obj_num();
	
	if (nb_args<1)
	{
        WriteError("Syntax: killplayer <ip>\r\n");
		return;
	}
    
	tst=NOTHING;
	for (i=0;i<Client.Length;i++)
	{
		if ((Client[i].Socket!=-1) && (!strcmp(Client[i].ip,arg[1])))
		{
			tst=i;
			break;
		}
	}
	if (tst==NOTHING)
	{
		WriteError("No such IP.\r\n");
		return;
	}

	m=Client[tst].player;
	if (m==-1)
		lvl=1;
	else
		lvl=Player[m].lvl;

	if (lvl>=Player[player].lvl)
	{
		WriteError("You cannot kill someone higher than you are.\r\n");
		return;
	}

	if (Client[tst].player==-1)
		sprintf(buffer,"IP [%s] disconnected by %s.\r\n",
			Client[tst].ip,Object[mm].name);
	else
		sprintf(buffer,"%s disconnected by %s.\r\n",Player[m].obj().name,Object[mm].name);
	Client.report(buffer,REPORT_KILLPLAYER);
	Client.Remove(tst);
}

void CCommandes::Banlist(int nb_args, char ** arg)
{
	int i;
	Writer Start(this);
	WriteEvent("List of banned IPs:\r\n");
	for (i=0;i<Ban.Length;i++)
		if (Ban.IsActive(i))
		{
			sprintf(buffer," %s\r\n",Ban[i].IP);
			WriteEvent(buffer);
		}
}

void CCommandes::Unban(int nb_args, char ** arg)
{
	int i,b;
	int m;

	m=obj_num();
	if (nb_args<1)
	{
		WriteError("Syntax: unban <ip>/all\r\n");
		return;
	}

	if (!strcmp(arg[1],"all"))
	{
		WriteEvent("All IPs are now unbanned.\r\n");
		sprintf(buffer,"All IPs unbanned by %s.\r\n",Object[m].name);
		Client.report(buffer,REPORT_UNBAN);

		for (i=0;i<Ban.Length;i++)
			Ban.Remove(i);
		return;
	}

	if ((b=Ban.Find(arg[1]))==NOTHING)
	{
		WriteError("This address is not banned.\r\n");
		return;
	}

	WriteEvent("IP is now unbanned.\r\n");
	sprintf(buffer,"IP [%s] unbanned by %s.\r\n",
		arg[1],Object[m].name);
	Client.report(buffer,REPORT_UNBAN);
	Ban.Remove(b);
}

void CCommandes::AddBan(int nb_args, char ** arg)
{
	int tst,i;
	int m,mm;

	mm=obj_num();
	if (nb_args<1)
	{
		WriteError("Syntax: ban <ip>\r\n");
		return;
	}

	if (Ban.Find(arg[1])!=NOTHING)
	{
		WriteError("This address is already banned.\r\n");
		return;
	}

	tst=NOTHING;
	for (i=0;i<Client.Length;i++)
	{
		if (Client.IsActive(i) && strcmp(Client[i].ip,arg[1])==0)
		{
			tst=i;
			break;
		}
	}
	if (tst==NOTHING)
	{
		return;
	}

	int lvl;
	m=Client[tst].player;
	if (m==-1)
		lvl=1;
	else
		lvl=Player[m].lvl;

	if (lvl>=Player[player].lvl)
	{
		WriteError("You cannot ban someone higher than you are.\r\n");
		return;
	}

	Ban.New(arg[1]);

	WriteEvent("IP is now banned.\r\n");
	sprintf(buffer,"IP [%s] banned by %s.\r\n",
		arg[1],Object[mm].name);
	Client.report(buffer,REPORT_BAN);
}

void CCommandes::Forblist(int nb_args, char ** arg)
{
	int i;

	Writer Start(this);
	WriteEvent("List of forbidden nicks:\r\n");
	for (i=0;i<ForbNick.Length;i++)
		if (ForbNick.IsActive(i))
		{
			sprintf(buffer," %s\r\n",ForbNick[i].name);
			WriteEvent(buffer);
		}
}

void CCommandes::Go(int nb_args, char **arg)
{
	int r=obj().container;
	if (r==NOTHING)
		return;

	if (nb_args<1)
		return;

	if (isdigit(arg[1][0]))
	{
		int ou=atoi(arg[1]);
		bool ok=false;
		for (int d=Object[r].door; d!=NOTHING; d=Door[d].next_door_of_room(r))
			if (d==ou)
			{
				ok=true;
				break;
			}
		if (ok)
		{
			obj().add_to_obj(Door[ou].OtherSide(r));
			send_desc();
		}
		else
			WriteError("Cette porte n'est pas accessible");
	}
	else
	{

	}
}

void CCommandes::Goto(int nb_args, char ** arg)
{
  int tst,r=0;
  int p=player;
  int m=obj_num();
	
  if (nb_args<1)
  {
    WriteError("Syntax: goto <nick>/<room number>\r\n");
    return;
  }
  if ((arg[1][0]>='0') && (arg[1][0]<='9')) /* Certainly a number. */
  {
    r=atoi(arg[1]);
	if (r<0 || r>=Object.Length || !Object.IsActive(r))
    {
      WriteError("Please enter a valid room number.\r\n");
      return;
    }
  }  
  else
  {
	  tst=Player.Find(arg[1]);
    if (tst==NOTHING) //Cette personne n'existe pas
    {
      WriteError("This character does not exist.\r\n");
      return;
    }
	if (Player[tst].client==NOTHING)
    {
      WriteError("This person is out of this world.\r\n");
      return;
    }
    if (tst==m)
    {
      WriteError("Hhmm, it seems that you are already here.\r\n");
      return;
    }
    r=Player[tst].obj().container;
  }
	int csav=Object[m].container;
  if (!Object[m].add_to_obj(r))
  {
  	WriteError("You can not enter this object.\r\n");
	return;
  }
  sprintf(buffer,"%s disappears in a flash of light.\r\n",Object[m].name);
  Object[csav].send_text_to_room_from(buffer,m);
  sprintf(buffer,"%s appears in a flash of light.\r\n",Object[m].name);
  Object[r].send_text_to_room_from(buffer,m);
  send_desc();
}

void CCommandes::Close(int nb_args, char ** arg)
{
  int d;
  int m=obj_num();
	
  if (nb_args<1)
  {
    WriteError("Syntax: close <exit name>\r\n");
    return;
  }
	if ((d=Object[Object[m].container].is_an_exit(arg[1]))==NOTHING)
	{
		WriteError("There is no such exit.\r\n");
		return;
	}

    if (Door[d].closed)
    {
      WriteError("It is already closed.\r\n");
      return;
    }
    Door[d].closed=true;
    WriteEvent("Ok.\r\n");
}

void CCommandes::Open(int nb_args, char ** arg)
{
  int d;
  int m=obj_num();
	
  if (nb_args<1)
  {
    WriteError("Syntax: open <exit name>\r\n");
    return;
  }
  if ((d=Object[Object[m].container].is_an_exit(arg[1]))!=NOTHING)
  {
    if (!Door[d].closed)
    {
      WriteError("It is already open.\r\n");
      return;
    }
    Door[d].closed=false;
    WriteEvent("Ok.");
  }
  else
    WriteError("There is no such exit.\r\n");

}

void CCommandes::Edit(int nb_args, char ** arg)
{
	int r;
	int m=obj_num();
	
	WriteError("WARNING ! This command is still under heavy development.\r\n");
	if (nb_args<1)
	{
		r=Object.New(DEFAULT_OBJECT_NAME);
		if (r==NOTHING)
		{
			WriteError("Cannot create the new object. Sorry.\r\n");
			return;
		}
	}
	else
	{
		if (isdigit(arg[1][0]))
		{
			r=atoi(arg[1]);
			if ((r<0) || (r>=Object.Length) || !Object.IsActive(r))
			{
				WriteError("Please provide a correct object number.\r\n");
				return;
			}
		}
		else
		{
			if ((r=Object[Object[m].container].is_an_obj_in_room(arg[1]))==NOTHING)
			{
				WriteError("Please provide the name of a material object in this room.\r\n");
				return;
			}
		}
	}
	state=STATE_EDIT_OBJ;
	EditedObject=r;
}

void CCommandes::Unforbid(int nb_args, char ** arg)
{
	int i,b;
	int m;
	
      m=obj_num();
	if (nb_args<1)
      {
        WriteError("Syntax: unforbid <nick>/all\r\n");
      }
      else
      {
        if (!strcmp(arg[1],"all"))
	{
          WriteEvent("All nicks are now unforbidden.\r\n");
          sprintf(buffer,"All nicks unforbidden by %s.\r\n",Object[m].name);
          Client.report(buffer,REPORT_UNFORBID);
		  for (i=0;i<ForbNick.Length;i++)
			  if (ForbNick.IsActive(i))
				  ForbNick.Remove(i);
	  return;
	}
	if ((b=ForbNick.Find(arg[1]))!=NOTHING)
	{
          WriteEvent("Nick is now unforbidden.\r\n");
          sprintf(buffer,"Nick [%s] unforbidden by %s.\r\n",
	          arg[1],Object[m].name);
          Client.report(buffer,REPORT_UNFORBID);
          ForbNick.Remove(b);
        }
	else
	  WriteError("This nick is not forbidden.\r\n");
      }
}

void CCommandes::Forbid(int nb_args, char ** arg)
{
	int tst,lvl;
	int mm;

	mm=obj_num();
	if (nb_args<1)
	{
		WriteError("Syntax: forbid <nick>\r\n");
		return;
	}
	
	if (ForbNick.Find(arg[1])!=-1)
	{
		WriteError("This nick is already forbidden.\r\n");
		return;
	}

	tst=Player.Find(arg[1]);
	if (tst!=-1) // That name does exist.
	{
		lvl=Player[tst].lvl;
		if (lvl>=Player[player].lvl)
		{
			WriteEvent("You cannot forbid someone higher than you are.\r\n");
			return;
		}
		else
		{
			if (Player[lvl].client!=-1)
			{
				Client[Player[tst].client].WriteEvent("Your nick has been forbidden.\r\n");
			}
		}
	}

	ForbNick.New(arg[1]);
	WriteEvent("Nick is now forbidden.\r\n");
	sprintf(buffer,"Nick [%s] forbidden by %s.\r\n",
		arg[1],Object[mm].name);
	Client.report(buffer,REPORT_FORBID);
}

void CCommandes::Destroy(int nb_args, char ** arg)
{
	int val;
	int m=obj_num();

	if (nb_args<1)
	{
		WriteError("Syntax: destroy <object number>/<object name in room>\r\n");
		return;
	}

	if (isdigit(arg[1][0]))
	{
		val=atoi(arg[1]);
		if ((val<0) || (val>=Object.Length) || !Object.IsActive(val))
		{
			WriteError("This is not a correct object number.\r\n");
			return;
		}
	}
	else
	{
		val=Object[Object[m].container].is_an_obj_in_room(arg[1]);
		if (val==-1)
		{
			WriteError("There is no such object in this room.\r\n");
			return;
		}
	}

	if (Object[val].type==ObjType::Human)
	{
		WriteError("This is a HUMAN player !\r\n");
		return;
	}

	sprintf(buffer,"Object number %d [%s] destroyed by %s.\r\n",val,Object[val].name,
		Object[m].name);
	Client.report(buffer,REPORT_DESTROY);
	if (Object[val].type==ObjType::Obj)
	{
		sprintf(buffer,"\r\nYou point your finger at %s.\r\n"
			"It disappears definitely in a flash of light.\r\n",Object[val].name);
		WriteEvent(buffer);
		sprintf(buffer,"\r\n%s points a finger at %s.\r\n"
			"It disappears definitely in a flash of light.\r\n",Object[m].name,
			Object[val].name);
		Object[Object[m].container].send_text_to_room_from(buffer,m);
	}
	else if (Object[val].type==ObjType::Bot)
	{
		sprintf(buffer,"\r\nYou raise your hands and invoke your divine power.\r\n"
			"%s is burnt into ashes.\r\n",Object[val].name);
		WriteEvent(buffer);
		sprintf(buffer,"\r\n%s raises his hands and invokes his divine power.\r\n"
			"%s is burnt into ashes.\r\n",Object[m].name,Object[val].name);
		Object[Object[m].container].send_text_to_room_from(buffer,m);
	}
	Object.Remove(val);
}

bool Match(const char * quoi, char ** critere, int ncriteres)
{
	bool ok=true;
	for (int a=0; a<ncriteres; a++)
	{
		if (critere[a][0]=='!')
		{
			if (strcmp(quoi,&critere[a][1])!=0)
			{
				ok=false;
				break;
			}
		}
		else if (critere[a][0]=='-')
		{
			if (strstrnocase(quoi,&critere[a][1]))
			{
				ok=false;
				break;
			}
		}
		else	if (!strstrnocase(quoi,critere[a]))
		{
			ok=false;
			break;
		}
	}
	return ok;
}

void CCommandes::GetObjectList(AutoArray<FoundObject> &ret, int nb_args, char ** arg, int inwhat)
{
	if ((nb_args==1) && (isdigit(arg[0][0])))
	{
		int nbo=atoi(arg[0]);
		if (!Object.IsActive(nbo))
			return;		
		ret.Add();
		ret.Last().o=nbo;
		ret.Last().Number=0;
		return;
	}
	if ((nb_args==2) && (isdigit(arg[0][0])) && (isdigit(arg[1][0])))
	{
		int nbo=atoi(arg[1]);
		if (!Object.IsActive(nbo))
			return;		
		ret.Add();
		ret.Last().o=nbo;
		int quant = atoi(arg[0]);
		if (quant>Object[nbo].quantity)
			ret.Last().Number=Object[nbo].quantity;
		else
			ret.Last().Number=quant;
		return;
	}

	AutoArray<FoundObject> where, found;
	where.Add();			
	where.Last().o=inwhat;

	int Start=nb_args-1;
	while (1)
	{
		int End=Start;
		//On cherche le nom de l'objet
		while (Start>=0 && strcmp(arg[Start],"in"))
			Start--;
		
		int NameArg=Start+1;
		int Number;
	
		if (NameArg>End)
			Number=-1;
		else	if (isdigit(arg[NameArg][0]))
			Number=atoi(arg[NameArg++]);
		else if (strcmp(arg[NameArg],"all")==0)
		{
			Number=-1;
			NameArg++;
		}
		else
			Number=1;
		
		int NameLastArg=NameArg;
		
		while (NameLastArg<End && !isdigit(arg[NameLastArg][0]))
			NameLastArg++;
		int First;
		if (NameLastArg<=End && isdigit(arg[NameLastArg][0]))
			First=atoi(arg[NameLastArg--])-1;
		else 
			First=0;
		
		for (int i=0; i<where.Length; i++)
		{
			int r=where[i].o;
			for (int p=Object[r].door; p!=NOTHING; p=Door[p].next_door_of_room(r))
			{
				if (NameArg<=End
					&& Match(Door[p].name(r),&arg[NameArg],NameLastArg-NameArg+1))
				{
					if (First<=0)
					{
						if (Number)
						{
							found.Add();
							found.Last().o=Door[p].OtherSide(r);
							found.Last().Number=0;
							Number--;
						}
					}
					First--;
				}					
			}
			
			for (int o=Object[r].in_object; o!=NOTHING; o=Object[o].next_o)
			{
				if (NameArg<=End
					&& Match(Object[o].name,&arg[NameArg],NameLastArg-NameArg+1))
				{
					int dj=Object[o].quantity?Object[o].quantity:1;
					if (dj>First)
					{
						if (Number)
						{
							found.Add();
							found.Last().o=o;
							
							if (Object[o].quantity)
							{
								int n=Number;
								if (First>0)
									n+=First;
								if (n>Object[o].quantity || n<0)
									n=Object[o].quantity;
								if (First>0)
									n-=First;
								found.Last().Number=n;
								Number-=n;
							}
							else
							{
								found.Last().Number=0;
								Number--;
							}
						}
					}
					First-=dj;
				}
			}
		}
				
		if (Start>=0)
		{
			where=found;
		}
		else
		{
			ret=found;
			break;
		}
		Start--;
	}
}

void CCommandes::GeneralGive(int nb_args, char **arg)
{
	int m=obj_num();



}

void CCommandes::GeneralMove(int nb_args, char **arg, int Flags)
{
	int m=obj_num();
	int r=Object[m].container;

	int posinto=1;
	int hasinto=1;
	
	while (posinto<=nb_args && (hasinto=strcmp(arg[posinto],"into")))
		posinto++;
	
	hasinto=!hasinto;
	
	AutoArray<FoundObject> what;
	GetObjectList(what, hasinto?posinto-1:nb_args,&arg[1], (Flags & GMF::FromGround)?r:m);
	if (what.Length==0)
	{
		WriteError("There is no such object.\r\n");
		return;
	}
	
	AutoArray<FoundObject> where;
	if (hasinto)
	{
		GetObjectList(where, nb_args-posinto,&arg[posinto+1], (Flags & GMF::ToGround)?r:m);
		if (where.Length==0)
		{
			WriteError("There is no such object 'into'");
			return;
		}
	}
	
	Writer Start(this);

	for (int i=0;i<what.Length; i++)
	{
		int obj=what[i].o;
		if (obj==m)
		{
		//	WriteError("Tu peux pas te ramasser.\r\n");
			continue;
		}
		if (Object[obj].type!=ObjType::Obj)
		{
			WriteError("You cannot take a creature, yet.\r\n");
      		continue;
		}
		bool Reussi;
		int toobj;
		
		if (hasinto)
		{
			Reussi=true;
			for (int j=0;j<where.Length;j++)
			{
				if (Object[obj].add_to_obj(where[j].o,m,what[i].Number,(Flags & GMF::Hold)?true:false))
				{
					toobj=where[j].o;
					Reussi=true;
					break;
				}
			}
		}
		else
		{
			toobj=(Flags & GMF::ToGround)?r:m;
			Reussi=Object[obj].add_to_obj(toobj,m,what[i].Number,(Flags & GMF::Hold)?true:false);
		}
		
		
		if (Reussi)
		{
			/*char *b=buffer;
			b+=sprintf(b,"%s moved to %s.",Object[obj].name,Object[toobj].name);
			if (what[i].Number>1)
				b+=sprintf(b," <x%d>",what[i].Number);
			b+=sprintf(b,"\r\n");
			WriteEvent(buffer);
			b=buffer;
			b+=sprintf(buffer,"%s moves %s to %s.",Object[m].name,Object[obj].name,
					Object[toobj].name);
			if (what[i].Number>1)
				b+=sprintf(b," <x%d>",what[i].Number);
			b+=sprintf(b,"\r\n");
			Object[r].send_text_to_room(buffer);*/
		}
		else
		{
			sprintf(buffer,"You have no room for %s.",Object[obj].name);
			WriteError(buffer);
		} 
	}
}

void CCommandes::Hold(int nb_args, char ** arg)
{
	if (nb_args<1)
	{
		WriteError("Syntax: hold <object>/<creature>\r\n");
		return;
	}
	GeneralMove(nb_args,arg,GMF::FromMe|GMF::ToMe|GMF::Hold);
}

void CCommandes::Take(int nb_args, char ** arg)
{
	if (nb_args<1)
	{
		WriteError("Syntax: take <object>/<creature>\r\n");
		return;
	}
	GeneralMove(nb_args,arg,GMF::FromGround|GMF::ToMe|GMF::Hold);
}

void CCommandes::Move(int nb_args, char ** arg)
{
	if (nb_args<1)
	{
		WriteError("Syntax: move <object>/<creature>\r\n");
		return;
	}
	GeneralMove(nb_args,arg,GMF::FromGround|GMF::ToGround);
}

void CCommandes::Get(int nb_args, char ** arg)
{
	if (nb_args<1)
	{
		WriteError("Syntax: get <object>/<creature>\r\n");
		return;
	}
	GeneralMove(nb_args,arg,GMF::FromGround|GMF::ToMe);
}

void CCommandes::Drop(int nb_args, char ** arg)
{
	if (nb_args<1)
	{
		WriteError("Syntax: drop <object>/<creature>\r\n");
		return;
	}
	GeneralMove(nb_args,arg,GMF::FromMe|GMF::ToGround);
}

void CCommandes::Who(int nb_args, char ** arg)
{
	int i,nb=0;
	int m,p,tst,size=TMAXPLAYERNAME;
	char *buf;
	p=player;
	m=obj_num();
	
	Writer Start(this);
	buff.Add("Event('<table><tr><th>Name</th><th>Level</th><th>Race</th></tr>");
    for (i=0;i<Client.Length;i++)
	{
		if (Client.IsActive(i))
		{
			buf=&buffer[0];
			tst=Client[i].player;
			int o=Player[tst].object;
			if ((Client[i].state!=STATE_PLAY) && !Player[p].god)
				continue;
			
			nb++;
			buff.Add("<tr><td>");
			buff.AddQuoted(Object[o].name);
			buff.Add("</td><td>");
			
			if (Player[tst].god)
				buff.AddQuoted(god_title_abbrev[Player[tst].lvl]);
			else
				buff.Add(Player[tst].lvl);
			buff.Add("</td><td>");
			buff.AddQuoted(race_name_abbrev[Object[o].race]);

			if (m!=-1 && Player[p].god)
			{
				buff.Add(Client[i].ip);
				
				if (Client[tst].state<STATE_PLAY)
					buff.Add(" <login>");
			}
            buff.Add("</td></tr>");
		}
	}
	buff.Add("</table><p>A total of ");
	buff.Add(nb);
	buff.Add("displayed.</p>');");
}

void CCommandes::Look(int nb_args, char ** arg)
{
	int l=0;
	int m=obj_num();
	int r=Object[m].container;

	if (nb_args==0)
	{
		send_desc();
		return;
	}

	Writer Start(this);
	
	AutoArray<FoundObject> what;
	if (Object[r].in_object!=NOTHING) /* In case you are a god and you look from afar. */
	{
		GetObjectList(what,nb_args,&arg[1],r);
		l+=what.Length;
		if (what.Length>0)
		{
			for (int i=0;i<what.Length; i++)
			{
				int obj=what[i].o;
				buff.Add("Event('You look at ");
				buff.AddQuoted(Object[obj].name);
				buff.Add("');");
				if (Object[obj].desc)
					WriteEvent(Object[obj].desc);	
				if (Object[obj].in_object!=NOTHING)
					DisplayRecursiveInventory(obj,2,false);
			}
		}
	}
	what.Empty();
	if (Object[m].in_object!=NOTHING)
	{
		GetObjectList(what,nb_args,&arg[1],m);
		l+=what.Length;
		if (what.Length>0)
		{
			for (int i=0;i<what.Length; i++)
			{
				int obj=what[i].o;
				buff.Add("Event('You look at ");
				buff.AddQuoted(Object[obj].name);
				buff.Add("');");	
				if (Object[obj].desc)
					WriteEvent(Object[obj].desc);	
				if (Object[obj].in_object!=NOTHING)
					DisplayRecursiveInventory(obj,2,false);
			}
		}
	}
	if (l==0)
	{
		WriteError("There is no such object or people.");
	}
}

void CCommandes::Say(int nb_args, char ** arg)
{
  if (nb_args>0)
    obj().say(arg[1]);
  else
    WriteError("Syntax: say <something>\r\n");	
}

void CCommandes::Score(int nb_args, char ** arg)
{
	/*int i,m,p;
	char *b;
	p=player;
	m=obj_num();*/

	Writer Start(this);
	WriteEvent("/---------------------------------------------------\\\r\n");
	WriteEvent("|               Character informations              |\r\n");
	WriteEvent("+---------------------------------------------------+\r\n");
	/*b=buffer;
	b+=sprintf(buffer,"| Name: %-*s Level: ",25,obj().name);
	if (Player[p].god)
		b+=sprintf(b,"%-*s |\r\n",10,god_title[Player[p].lvl]);
	else
		b+=sprintf(b,"%-*d |\r\n",10,Player[p].lvl);
	write_socket(buffer);
	sprintf(buffer,"| Race: %-*s  Weight: %*d/%-*d |\r\n",15,race_name[obj().race],
		3,(int)obj().weight,14,(int)obj().max_weight);
	write_socket(buffer);
	// Caracteristics
	sprintf(buffer,"| STR: %-*d  WIS: %-*d  DEX: %-*d  CON: %-*d  CHA: %-*d |\r\n",
		3,(int)obj().carac[CARAC_STR],	3,(int)obj().carac[CARAC_WISDOM],
		3,(int)obj().carac[CARAC_DEXT],3,(int)obj().carac[CARAC_CONSTIT],
		4,(int)obj().carac[CARAC_CHARISMA]);
	write_socket(buffer);
	sprintf(buffer,"| COU: %-*d  SI/HE/TO/TA/SM: %-*d/%-*d/%-*d/%-*d/%-*d |\r\n",
  		3,(int)obj().carac[CARAC_COURAGE],3,(int)obj().carac[CARAC_SIGHT],
		3,(int)obj().carac[CARAC_HEAR],3,(int)obj().carac[CARAC_TOUCH],
		3,(int)obj().carac[CARAC_TASTE],7,(int)obj().carac[CARAC_SMELL]);
	write_socket(buffer);
	// Bodyparts
	for (i=0;i<ObjCateg[obj().categ].nbodyparts;i++)
	{
		sprintf(buffer,"| %*s : %*.2f / %-*.2f |\r\n",20,ObjCateg[obj().categ].bodypart[i].name,
			5,obj().bodypart[i].health,18,obj().bodypart[i].max_health);
		write_socket(buffer);
	}
	write_socket("\\---------------------------------------------------/\r\n");
*/
}


void CCommandes::Tell(int nb_args, char ** arg)
{
	int tst;
	int m,p;
	p=player;
	m=obj_num();

	if (nb_args>=2)
    {
		tst=Player.Find(arg[1]);
		if (tst==-1) //Cette personne n'existe pas
			WriteError("This person does not exist.\r\n");
		else if (Player[tst].client!=-1) // That person has logged in.
		{
			sprintf(buffer,"\r\n(PRIVATE) [%s] %s\r\n",Object[m].name,arg[2]);
			Client[Player[tst].client].WriteEvent(buffer);
			sprintf(buffer,"\r\n(PRIVATE to %s) %s\r\n",arg[1],arg[2]);
			WriteEvent(buffer);
		}
		else
			WriteError("This person has not logged in.\r\n");
	}
	else
		WriteError("Syntax: send <someone> <something>\r\n");
}

void CCommandes::Quit(int nb_args, char ** arg)
{
  int m;
  m=obj_num();

  if (state>=STATE_PLAY)
  {
    sprintf(buffer,"\r\n%s has left this realm.\r\n",Object[m].name);
	Object[Object[m].container].send_text_to_room_from(buffer,m);
  }
  
  WriteEvent("Goodbye");
  Client.Remove(Id);
}

void CCommandes::Last_Cmd(int nb_args, char ** arg)
{
	char temp_cmd[BSIZE];

	strcpy(temp_cmd,last_cmd);
	ExecuterCommande(temp_cmd);
}

void CCommandes::Version(int nb_args, char ** arg)
{
  sprintf(buffer,"Tilkal server version %s\r\n",VERSION);
  WriteEvent(buffer);
}

void CCommandes::Stats(int nb_args, char ** arg)
{
  int v,nb[4];
  Writer Start(this);

  if (nb_args<1)
  {
    WriteEvent("World statistics:\r\n");
    v=0;
    for (int i=0;i<Object.Length;i++)
      if (Object.IsActive(i))
        v++;
    sprintf(buffer,"- Allocated objects: %d   Used: %d   Free: %d\r\n",Object.Allocated,v,Object.Allocated-v);
    WriteEvent(buffer);
    memset(nb,0,4*sizeof(int));
    for (int i=0;i<Object.Length;i++)
    {
	if (!Object.IsActive(i))
		continue;
	nb[Object[i].type]++;
    }
    sprintf(buffer,"- Object types: H: %d  B: %d  O: %d  R: %d\r\n",
      nb[0],nb[1],nb[2],nb[3]);
    WriteEvent(buffer);
	v=nb[0]=0;
    for (int i=0;i<Player.Length;i++)
    {
      if (Player.IsActive(i))
        v++;
      else
        continue;
      if (Player[i].god)
        nb[0]++;
    }
		sprintf(buffer,"- Allocated players: %d   Used: %d   Free: %d   Imm: %d\r\n",
			Client.Allocated,v,Client.Allocated-v,nb[0]);
		WriteEvent(buffer);
	}
}

/* 'nspaces' is the blank shift at the beginning of the line.
   'recurs' is true if it must display all levels. */
int CClient::DisplayRecursiveInventory(int o, int nspaces, bool recurs)
{
	char Small[300];
	int nb=0;
	
	int co=Object[o].categ;
	
	for (int p=Object[o].in_object;p!=NOTHING;p=Object[p].next_o)
	{
		if (nspaces)
			memset(buffer,' ',nspaces);
		char *b=&buffer[nspaces];
		if (Object[p].where)
		{
                  char *s=&Small[0];
		  s+=sprintf(s,"[");
		  int jb=1;
		  for (int j=0; j<ObjCateg[co].nbodyparts; j++)
		  {
		    if (Object[p].where & jb)
		      s+=sprintf(s,"%s | ",ObjCateg[co].bodypart[j].name);
		    jb<<=1;
		  }	
          s-=3;
		  s+=sprintf(s,"] ");
		  b+=sprintf(b,"%-*s",35,Small);
		}
		else
		  b+=sprintf(b,"%-*s",35,"(inside) ");
		b+=sprintf(b,"%s",Object[p].name);
		if (Object[p].quantity>1)
			b+=sprintf(b," <x%d>",Object[p].quantity);
/*		if (Object[p].clothes)
			b+=sprintf(b," (clothes)");
		if (Object[p].held)
			b+=sprintf(b," (held)"); */
		b+=sprintf(b,"\r\n");
		WriteEvent(buffer);
		nb++;
		if ((Object[p].in_object!=NOTHING) && recurs)
			nb+=DisplayRecursiveInventory(p,nspaces+2,true);
	}
 	return nb;
}

void CCommandes::Inventory(int nb_args, char ** arg)
{
  int o=obj_num();

	Writer Start(this);
	
	WriteEvent("------------------------ You own -------------------------\r\n");
	if (DisplayRecursiveInventory(o,0,true)==0)
		WriteEvent("  -nothing-\r\n");
	sprintf(buffer,"\r\n<Weight: %d/%d>\r\n",
		  (int)Object[o].weight,(int)Object[o].max_weight);
	WriteEvent(buffer);
}

void CCommandes::Skills(int nb_args, char ** arg)
{
	int p,nb=0,o=obj_num();

	Writer Start(this);
	WriteEvent("-- You are skilled in: --\r\n");
	p=Object[o].skill;
	strcpy(buffer,"");
	char *b=buffer;
	while (p!=NOTHING)
	{
		b+=sprintf(b,"[%s : %d %%]  ",SkillProt[Skill[p].type].name,Skill[p].val);
		if (strlen(buffer)>=LINE_LENGTH)
		{
			WriteEvent(buffer);
			strcpy(buffer,"");
			b=buffer;
		}
		nb++;
		p=Skill[p].next;
	}
	if (nb==0)
		WriteEvent("  -no skill-");
	else
		WriteEvent(buffer);
}

void CCommandes::Rules(int nb_args, char ** arg)
{
	int o=obj_num();

	if (nb_args<1)
	{
		WriteError("Syntax: rules start/admin/authors\r\n");
		if (Player[Object[o].player].god)
			WriteError("        immort\r\n");
	}
	else
	{
		if (!strcmp(arg[1],"start"))
			read_book(rules_start_buf);
		if (!strcmp(arg[1],"admin"))
			read_book(rules_admin_buf);
		if (!strcmp(arg[1],"authors"))
			read_book(rules_authors_buf);
		if ((!strcmp(arg[1],"immort")) && Player[Object[o].player].god)
			read_book(rules_immort_buf);
  }
}

void CCommandes::Help(int nb_args,char**arg)
{
	disp_help();
}

void CCommandes::Shutdown(int nb_args,char**arg)
{
	if (nb_args<1)
	{
		WriteError("Syntax: shutdown now\r\n");
		return;
	}

	if (strcmp(arg[1],"now")==0)
	{
		sprintf(buffer,"Server was shutdown by %s.\r\n",obj().name);
		Client.report(buffer,REPORT_NOTHING);
		Server::shut_down();
	}
}

void CCommandes::Rehash(int nb_args, char ** arg)
{
  int m;
  m=obj_num();

  Server::LoadTexts();
  WriteEvent("Text files reloaded.\r\n");
  sprintf(buffer,"Text files reloaded by %s\r\n",Object[m].name);
  Client.report(buffer,REPORT_REHASH);
}

void CCommandes::Setreport(int nb_args, char ** arg)
{
  int m,p;
  p=player;
  m=obj_num();

  if (nb_args<1)
  {
		Writer Start(this);
		WriteError("Syntax: setreport <min lvl of reports>\r\n");	
		sprintf(buffer,"  Your current report level is %d.\r\n",Player[p].lvl_report);
		WriteEvent(buffer);	
  }
  else
  {
    if (atoi(arg[1])<0)
    {
      WriteError("The level must be at least 0.\r\n");	
      return;
    }
    if (atoi(arg[1])>11)
    {
      sprintf(buffer,"The level must be lower than %d.\r\n",GOD_HIGHEST);
      WriteError(buffer);	
      return;
    }
    Player[p].lvl_report=(EReportLevel)atoi(arg[1]);
    sprintf(buffer,"Your report level is now %d.\r\n",Player[p].lvl_report);
    WriteEvent(buffer);	
  }
}


void CCommandes::Genesis(int nb_args, char ** arg)
{
	CreerUnivers(0);
}

#define COMMANDE(x) (FonctionCommande)ADRESSEFONC(CCommandes::x)

SDefCommande CClient::DefCommande[200]=
{
{"!",			STATE_PLAY,		false,	1,				0,	false,	COMMANDE(Last_Cmd)},
{"attack",		STATE_PLAY,		false,	1,				1,	false,	COMMANDE(Attack)},
{"ban",			STATE_PLAY,		true,	GOD_ARCHANGEL,	1,	false,	COMMANDE(AddBan)},
{"banlist",		STATE_PLAY,		true,	GOD_SPIRIT,		0,	false,	COMMANDE(Banlist)},
{"change",		STATE_ALL,		false,	1,				2,	false,	COMMANDE(Change)},
{"close",		STATE_PLAY,		false,	1,				1,	false,	COMMANDE(Close)},
{"demote",		STATE_PLAY,		true,	GOD_GOD,		1,	false,	COMMANDE(Demote)},
{"destroy",		STATE_PLAY,		true,	GOD_ANGEL,		1,	false,	COMMANDE(Destroy)},
{"drop",		STATE_PLAY,		false,	1,				20,	false,	COMMANDE(Drop)},
{"edit",		STATE_PLAY,		true,	GOD_ANGEL,		2,	false,	COMMANDE(Edit)},
{"finish",		STATE_ALL,		false,	1,				0,	false,	COMMANDE(Finish)},
{"forbid",		STATE_PLAY,		true,	GOD_HIGHGOD,	1,	false,	COMMANDE(Forbid)},
{"forblist",	STATE_PLAY,		true,	GOD_SPIRIT,		0,	false,	COMMANDE(Forblist)},
{"genesis",		STATE_PLAY,		true,	GOD_HIGHEST,	0,	false,	COMMANDE(Genesis)},
{"get",			STATE_PLAY,		false,	1,				20,	false,	COMMANDE(Get)},
{"go",			STATE_PLAY,		false,	1,				1,	false,	COMMANDE(Go)},
{"goto",		STATE_PLAY,		true,	GOD_SPIRIT,		1,	false,	COMMANDE(Goto)},
{"help",		STATE_ALL,		false,	1,				0,	false,	COMMANDE(Help)},
{"hold",		STATE_PLAY,		false,	1,				20,	false,	COMMANDE(Hold)},
{"inventory",	STATE_PLAY,		false,	1,				0,	false,	COMMANDE(Inventory)},
{"killplayer",	STATE_PLAY,		true,	GOD_ANGEL,		1,	false,	COMMANDE(KillPlayer)},
{"login",		STATE_LOGIN,	false,	1,				2,	false,	COMMANDE(Login)},
{"look",		STATE_PLAY,		false,	1,				20,	false,	COMMANDE(Look)},
{"move",		STATE_PLAY,		false,	1,				20,	false,	COMMANDE(Move)},
{"open",		STATE_PLAY,		false,	1,				1,	false,	COMMANDE(Open)},
{"promote",		STATE_PLAY,		true,	GOD_GOD,		2,	false,	COMMANDE(Promote)},
{"quit",		STATE_ALL,		false,	1,				0,	false,	COMMANDE(Quit)},
{"register",	STATE_LOGIN,	false,	1,				0,	false,	COMMANDE(Register)},
{"rehash",		STATE_PLAY,		true,	GOD_HIGHGOD,	0,	false,	COMMANDE(Rehash)},
{"rules",		STATE_PLAY,		false,	1,				1,	false,	COMMANDE(Rules)},
{"say",			STATE_PLAY,		false,	1,				1,	true,	COMMANDE(Say)},
{"score",		STATE_PLAY,		false,	1,				0,	false,	COMMANDE(Score)},
{"setreport",	STATE_PLAY,		true,	GOD_GHOST,		1,	false,	COMMANDE(Setreport)},
{"shutdown",	STATE_PLAY,		true,	GOD_HIGHEST,	1,	false,	COMMANDE(Shutdown)},
{"skills",		STATE_PLAY,		false,	1,				0,	false,	COMMANDE(Skills)},
{"stats",		STATE_PLAY,		true,	GOD_GHOST,		1,	false,	COMMANDE(Stats)},
{"take",			STATE_PLAY,		false,	1,				20,	false,	COMMANDE(Take)},
{"tell",		STATE_PLAY,		false,	1,				2,	true,	COMMANDE(Tell)},
{"unban",		STATE_PLAY,		true,	GOD_ARCHANGEL,	1,	false,	COMMANDE(Unban)},
{"unforbid",	STATE_PLAY,		true,	GOD_HIGHGOD,	1,	false,	COMMANDE(Unforbid)},
{"version",		STATE_PLAY,		false,	1,				0,	false,	COMMANDE(Version)},
{"who",			STATE_PLAY,		false,	1,				1,	false,	COMMANDE(Who)},
{""}//last
};
