#ifdef PRECOMPILEDHEADERS
	#include "Strategic All.h"
	#include "HelpScreen.h"
	#include "Ja25Update.h"
	#include "Ja25 Strategic Ai.h"
	#include "Ja25_Tactical.h"
#endif

//** Prototypes ****************************************************************

extern UNDERGROUND_SECTORINFO* NewUndergroundNode( UINT8 ubSectorX, UINT8 ubSectorY, UINT8 ubSectorZ, BOOLEAN fCampaignSector );




//** Functions  ****************************************************************



void InitJa25StrategicAi()
{

	//Init the underground sectors ( add them to the list of under ground )
	InitJa25UnderGroundSectors();
}

void InitJa25UnderGroundSectors()
{
	UNDERGROUND_SECTORINFO *curr;

	//Under the mine ( i13
	curr = NewUndergroundNode( 13, 9, 1, TRUE );
	Assert( curr );

	//under the power generator j13
	curr = NewUndergroundNode( 13, 10, 1, TRUE );
	Assert( curr );

	//the tunnels	J14, k14
	curr = NewUndergroundNode( 14, 10, 1, TRUE );
	Assert( curr );
	curr = NewUndergroundNode( 14, 11, 1, TRUE );
	Assert( curr );

	//under the complex, K15, level 1 & 2
	curr = NewUndergroundNode( 15, 11, 1, TRUE );
	Assert( curr );
	curr = NewUndergroundNode( 15, 11, 2, TRUE );
	Assert( curr );

	//under the complex, 2nd part, L15, level 2 & 3
	curr = NewUndergroundNode( 15, 12, 2, TRUE );
	Assert( curr );
	curr = NewUndergroundNode( 15, 12, 3, TRUE );
	Assert( curr );

}


void SetNumberJa25EnemiesInUnderGroundSector( INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ, UINT8 ubNumAdmins, UINT8 ubNumTroops, UINT8 ubNumElites )
{
	UNDERGROUND_SECTORINFO *pSector=NULL;

	pSector = FindUnderGroundSector( sSectorX, sSectorY, bSectorZ );
	Assert( pSector );

	//Set the values
	pSector->ubNumAdmins = ubNumAdmins;
	pSector->ubNumTroops = ubNumTroops;
	pSector->ubNumElites = ubNumElites;
}


void InitNumberOfEnemiesInUnderGroundSectors( )
{
	UINT8	ubNumAdmins=0;
	UINT8	ubNumTroops=0;
	UINT8	ubNumElites=0;

	//Mine Sector Level 1
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_MEDIUM:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_HARD:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
	}
	SetNumberJa25EnemiesInSector( 13, 9, 1, ubNumAdmins, ubNumTroops, ubNumElites );


	//Power Generator Level 1
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_MEDIUM:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_HARD:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
	}
	SetNumberJa25EnemiesInSector( 13, 10, 1, ubNumAdmins, ubNumTroops, ubNumElites );


	//Tunnel First Part, Level 1
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_MEDIUM:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_HARD:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
	}
	SetNumberJa25EnemiesInSector( 14, 10, 1, ubNumAdmins, ubNumTroops, ubNumElites );


	//Tunnel Second Part, Level 1
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_MEDIUM:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_HARD:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 0 + Random( 0 );
			ubNumElites = 0 + Random( 0 );
			break;
	}
	SetNumberJa25EnemiesInSector( 14, 11, 1, ubNumAdmins, ubNumTroops, ubNumElites );



	//Complex, Level 1, K15
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 14 + Random( 6 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_MEDIUM:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 14 + Random( 4 );
			ubNumElites = 4 + Random( 3 );
			break;
		case DIF_LEVEL_HARD:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 15 + Random( 4 );
			ubNumElites = 6 + Random( 4 );
			break;
	}
	SetNumberJa25EnemiesInSector( 15, 11, 1, ubNumAdmins, ubNumTroops, ubNumElites );


	//Complex, Level 2, K15
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 10 + Random( 3 );
			ubNumElites = 0 + Random( 0 );
			break;
		case DIF_LEVEL_MEDIUM:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 15 + Random( 2 );
			ubNumElites = 5 + Random( 2 );
			break;
		case DIF_LEVEL_HARD:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 12 + Random( 3 );
			ubNumElites = 10 + Random( 2 );
			break;
	}
	SetNumberJa25EnemiesInSector( 15, 11, 2, ubNumAdmins, ubNumTroops, ubNumElites );


	//Complex, Level 2, L15
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 12 + Random( 2 );
			ubNumElites = 1 + Random( 2 );
			break;
		case DIF_LEVEL_MEDIUM:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 17 + Random( 2 );
			ubNumElites = 5 + Random( 2 );
			break;
		case DIF_LEVEL_HARD:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 10 + Random( 3 );
			ubNumElites = 14 + Random( 2 );
			break;
	}
	SetNumberJa25EnemiesInSector( 15, 12, 2, ubNumAdmins, ubNumTroops, ubNumElites );


	//Complex, Level 3, L15
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 26 + Random( 0 );
			ubNumElites = 2 + Random( 0 );
			break;
		case DIF_LEVEL_MEDIUM:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 20 + Random( 0 );
			ubNumElites = 8 + Random( 0 );
			break;
		case DIF_LEVEL_HARD:
			ubNumAdmins = 0 + Random( 0 );
			ubNumTroops = 13 + Random( 0 );
			ubNumElites = 15 + Random( 0 );
			break;
	}
	SetNumberJa25EnemiesInSector( 15, 12, 3, ubNumAdmins, ubNumTroops, ubNumElites );
}

