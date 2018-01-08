#ifndef _BUILDDEFINES_H
#define _BUILDDEFINES_H

#include "Language Defines.h"

// Beta version
// #define	JA2BETAVERSION

// If we want to include the editor
//#define JA2EDITOR

// Do combinations

#ifdef JA2BETAVERSION
	#define SGP_DEBUG
	#define	FORCE_ASSERTS_ON
	#define SGP_VIDEO_DEBUGGING
#endif

//#define CRIPPLED_VERSION

// Huge speed and memory hog, but thorough -- will work with release builds.
// #define EXTREME_MEMORY_DEBUGGING 

#endif
