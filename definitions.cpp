#define _DEFINITIONS
#include "tilkal.h"
#include "all.h"

using namespace Tilkal;

/* These are the god names. Make sure that there are at least GOD_HIGHEST
   names here, as given in tilkal.h */
char god_title[TXT_BLOB][TXT_SIZE] = {
  "Ghost","Spirit","Messiah","Angel","Archangel","Lesser God","God",
  "High God","Immortal"
};

/* These are the god abbreviated names. Make sure that there are at least
   GOD_HIGHEST names here, as given in tilkal.h */
char god_title_abbrev[TXT_BLOB][TXT_SIZE] = {
  "GHOST","SPIRT","MESSI","ANGEL","ARCHA","L-GOD"," GOD ",
  "H-GOD"," IMM "
};

char welcome_buf[BUFSIZ];
char motd_buf[BUFSIZ];
char death_buf[BUFSIZ];
char rules_start_buf[BUFSIZ];
char rules_admin_buf[BUFSIZ];
char rules_authors_buf[BUFSIZ];
char rules_immort_buf[BUFSIZ];

/* These are the races names. Make sure that there are at least
   MAX_RACES names here, as given in tilkal.h */
char race_name[TXT_BLOB][TXT_SIZE] = {
  "Human","Elf","Dwarf","Troll"
};

/* These are descriptions for the races. Make sure that there are at least
   MAX_RACES lines here, as given in tilkal.h */
char race_desc[TXT_BLOB][TXT_SIZE] = {
  "A rather average race.",
  "Skillful and agile race.",
  "Very resistant race.",
  "Strong and violent race."
};

/* These are the abbreviated races names. Make sure that there are at least
   MAX_RACES names here, as given in tilkal.h */
char race_name_abbrev[TXT_BLOB][TXT_SIZE] = {
  "Human","Elf","Dwarf","Troll"
};

/* Door type names. Make sure that there are at least MAX_DOOR_TYPES names
   here, as given in tilkal.h */
char door_type_name[TXT_BLOB][TXT_SIZE] =
{
  "passage","hole","wooden door","iron door","iron grid","gate"
};

/* Door resistance per type. Make sure that there are at least MAX_DOOR_TYPES
   numbers here, as given in tilkal.h */
unsigned short door_resist[TXT_BLOB] =
{
  0,0,18,95,35,330
};
