#include "tilkal.h"
#include "objects.h"
#include "doors.h"
#include "tools.h"

using namespace Tilkal;
using namespace TilTools;

void CObject::send_text_to_room_from(char *text, int object)
{
	int o = in_object;
	while (o != NOTHING)
	{
		if (o != object)
			Object[o].Listen(text, object);
		else
			Object[o].ListenYourself(text);
		o = Object[o].next_o;
	}
}

void CObject::send_text_to_room(char *text)
{
	int o = in_object;
	while (o != NOTHING)
	{
		Object[o].send_text(text);
		o = Object[o].next_o;
	}
}

int CObject::is_an_exit(char *cmd)
{
	int i, d;
	int dd, size;
	char Small[10], *s;

	dd = NOTHING;
	size = 1000;
	d = door;
	while (d != NOTHING)
	{
		if (Door[d].r1 == Id)
		{
			s = &Small[0];
			*s = Door[d].name1[0];
			s++;
			for (i = 1; i < (int)strlen(Door[d].name1) - 1; i++)
			{
				if ((Door[d].name1[i] == '-') && (Door[d].name1[i + 1] != '-'))
				{
					*s = Door[d].name1[i + 1];
					s++;
				}
			}

			*s = 0;

			if (!strncasecmp(Door[d].name1, cmd, strlen(cmd)) ||
				!strncasecmp(Small, cmd, strlen(cmd)))
			{
				if ((int)strlen(Door[d].name1) < size)
				{
					dd = d;
					size = (int)strlen(Door[d].name1);
				}
			}
		}
		else
		{
			s = &Small[0];
			*s = Door[d].name2[0];
			s++;
			for (i = 1; i < (int)strlen(Door[d].name2) - 1; i++)
			{
				if ((Door[d].name2[i] == '-') && (Door[d].name2[i + 1] != '-'))
				{
					*s = Door[d].name2[i + 1];
					s++;
				}
			}

			*s = 0;

			if (!strncasecmp(Door[d].name2, cmd, strlen(cmd)) ||
				!strncasecmp(Small, cmd, strlen(cmd)))
			{
				if ((int)strlen(Door[d].name2) < size)
				{
					dd = d;
					size = (int)strlen(Door[d].name2);
				}
			}
		}
		d = Door[d].next_door_of_room(Id);
	}
	return dd;
}

int CObject::is_an_obj_in_room(char *name)
{
	int o;

	o = in_object;
	while (o != NOTHING)
	{
		if (strstrnocase(Object[o].name, name))
			return o;
		o = Object[o].next_o;
	}
	return NOTHING;
}

void CObject::create_room(const char *_name, const char *_desc)
{
	set_name(_name);
	set_desc(_desc);
	type = ObjType::Room;
	max_weight = max_containweight = 10000;
	door = NOTHING;
	in_object = NOTHING;
	alt = 0;
}

