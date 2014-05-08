#include "tilkal.h"
#include "clients.h"
#include "objects.h"
#include "players.h"
#include "tools.h"
#include "doors.h"
#include "objcategs.h"
#include "wwwfiles.h"

using namespace Tilkal;
using namespace TilTools;
using namespace std;

bool manage_edit_text(char *text, char *dest, short int *size);

/*   extern __inline__ unsigned long long int rdtsc()
   {
   unsigned long long int x;
   __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
   return x;
   }

   static unsigned long long blob=0;*/

void CClient::Write(char *buffer)
{
	int n;

	if (Socket == -1)
		return;
	/*	if (blob==0)
		{
		unsigned long long a=rdtsc();
		sleep(5);
		unsigned long long b=rdtsc();
		blob=(b-a)/5;
		}*/

	//	unsigned long long a=rdtsc();
	/*	if (httpid)
			send(Socket,"<p>",3,MSG_NOSIGNAL);*/
	n = send(Socket, buffer, (int)strlen(buffer), MSG_NOSIGNAL);
	/*if (httpid)
		send(Socket,"</p><script>s();</script>",(int)strlen("</p><script>s();</script>"),MSG_NOSIGNAL);*/

	/*	unsigned long long b=rdtsc();
		printf("Durée du send : %d µs\n",(b-a)*1000000/blob);*/
	if (n < 0)
	{
		Socket = -1;
		char buf[300];
		sprintf(buf, "Client %d has lost connection.", Id);
		Client.report(buf, REPORT_CONNECTION_LOST);
		Client.Remove(Id);
	}
}

bool CClient::WriteBegin()
{
	if (buff.IsEmpty())
	{
		buff.Add("<script>");
		return true;
	}
	else
		return false;
}

void CClient::WriteTuDis(const char *text)
{
	Writer Start(this);
	buff.Add("YouSay('");
	buff.AddQuoted(text);
	buff.Add("');");
}

void CClient::WriteDialogue(const char *who, const char *text)
{
	Writer Start(this);
	buff.Add("Say('");
	buff.AddQuoted(who);
	buff.Add("','");
	buff.AddQuoted(text);
	buff.Add("');");
}

void CClient::WriteRoom(const char *name, const char * desc, int image, int num)
{
	Writer Start(this);
	buff.Add("Room('");
	buff.AddQuoted(name);
	buff.Add("','");
	buff.AddQuoted(desc);
	buff.Add("','");
	if (image != NOTHING)
		buff.AddQuoted(WWWFile[image]->Key);
	buff.Add("',");
	buff.Add(num);
	buff.Add(");");
}

void CClient::WriteExit(const char *text, int num)
{
	Writer Start(this);
	buff.Add("Exit('");
	buff.AddQuoted(text);
	buff.Add("',");
	buff.Add(num);
	buff.Add(");");
}

void CClient::WriteObjectId()
{
	Writer Start(this);
	buff.Add("YourId(");
	buff.Add(obj_num());
	buff.Add(");");
}

void CClient::WriteAddObject(int what, int who, int howmany)
{
	Writer Start(this);

	int where = Object[what].container;
	buff.Add("AddObj(");
	buff.Add(what);
	buff.Add(",'");
	buff.AddQuoted(Object[what].name);
	buff.Add("',");
	buff.Add(Object[what].image);
	buff.Add(",");
	buff.Add(Object[what].type);
	buff.Add(",");
	buff.Add(where);
	buff.Add(",'");
	buff.AddQuoted(Object[where].name);
	buff.Add("',");
	buff.Add(who);
	buff.Add(",'");
	if (who != NOTHING)
		buff.AddQuoted(Object[who].name);
	buff.Add("','");
	if (Object[what].where)
	{
		int jb = 1;
		int co = Object[where].categ;
		bool Premier = true;
		for (int j = 0; j < ObjCateg[co].nbodyparts; j++)
		{
			if (Object[what].where & jb)
			{
				if (Premier)
					Premier = false;
				else
					buff.Add(",");
				buff.AddQuoted(ObjCateg[co].bodypart[j].name);
			}
			jb <<= 1;
		}
	}
	buff.Add("',");
	buff.Add(howmany);
	buff.Add(",");
	buff.Add(Object[what].quantity);
	buff.Add(",");
	buff.Add(Object[what].held ? "true" : "false");
	buff.Add(");");
}

void CClient::WriteRemoveObject(int num)
{
	Writer Start(this);
	buff.Add("RemObj(");
	buff.Add(num);
	buff.Add(");");
}

void CClient::WriteChangeQuantity(int id)
{
	Writer Start(this);
	buff.Add("ChgQu(");
	buff.Add(id);
	buff.Add(",");
	buff.Add(Object[id].quantity);
	buff.Add(");");
}

void CClient::WriteEvent(const char *text)
{
	Writer Start(this);
	buff.Add("Event('");
	buff.AddQuoted(text);
	buff.Add("');");
}

void CClient::WriteError(const char *text)
{
	Writer Start(this);
	buff.Add("Error('");
	buff.AddQuoted(text);
	buff.Add("');");
}

void CClient::WriteFinish()
{
	buff.Add("</script>\n");
	Write(buff);
	buff.Clear();
}

int CClient::read_socket(char *buffer)
{
	int n;

	if (Socket == -1)
		return (-1);

	n = recv(Socket, buffer, BUFSIZE, 0);

	if (n < 0)
		Client.Remove(Id);
	return n;
}

void CClient::Destroy()
{
	if (Socket != -1)
		closesocket(Socket);
	Socket = -1;
	if (player != NOTHING)
	{
		sprintf(buffer, "[%s] disconnected.\r\n", obj().name);

		Player[player].client = NOTHING;//Avoid that Player.Remove call this function

		if (Player[player].is_registered == false)
			Player.Remove(player);
		else
			obj().remove_from_obj();

		Client.report(buffer, REPORT_DISCONNECT);//We do it before the socket is closed
		//because this function could send a message to it
	}

	//	cout <<"Client déconnecté"<<endl;
	player = NOTHING;
}


int CompareDefCommande(const void *a, const void *b)
{
	return strcmp((char*)a, (char*)b);
}

SDefCommande * CClient::ChercherPlusProche(char * Commande)
{
	int inf = -1;
	int sup = (int)nCommandes;
	int l = (int)strlen(Commande);
	int min, max;

	if (l == 0)
		return NULL;

	while (inf < sup - 1)
	{
		int mil = (inf + sup) / 2;
		int ret = strncasecmp(Commande, DefCommande[mil].Nom, l);
		if (ret < 0)
			sup = mil;
		else if (ret > 0)
			inf = mil;
		else
		{
			int cmd;
			int ok;//Quelle commande est de bon niveau et de bon nom
			int nok = 0;//Nombre de commandes du bon niveau et du bon nom

			min = mil;

			while (min > 0 && strncasecmp(Commande, DefCommande[min - 1].Nom, l) == 0)
				min--;

			max = mil;
			while (max < (int)nCommandes - 1 && strncasecmp(Commande, DefCommande[max + 1].Nom, l) == 0)
				max++;

			for (cmd = min; cmd <= max; cmd++)
			{
				if (can_use_command(cmd))
				{
					if ((int)strlen(DefCommande[cmd].Nom) == l)
					{
						ok = cmd;
						nok = 1;
						break;
					}
					else
					{
						ok = cmd;
						nok++;
					}
				}
			}

			if (nok > 1)
			{
				Writer Start(this);
				WriteEvent("Type the right command:\r\n");
				disp_help_intervalle(min, max, false);
				if (Player[player].god)
				{
					WriteEvent("-= Divine commands =-\r\n");
					disp_help_intervalle(min, max, true);
				}
				return NULL;
			}
			else if (nok == 0)
			{
				sprintf(buffer, "Unknown command '%s'\r\n", Commande);
				WriteError(buffer);
				return NULL;
			}

			return &DefCommande[ok];
		}
	}
	sprintf(buffer, "Unknown command '%s'\r\n", Commande);
	WriteError(buffer);
	return NULL;
}

bool CClient::ExecuterCommande(char * DebutLigne)
{
	if (DebutLigne == 0)
		return false;

	char * c;
	int nb_args, nMaxParams;
	char *arg[NMAXPARAMS];
	SDefCommande * Cmd;
	int m;
	m = obj_num();

	int e;
	int r = Object[m].container;

	if ((state == STATE_PLAY) && r != NOTHING && (e = Object[r].is_an_exit(DebutLigne)) != NOTHING)
	{
		Object[m].use_door(e);
		return true;
	}

	//On saute les espaces du début
	while ((BYTE)(*DebutLigne) && (BYTE)(*DebutLigne) <= ' ')
		DebutLigne++;

	//On cherche la commande
	c = DebutLigne;
	while ((BYTE)(*c) > ' ')
		c++;
	if (*c)
		*(c++) = 0;

	Cmd = (SDefCommande*)ChercherPlusProche(DebutLigne);
	if (!Cmd)
		return false;

	while ((BYTE)(*c) && (BYTE)(*c) <= ' ')
		c++;

	nb_args = 0;
	arg[0] = DebutLigne;
	nMaxParams = Cmd->nParams;
	if (Cmd->Texte)
		nMaxParams--;
	//On cherche les paramètres
	while ((*c) && (nb_args < nMaxParams))
	{
		if (*c == '"')
		{
			c++;
			arg[nb_args + 1] = c;
			while ((BYTE)(*c) != '"' && *c)
				c++;
		}
		else
		{
			arg[nb_args + 1] = c;
			while ((BYTE)(*c) > ' ')
				c++;
		}
		if (*c)
			*(c++) = 0;
		nb_args++;
		while ((BYTE)(*c) && (BYTE)(*c) <= ' ')
			c++;
	}
	if (*c && Cmd->Texte)
	{
		arg[nb_args + 1] = c;
		nb_args++;
	}
	(this->*Cmd->Fonction)(nb_args, arg);
	return true;
}


void CClient::WriteInventory(int container)
{
	for (int p = Object[container].in_object; p != NOTHING; p = Object[p].next_o)
	{
		WriteAddObject(p, NOTHING, Object[p].quantity);
	}
}

void CClient::WriteInit()
{
	Writer Start(this);
	WriteObjectId();
	send_desc();
	//Inventaire
	WriteInventory(obj_num());
}

void CClient::send_prompt()
{
	switch (state)
	{
	case STATE_LOGIN:
	{
		Writer Start(this);
		WriteEvent("[MAIN ENTRANCE]");
		WriteEvent("register : create your character");
		WriteEvent("login    : enter the mud");
		WriteEvent("quit     : return to the real world");
		break;
	}
	case STATE_REGISTER:
	{
		/*int n=obj_num();
		sprintf(buffer,"\r\n-------------------------------------------------------------");
		write_socket(buffer);
		sprintf(buffer,"\r\nChange these fields using: change <field> (value)\r\n\n");
		write_socket(buffer);
		sprintf(buffer,"<name> : %s   <pass>: %s\r\n",Object[n].name,Player[player].pass);
		write_socket(buffer);
		sprintf(buffer,"<race> : %s\r\n",race_name[Object[n].race]);
		write_socket(buffer);
		sprintf(buffer,"\r\nType 'finish' when you're satisfied. > ");
		write_socket(buffer);*/
		break;
	}
	case STATE_READ:
		//write_socket("*** Press ENTER to continue. Q to exit. ***\r\n");
		break;
	case STATE_EDIT_OBJ:
		//send_olc_edit_object();
		break;
	case STATE_WRITE_OBJ_NAME:
		//send_olc_edit_object_name();
		break;
	case STATE_WRITE_OBJ_DESC:
		//send_olc_edit_object_desc();
		break;
	case STATE_PLAY:
	{
		/*char *b=buffer,buf[50];
		int n=obj_num();
		if (Object[n].opponent!=NOTHING) // Fighting.
		{
		float nb,nb_max;
		int op=IDfromUID(Object[n].opponent);
		nb=Object[n].compute_health();
		nb_max=Object[n].compute_max_health();
		make_bar_from_values(buf,nb,nb_max,15);
		b+=sprintf(b,"\r\nYou [%s] - ",buf);
		nb=Object[op].compute_health();
		nb_max=Object[op].compute_max_health();
		make_bar_from_values(buf,nb,nb_max,15);
		b+=sprintf(b,"[%s] Opponent > ",buf);
		}
		else // Standard play.
		sprintf(buffer,"\r\n> ");
		WriteEvent(buffer);*/
		break;
	}
	}
}

void CClient::send_desc()
{
	int n, d, r, nb;

	n = obj_num();
	if (n == -1)
		return;
	r = Object[n].container;

	Writer Start(this);
	WriteRoom(Object[r].name, Object[r].desc, Object[r].image, r);

	nb = 0;
	d = Object[r].door;
	while (d != NOTHING)
	{
		nb++;
		if (Door[d].r1 == r)
			WriteExit(Door[d].name1, d);
		else
			WriteExit(Door[d].name2, d);
		d = Door[d].next_door_of_room(r);
	}

	d = Object[r].in_object;
	while (d != NOTHING)
	{
		if (d != n)
		{
			WriteAddObject(d, NOTHING, 0);
		}
		d = Object[d].next_o;
	}
}

void CClient::send_buffer_part()
{
	int nlines = 0;
	char *ch = reading;
	char buf[300];

	Writer Start(this);
	do
	{
		if (*ch == '\n')
		{
			memcpy(buf, reading, ch - reading + 1);
			buf[ch - reading + 1] = '\0';
			WriteEvent(buf);
			reading = ch + 1;
			nlines++;
		}
		ch++;
	} while (*ch && (nlines < LINES_PER_BLOCK));
	if (nlines < LINES_PER_BLOCK)
	{
		state = STATE_PLAY;
		reading = NULL;
	}
}

void CClient::read_book(char *text)
{
	state = STATE_READ;
	reading = text;
	send_buffer_part();
}

void CClient::send_refresh()
{
	//write_socket("\r\n");
	send_desc();
	send_prompt();
}


bool CClient::can_use_command(int cmd)
{
	int lvl = DefCommande[cmd].lvl;
	int god = DefCommande[cmd].god;
	int _state = DefCommande[cmd].state;

	if ((_state == STATE_ALL) || (state == _state))
	{
		if (Player[player].lvl >= lvl&& Player[player].god >= god)
			return true;
	}
	return false;
}

void CClient::disp_help()
{
	Writer Start(this);
	WriteEvent("<b>Available commands are:</b>");
	disp_help_intervalle(0, int(nCommandes) - 1, false);

	if (play().god)
	{
		WriteEvent("<b>Your divine commands are:</b>");
		disp_help_intervalle(0, (int)nCommandes - 1, true);
	}
}

void CClient::disp_help_intervalle(int min, int max, int god)
{
	int m, p;
	int nCmdParLigne = 10;
	int nAffichees = 0;

	m = obj_num();
	p = player;
	buff.Add("Event('<table><tr>");
	for (int i = min; i <= max; i++)
	{
		if (can_use_command(i) && (DefCommande[i].god == god))
		{
			buff.Add("<td>");
			buff.AddQuoted(DefCommande[i].Nom);
			buff.Add("</td>");
			if (god)
			{
				buff.Add("<td>");
				buff.Add(DefCommande[i].lvl);
				buff.Add("</td>");
			}

			if ((++nAffichees % nCmdParLigne) == 0) //(strlen(buffer)>LINE_LENGTH)
			{
				buff.Add("</tr><tr>");
			}
		}
	}
	buff.Add("</tr></table>');");
}

#define IAC 255 // interpret as command:
#define WONT 252 // I won't use option
#define WILL 251 // I will use option
#define TELOPT_ECHO 1 // echo

void CClient::echo_off()
{
	char off_string[] =
	{
		(char)IAC,
		(char)WILL,
		(char)TELOPT_ECHO,
		(char)0,
	};
	//write_socket(off_string);
}

void CClient::echo_on()
{
	char on_string[] =
	{
		(char)IAC,
		(char)WONT,
		(char)TELOPT_ECHO,
		(char)0
	};
	//write_socket(on_string);
}

bool manage_edit_text(char *text, char *dest, short int *size)
{
	/*if (*text=='@')
	  return true;
	  else if (*text=='%')
	  {
	  dest=(char *)realloc(dest,1);
	  strcpy(dest,"\0");
	  *size=(short int)strlen(dest);
	  }
	  else
	  {
	  dest=(char *)realloc(dest,strlen(dest)+
	  strlen(text)+3);
	  strcat(dest,text);
	  strcat(dest,"\r\n");
	  *size=(short int)strlen(dest);
	  }*/
	return false;
}

void CClient::send_olc_edit_object()
{
	/*char *b;

	int r=EditedObject;
	sprintf(buffer,"\r\n-------------------------------------------------------------");
	write_socket(buffer);
	sprintf(buffer,"\r\nChange these fields using: change <field> (value)\r\n"
	"Type 'finish' when it's over.\r\n\n");
	write_socket(buffer);
	b=buffer;
	b+=sprintf(b,"Object number [%d]    Type [ ",r);
	switch (Object[r].type)
	{
	case ObjType::Human:
	b+=sprintf(b,"PLAYER - be careful");
	break;
	case ObjType::Bot:
	b+=sprintf(b,"BOT");
	break;
	case ObjType::Obj:
	b+=sprintf(b,"OBJECT");
	break;
	case ObjType::Room:
	b+=sprintf(b,"ROOM");
	break;
	default:
	b+=sprintf(b,"?");
	}
	b+=sprintf(b," ]\r\n");
	write_socket(buffer);
	sprintf(buffer,"<name> %s\r\n",Object[r].name);
	write_socket(buffer);
	sprintf(buffer,"<desc>\r\n%s\r\n",Object[r].desc);
	write_socket(buffer);
	if (Object[r].type==ObjType::Room)
	{
	sprintf(buffer,"<alt> %d\r\n",Object[r].alt);
	write_socket(buffer);
	}
	sprintf(buffer,"\r\n> ");
	write_socket(buffer);*/
}

void CClient::send_olc_edit_object_name()
{
	/*sprintf(buffer,"\r\n-------------------------------------------------------------");
	write_socket(buffer);
	sprintf(buffer,"\r\nWrite down the new description. @ to finish\r\n\n");
	write_socket(buffer);
	sprintf(buffer,"<name> %s\r\n",Object[EditedObject].name);
	write_socket(buffer);
	sprintf(buffer,"\r\n> ");
	write_socket(buffer);*/
}

void CClient::send_olc_edit_object_desc()
{
	/*sprintf(buffer,"\r\n-------------------------------------------------------------");
	write_socket(buffer);
	sprintf(buffer,"\r\n%% on a new line to clear buffer, @ to finish\r\n\n");
	write_socket(buffer);
	sprintf(buffer,"<desc>\r\n%s\r\n",Object[EditedObject].desc);
	write_socket(buffer);
	sprintf(buffer,"\r\n> ");
	write_socket(buffer);*/
}

void CClient::Do()
{
	char buf[BSIZE];
	char *src, *dest;
	char * DebutLigne, *FinLigne;
	int nb, r, e;
	int m = obj_num();

	if ((nb = read_socket(buf)) > 0)
	{
		buf[nb] = 0;
		src = dest = buf;
		while (*src)
		{
			if ((BYTE)*src == 0xFF)
			{
				src++;
				if ((BYTE)*src == 0xFF)
					*(dest++) = (char)0xFF;
				else
					src += 2;
			}
			else
				*(dest++) = *(src++);
		}
		*dest = 0;

		//On interdit que le tampon fasse plus de 510 caractères
		//1 pour rajouter un \n, 1 pour le 0
		if (lTampon + nb >= 510)
		{
			nb = 510 - lTampon;
			buf[nb++] = '\n';
		}

		buf[nb] = 0;
		memcpy(&Tampon[lTampon], buf, nb + 1);
		DebutLigne = Tampon;
		FinLigne = Tampon;

		while (1)
		{
			while ((*FinLigne != '\r') && (*FinLigne != '\n') && (*FinLigne))
				FinLigne++;
			//Si ça se termine pas par un \r, la ligne n'est pas complète
			if (*FinLigne == 0)
				break;
			if (*FinLigne == '\r')
				*(FinLigne++) = 0;
			if (*FinLigne == '\n')
				*(FinLigne++) = 0;
			if (*DebutLigne != '!')
				strcpy(last_cmd, DebutLigne);

			switch (state) {
			case STATE_READ:
				if ((*DebutLigne == 'q') || (*DebutLigne == 'Q'))
				{
					state = STATE_PLAY;
					reading = NULL;
				}
				if (!*DebutLigne) // Pressed ENTER
					send_buffer_part();
				break;
			case STATE_WRITE_OBJ_NAME:
				r = EditedObject;
				if (*DebutLigne == '@')
					state = STATE_EDIT_OBJ;
				else
					strncpy(Object[r].name, DebutLigne, TMAXNAME);
				break;
			case STATE_WRITE_OBJ_DESC:
				r = EditedObject;
				if (manage_edit_text(DebutLigne, Object[r].desc,
					&Object[r].desc_size))
					state = STATE_EDIT_OBJ;
				break;
			default:
				if (!*DebutLigne)
					break;
				r = Object[m].container;
				if ((state == STATE_PLAY) && (e = Object[r].is_an_exit(DebutLigne)) != NOTHING)
					Object[m].use_door(e);
				else
					ExecuterCommande(DebutLigne);
				break;
			}
			DebutLigne = FinLigne;
		}

		if (FinLigne != DebutLigne)
		{
			//On déplace la dernière ligne (inachevée) en début de tampon
			if (DebutLigne != Tampon)
				memmove(Tampon, DebutLigne, FinLigne - DebutLigne + 1);
			//Le +1, c'est pour déplacer aussi le zéro terminal
			lTampon = (int)(FinLigne - DebutLigne);
		}
		else
		{
			//On vide le tampon
			lTampon = 0;
			Tampon[0] = 0;
			if (Socket != -1)
				send_prompt();
		}
	}
}
