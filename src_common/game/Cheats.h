#ifndef _CHEATS__H_
#define _CHEATS__H_

#include "Language Defines.h"


extern	UINT8			gubCheatLevel;




// GLOBALS FOR CHEAT MODE......
#ifdef GERMAN
		#define						STARTING_CHEAT_LEVEL						0
#else
		#define						STARTING_CHEAT_LEVEL						0
#endif



#ifdef GERMAN

  // ATE: remove cheats unless we're doing a debug build
	#define						INFORMATION_CHEAT_LEVEL( )			( gubCheatLevel >= 5 )
	#define						CHEATER_CHEAT_LEVEL( )					( gubCheatLevel >= 6 )
	#define						DEBUG_CHEAT_LEVEL( )					  ( gubCheatLevel >= 7 )
	#define						RESET_CHEAT_LEVEL( )						( gubCheatLevel = 0 )

#else

  // ATE: remove cheats unless we're doing a debug build
	#define						INFORMATION_CHEAT_LEVEL( )			( gubCheatLevel >= 5 )
	#define						CHEATER_CHEAT_LEVEL( )					( gubCheatLevel >= 6 )
	#define						DEBUG_CHEAT_LEVEL( )					  ( gubCheatLevel >= 7 )
	#define						RESET_CHEAT_LEVEL( )						( gubCheatLevel = 0 )
#endif


#endif
