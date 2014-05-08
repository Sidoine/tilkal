#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <assert.h>
#include <signal.h>
#include <time.h>

#ifdef _WIN32
#	include <windows.h>
#	include <winsock.h>
#	include <process.h>
	typedef int socklen_t;
	typedef __int64 int64;
	typedef HANDLE MUTEX;
	inline void WaitMutex(MUTEX & Mutex)
	{
		WaitForSingleObject(Mutex,INFINITE);
	}
	inline void NewMutex(MUTEX & Mutex)
	{
		Mutex=CreateMutex(NULL,FALSE,NULL);
	}
	#define MSG_NOSIGNAL 0
#else
#	include <unistd.h>
#	include <sys/socket.h>
#	include <sys/stat.h>
#	include <dirent.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <netdb.h>
#	include <pthread.h>
#	define closesocket(x) close(x)
	typedef int SOCKET;
	typedef unsigned char BYTE;
	typedef long long int64;
	typedef pthread_mutex_t MUTEX;
	inline void WaitMutex(MUTEX & Mutex)
	{
		pthread_mutex_lock(&Mutex);
	}
	inline void ReleaseMutex(MUTEX & Mutex)
	{
		pthread_mutex_unlock(&Mutex);
	}
	inline void NewMutex(MUTEX & Mutex)
	{
		pthread_mutex_init(&Mutex,NULL);
	}	
#	define MAX_PATH 512
#endif

#define BUFSIZE 65536

#include "myarray.h"
#include "hache.h"


#ifdef _WIN32
#	define ADRESSEFONC(x) &x
#else
#	define ADRESSEFONC(x) &x
#endif

#define SPORT  5000
#define VERSION	"0.0.2"
#define	DEFAULT_PLAYER_NAME "guest"
#define	DEFAULT_OBJECT_NAME "something"
#define	DEFAULT_ROOM_NAME "The void."
#define	DEFAULT_ROOM_DESC "You are floating in the void.\r\n"

#define	MAX_ALT		1000
#define	MIN_ALT		-1000
#define	SAVE_TIME	60 /* seconds between saves */

#define MAX_OBJ_FLAGS	5
#define	MAX_DATA_PER_OBJ	5
#define LINE_LENGTH	70
#define	LINES_PER_BLOCK	20	// Number of line sent in every book page
#define MAX_BODYPARTS 16
#define BSIZE (12*1024)
#define TMAXNOMCOMMANDE 32
#define TMAXNAME 30
#define TMAXPLAYERNAME 15
#define TMAXOBJNAME 30
#define TMAXDOORNAME 20
#define	TMAXSKILLNAME 20
#define TMAXPASS 20
#define NMAXPARAMS 10

#define START_ROOM	0 /* Number of the room where players start. */
#define	MORT_HIGHEST	100 /* Highest mortal level. */
/* The following is the number of skill percent points corresponding to the highest mortal level.
   It means that if you have a skill at 65% and another at 51% then you'll have 116 points. */
#define	PERCENT_TO_HIGHEST	1000
/* Put the following to 1 if you wish your bots to increase their skills like players. */
#define	BOT_PROGRESS	0

#define NOTHING	-1

namespace Tilkal
{

	enum ERace //Bof...
	{
		RACE_HUMAN=0,
		RACE_ELF=1,
		RACE_DWARF=2,
		RACE_TROLL=3
	};
	#define	MAX_RACES		4

	enum ECarac
	{
		CARAC_STR=0,
		CARAC_WISDOM=1,
		CARAC_DEXT=2,
		CARAC_CONSTIT=3,
		CARAC_CHARISMA=4,
		CARAC_COURAGE=5,

		CARAC_SIGHT=6,
		CARAC_HEAR=7,
		CARAC_TOUCH=8,
		CARAC_TASTE=9,
		CARAC_SMELL=10
	};
#define	MAX_CARACS	11
}

//Certaines de ces définitions ne devraient pas être nécessaires...
#define	SKILL_UNDEFINED		-1
#define	SKILL_MOVE		0
#define	SKILL_HAND_FIGHT	1
#define	SKILL_SWORD_FIGHT	2
#define	SKILL_AXE_FIGHT		3
#define	SKILL_MACE_FIGHT	4
#define	SKILL_BLOCK			5
#define	MAX_SKILL_TYPES		6
#define TXT_SIZE 64
#define TXT_BLOB 16


#ifndef _DEFINITIONS
//Variables globales, à supprimer dans la version finale
extern char welcome_buf[BUFSIZ];
extern char motd_buf[BUFSIZ];
extern char death_buf[BUFSIZ];
extern char rules_start_buf[BUFSIZ];
extern char rules_admin_buf[BUFSIZ];
extern char rules_authors_buf[BUFSIZ];
extern char rules_immort_buf[BUFSIZ];

extern char god_title[TXT_BLOB][TXT_SIZE];
extern char god_title_abbrev[TXT_BLOB][TXT_SIZE];
extern char mort_title_abbrev[TXT_BLOB][TXT_SIZE];
extern char race_name[TXT_BLOB][TXT_SIZE];
extern char race_name_abbrev[TXT_BLOB][TXT_SIZE];
extern char race_desc[TXT_BLOB][TXT_SIZE];
extern char door_type_name[TXT_BLOB][TXT_SIZE];
extern unsigned short door_resist[TXT_BLOB];
#endif
