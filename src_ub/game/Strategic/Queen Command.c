//Queen Command.c

#ifdef PRECOMPILEDHEADERS
	#include "Strategic All.h"
	#include "Ja25 Strategic Ai.h"
	#include "Ja25_Tactical.h"
#else
	#include "Queen Command.h"
	#include "Strategic Event Handler.h"
	#include "Overhead Types.h"
	#include "strategicmap.h"
	#include "Soldier Init List.h"
	#include "debug.h"
	#include "Random.h"
	#include "Strategic Movement.h"
	#include "Overhead.h"
	#include "Strategic Pathing.h"
	#include "strategic.h"
	#include "message.h"
	#include "Map Edgepoints.h"
	#include "PreBattle Interface.h"
	#include "strategic status.h"
	#include "squads.h"
	#include "assignments.h"
	#include "items.h"
	#include "tactical save.h"
	#include "Soldier Ani.h"
	#include "Strategic AI.h"
#endif



void HandleBloodCatDeaths( SECTORINFO *pSector );


//The sector information required for the strategic AI.  Contains the number of enemy troops,
//as well as intentions, etc.
SECTORINFO SectorInfo[256];
UNDERGROUND_SECTORINFO *gpUndergroundSectorInfoHead = NULL;
extern UNDERGROUND_SECTORINFO* gpUndergroundSectorInfoTail;
BOOLEAN gfPendingEnemies = FALSE;
extern void BuildUndergroundSectorInfoList();

extern void EndCreatureQuest();

extern GARRISON_GROUP *gGarrisonGroup;
extern INT32 giGarrisonArraySize;

INT16		gsGridNoForMapEdgePointInfo=-1;

/* Ja25:  no surrendering 
INT16 gsInterrogationGridNo[3] = { 7756, 7757, 7758 };
*/


void ValidateEnemiesHaveWeapons()
{
}

//Counts enemies and crepitus, but not bloodcats.
UINT8 NumHostilesInSector( INT16 sSectorX, INT16 sSectorY, INT16 sSectorZ )
{
	UINT8 ubNumHostiles = 0;

	Assert( sSectorX >= 1 && sSectorX <= 16 );
	Assert( sSectorY >= 1 && sSectorY <= 16 );
	Assert( sSectorZ >= 0 && sSectorZ <= 3 );

	if( sSectorZ )
	{
		UNDERGROUND_SECTORINFO *pSector;
		pSector = FindUnderGroundSector( sSectorX, sSectorY, (UINT8)sSectorZ );
		if( pSector )
		{
			ubNumHostiles = (UINT8)(pSector->ubNumAdmins + pSector->ubNumTroops + pSector->ubNumElites + pSector->ubNumCreatures);
		}
	}
	else
	{
		SECTORINFO *pSector;
		GROUP *pGroup;

		//Count stationary hostiles
		pSector = &SectorInfo[ SECTOR( sSectorX, sSectorY ) ];
		ubNumHostiles = (UINT8)(pSector->ubNumAdmins + pSector->ubNumTroops + pSector->ubNumElites + pSector->ubNumCreatures);
		
		//Count mobile enemies
		pGroup = gpGroupList;
		while( pGroup )
		{
			if( !pGroup->fPlayer && !pGroup->fVehicle && pGroup->ubSectorX == sSectorX && pGroup->ubSectorY == sSectorY )
			{
				ubNumHostiles += pGroup->ubGroupSize;
			}
			pGroup = pGroup->next;
		}
	}

	return ubNumHostiles;
}

UINT8 NumEnemiesInAnySector( INT16 sSectorX, INT16 sSectorY, INT16 sSectorZ )
{
	UINT8 ubNumEnemies = 0;
	GROUP *pGroup;

	Assert( sSectorX >= 1 && sSectorX <= 16 );
	Assert( sSectorY >= 1 && sSectorY <= 16 );
	Assert( sSectorZ >= 0 && sSectorZ <= 3 );

	if( sSectorZ )
	{
		UNDERGROUND_SECTORINFO *pSector;
		pSector = FindUnderGroundSector( sSectorX, sSectorY, (UINT8)sSectorZ );
		if( pSector )
		{
			ubNumEnemies = (UINT8)(pSector->ubNumAdmins + pSector->ubNumTroops + pSector->ubNumElites);
		}
	}
	else
	{
		SECTORINFO *pSector;

		//Count stationary enemies
		pSector = &SectorInfo[ SECTOR( sSectorX, sSectorY ) ];
		ubNumEnemies = (UINT8)(pSector->ubNumAdmins + pSector->ubNumTroops + pSector->ubNumElites);
	}

	//Count mobile enemies
	pGroup = gpGroupList;
	while( pGroup )
	{
		if( !pGroup->fPlayer && !pGroup->fVehicle && pGroup->ubSectorX == sSectorX && pGroup->ubSectorY == sSectorY && pGroup->ubSectorZ == sSectorZ )
		{
			ubNumEnemies += pGroup->ubGroupSize;
		}
		pGroup = pGroup->next;
	}

	return ubNumEnemies;
}

UINT8 NumEnemiesInSector( INT16 sSectorX, INT16 sSectorY )
{
	SECTORINFO *pSector;
	GROUP *pGroup;
	UINT8 ubNumTroops;
	Assert( sSectorX >= 1 && sSectorX <= 16 );
	Assert( sSectorY >= 1 && sSectorY <= 16 );
	pSector = &SectorInfo[ SECTOR( sSectorX, sSectorY ) ];
	ubNumTroops = (UINT8)(pSector->ubNumAdmins + pSector->ubNumTroops + pSector->ubNumElites);

	pGroup = gpGroupList;
	while( pGroup )
	{
		if( !pGroup->fPlayer && !pGroup->fVehicle && pGroup->ubSectorX == sSectorX && pGroup->ubSectorY == sSectorY )
		{
			ubNumTroops += pGroup->ubGroupSize;
		}
		pGroup = pGroup->next;
	}
	return ubNumTroops;
}

UINT8 NumStationaryEnemiesInSector( INT16 sSectorX, INT16 sSectorY )
{
	SECTORINFO *pSector;
	Assert( sSectorX >= 1 && sSectorX <= 16 );
	Assert( sSectorY >= 1 && sSectorY <= 16 );
	pSector = &SectorInfo[ SECTOR( sSectorX, sSectorY ) ];

	if( pSector->ubGarrisonID == NO_GARRISON )
	{ //If no garrison, no stationary.
		return( 0 );
	}

	Assert( 0 );
	return( 0 );
/*
Ja25:  no garrison, should never get here
	// don't count roadblocks as stationary garrison, we want to see how many enemies are in them, not question marks
	if ( gGarrisonGroup[ pSector->ubGarrisonID ].ubComposition == ROADBLOCK )
	{
		// pretend they're not stationary
		return( 0 );
	}

	return (UINT8)(pSector->ubNumAdmins + pSector->ubNumTroops + pSector->ubNumElites);
*/
}

UINT8 NumMobileEnemiesInSector( INT16 sSectorX, INT16 sSectorY, UINT8 ubSectorZ )
{
	GROUP *pGroup;
//	SECTORINFO *pSector;
	UINT8 ubNumTroops;
	Assert( sSectorX >= 1 && sSectorX <= 16 );
	Assert( sSectorY >= 1 && sSectorY <= 16 );

	ubNumTroops = 0;
	pGroup = gpGroupList;
	while( pGroup )
	{
		if( !pGroup->fPlayer && !pGroup->fVehicle && pGroup->ubSectorX == sSectorX && pGroup->ubSectorY == sSectorY  && pGroup->ubSectorZ == ubSectorZ )
		{
			ubNumTroops += pGroup->ubGroupSize;
		}
		pGroup = pGroup->next;
	}

	/*
	pSector = &SectorInfo[ SECTOR( sSectorX, sSectorY ) ];
	if( pSector->ubGarrisonID == ROADBLOCK )
	{ //consider these troops as mobile troops even though they are in a garrison
		ubNumTroops += (UINT8)(pSector->ubNumAdmins + pSector->ubNumTroops + pSector->ubNumElites);
	}
*/
	return ubNumTroops;
}

void GetNumberOfMobileEnemiesInSector( INT16 sSectorX, INT16 sSectorY, UINT8 *pubNumAdmins, UINT8 *pubNumTroops, UINT8 *pubNumElites )
{
	GROUP *pGroup;
	SECTORINFO *pSector;
	Assert( sSectorX >= 1 && sSectorX <= 16 );
	Assert( sSectorY >= 1 && sSectorY <= 16 );

	//Now count the number of mobile groups in the sector.
	*pubNumTroops = *pubNumElites = *pubNumAdmins = 0;
	pGroup = gpGroupList;
	while( pGroup )
	{
		if( !pGroup->fPlayer && !pGroup->fVehicle && pGroup->ubSectorX == sSectorX && pGroup->ubSectorY == sSectorY )
		{
			*pubNumTroops += pGroup->pEnemyGroup->ubNumTroops;
			*pubNumElites += pGroup->pEnemyGroup->ubNumElites;
			*pubNumAdmins += pGroup->pEnemyGroup->ubNumAdmins;
		}
		pGroup = pGroup->next;
	}

	pSector = &SectorInfo[ SECTOR( sSectorX, sSectorY ) ];
	if( pSector->ubGarrisonID == ROADBLOCK )
	{ //consider these troops as mobile troops even though they are in a garrison
		*pubNumAdmins += pSector->ubNumAdmins;
		*pubNumTroops += pSector->ubNumTroops;
		*pubNumElites += pSector->ubNumElites;
	}

}

void GetNumberOfStationaryEnemiesInSector( INT16 sSectorX, INT16 sSectorY, UINT8 *pubNumAdmins, UINT8 *pubNumTroops, UINT8 *pubNumElites )
{
	SECTORINFO *pSector;
	Assert( sSectorX >= 1 && sSectorX <= 16 );
	Assert( sSectorY >= 1 && sSectorY <= 16 );
	pSector = &SectorInfo[ SECTOR( sSectorX, sSectorY ) ];

	//grab the number of each type in the stationary sector
	*pubNumAdmins = pSector->ubNumAdmins;
	*pubNumTroops = pSector->ubNumTroops;
	*pubNumElites = pSector->ubNumElites;
}

void GetNumberOfEnemiesInSector( INT16 sSectorX, INT16 sSectorY, UINT8 *pubNumAdmins, UINT8 *pubNumTroops, UINT8 *pubNumElites )
{
	UINT8 ubNumAdmins, ubNumTroops, ubNumElites;
	
	GetNumberOfStationaryEnemiesInSector( sSectorX, sSectorY, pubNumAdmins, pubNumTroops, pubNumElites );

	GetNumberOfMobileEnemiesInSector( sSectorX, sSectorY, &ubNumAdmins, &ubNumTroops, &ubNumElites );

	*pubNumAdmins += ubNumAdmins;
	*pubNumTroops += ubNumTroops;
	*pubNumElites += ubNumElites;
}

void EndTacticalBattleForEnemy()
{
	GROUP *pGroup;
//	INT32 iNumMilitia = 0, iNumEnemies = 0;

	//Clear enemies in battle for all stationary groups in the sector.
	if( gbWorldSectorZ > 0 )
	{
		UNDERGROUND_SECTORINFO *pSector;
		pSector = FindUnderGroundSector( gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
		pSector->ubAdminsInBattle = 0;
		pSector->ubTroopsInBattle = 0;
		pSector->ubElitesInBattle = 0;
	}
	else if( !gbWorldSectorZ )
	{
		SECTORINFO *pSector;
		pSector = &SectorInfo[ SECTOR( gWorldSectorX, gWorldSectorY ) ];
		//grab the number of each type in the stationary sector
		pSector->ubAdminsInBattle = 0;
		pSector->ubTroopsInBattle = 0;
		pSector->ubElitesInBattle = 0;
	}
	else	// negative
		return;

	//Clear this value so that profiled enemies can be added into battles in the future.
	gfProfiledEnemyAdded = FALSE;

	//Clear enemies in battle for all mobile groups in the sector.
	pGroup = gpGroupList;
	while( pGroup )
	{
		if( !pGroup->fPlayer && !pGroup->fVehicle && pGroup->ubSectorX == gWorldSectorX && pGroup->ubSectorY == gWorldSectorY )
		{
			pGroup->pEnemyGroup->ubTroopsInBattle = 0;
			pGroup->pEnemyGroup->ubElitesInBattle = 0;
			pGroup->pEnemyGroup->ubAdminsInBattle = 0;
		}
		pGroup = pGroup->next;
	}

/*
Ja25:	no loyalty, or militia

	//Check to see if any of our mercs have abandoned the militia during a battle.  This is cause for a rather
	//severe loyalty blow.
	for( i = gTacticalStatus.Team[ MILITIA_TEAM ].bFirstID; i <= gTacticalStatus.Team[ MILITIA_TEAM ].bLastID; i++ )
	{
		if( MercPtrs[ i ]->bActive && MercPtrs[ i ]->bInSector && MercPtrs[ i ]->bLife >= OKLIFE )
		{ //found one live militia, so look for any enemies/creatures.
			// NOTE: this is relying on ENEMY_TEAM being immediately followed by CREATURE_TEAM
			for( i = gTacticalStatus.Team[ ENEMY_TEAM ].bFirstID; i <= gTacticalStatus.Team[ CREATURE_TEAM ].bLastID; i++ )
			{
				if( MercPtrs[ i ]->bActive && MercPtrs[ i ]->bInSector && MercPtrs[ i ]->bLife >= OKLIFE )
				{ //confirmed at least one enemy here, so do the loyalty penalty.
					HandleGlobalLoyaltyEvent( GLOBAL_LOYALTY_ABANDON_MILITIA, gWorldSectorX, gWorldSectorY, 0 );
					break;
				}
			}
			break;
		}
	}
*/
}

UINT8 NumFreeEnemySlots()
{
	UINT8 ubNumFreeSlots = 0;
	INT32 i;
	SOLDIERTYPE *pSoldier;
	//Count the number of free enemy slots.  It is possible to have multiple groups exceed the maximum.
	for( i = gTacticalStatus.Team[ ENEMY_TEAM ].bFirstID; i <= gTacticalStatus.Team[ ENEMY_TEAM].bLastID; i++ )
	{
		pSoldier = &Menptr[ i ];
		if( !pSoldier->bActive )
			ubNumFreeSlots++;
	}
	return ubNumFreeSlots;
}

//Called when entering a sector so the campaign AI can automatically insert the
//correct number of troops of each type based on the current number in the sector
//in global focus (gWorldSectorX/Y)
BOOLEAN PrepareEnemyForSectorBattle()
{
	SECTORINFO *pSector;
	GROUP *pGroup;
	SOLDIERTYPE *pSoldier;
	UINT8 ubNumAdmins, ubNumTroops, ubNumElites;
	UINT8 ubTotalAdmins, ubTotalElites, ubTotalTroops;
	UINT8 ubStationaryEnemies;
	INT32 i, num;
	INT16 sNumSlots;

	//if the player has WON in this sector before, we must destroy the ENEMIES soldier init list links
	HandleRemovingEnemySoldierInitLinksIfPlayerEverWonInSector();

	gfPendingEnemies = FALSE;

	if( gbWorldSectorZ > 0 )
	{
		PrepareEnemyForUndergroundBattle();

		if( gpBattleGroup )
		{
			HandleArrivalOfReinforcements( gpBattleGroup );

			gpBattleGroup = NULL;
		}
		return( TRUE );
	}

	if( gpBattleGroup && !gpBattleGroup->fPlayer )
	{ //The enemy has instigated the battle which means they are the ones entering the conflict.
		//The player was actually in the sector first, and the enemy doesn't use reinforced placements
		HandleArrivalOfReinforcements( gpBattleGroup );
		//It is possible that other enemy groups have also arrived.  Add them in the same manner.
		pGroup = gpGroupList;
		while( pGroup )
		{
			if( pGroup != gpBattleGroup && !pGroup->fPlayer && !pGroup->fVehicle &&
				  pGroup->ubSectorX == gpBattleGroup->ubSectorX &&
					pGroup->ubSectorY == gpBattleGroup->ubSectorY &&
					!pGroup->pEnemyGroup->ubAdminsInBattle &&
					!pGroup->pEnemyGroup->ubTroopsInBattle &&
					!pGroup->pEnemyGroup->ubElitesInBattle )
			{
				HandleArrivalOfReinforcements( gpBattleGroup );
			}
			pGroup = pGroup->next;
		}
		ValidateEnemiesHaveWeapons();
		return ( ( BOOLEAN) ( gpBattleGroup->ubGroupSize > 0 ) );
	}

	if( !gbWorldSectorZ )
	{
		if( NumEnemiesInSector( gWorldSectorX, gWorldSectorY ) > 32 )
		{
			gfPendingEnemies = TRUE;
		}
	}

	pSector = &SectorInfo[ SECTOR( gWorldSectorX, gWorldSectorY ) ];
	if( pSector->uiFlags & SF_USE_MAP_SETTINGS )
	{ //count the number of enemy placements in a map and use those
		SOLDIERINITNODE *curr;
		curr = gSoldierInitHead;
		ubTotalAdmins = ubTotalTroops = ubTotalElites = 0;
		while( curr )
		{
			if( curr->pBasicPlacement->bTeam == ENEMY_TEAM )
			{
				switch( curr->pBasicPlacement->ubSoldierClass )
				{
					case SOLDIER_CLASS_ADMINISTRATOR:		ubTotalAdmins++;	break;
					case SOLDIER_CLASS_ARMY:						ubTotalTroops++;	break;
					case SOLDIER_CLASS_ELITE:						ubTotalElites++;	break;
				}	
			}
			curr = curr->next;
		}
		pSector->ubNumAdmins = ubTotalAdmins;
		pSector->ubNumTroops = ubTotalTroops;
		pSector->ubNumElites = ubTotalElites;
		pSector->ubAdminsInBattle = 0;
		pSector->ubTroopsInBattle = 0;
		pSector->ubElitesInBattle = 0;
	}
	else
	{
		ubTotalAdmins = (UINT8)(pSector->ubNumAdmins - pSector->ubAdminsInBattle);
		ubTotalTroops = (UINT8)(pSector->ubNumTroops - pSector->ubTroopsInBattle);
		ubTotalElites = (UINT8)(pSector->ubNumElites - pSector->ubElitesInBattle);
	}
	ubStationaryEnemies = (UINT8)(ubTotalAdmins + ubTotalTroops + ubTotalElites);

	if( ubTotalAdmins + ubTotalTroops + ubTotalElites > 32 )
	{

		ubTotalAdmins = min( 32, ubTotalAdmins );
		ubTotalTroops = min( 32-ubTotalAdmins, ubTotalTroops );
		ubTotalElites = min( 32-ubTotalAdmins+ubTotalTroops, ubTotalElites );
	}

	pSector->ubAdminsInBattle += ubTotalAdmins;
	pSector->ubTroopsInBattle += ubTotalTroops;
	pSector->ubElitesInBattle += ubTotalElites;


	//Search for movement groups that happen to be in the sector.
	sNumSlots = NumFreeEnemySlots();
	//Test:  All slots should be free at this point!
	if( sNumSlots != gTacticalStatus.Team[ENEMY_TEAM].bLastID - gTacticalStatus.Team[ENEMY_TEAM].bFirstID + 1 )
	{
	}
	//Subtract the total number of stationary enemies from the available slots, as stationary forces take
	//precendence in combat.  The mobile forces that could also be in the same sector are considered later if
	//all the slots fill up.
	sNumSlots -= ubTotalAdmins + ubTotalTroops + ubTotalElites;
	//Now, process all of the groups and search for both enemy and player groups in the sector.
	//For enemy groups, we fill up the slots until we have none left or all of the groups have been 
	//processed.
	pGroup = gpGroupList;
	while( pGroup && sNumSlots )
	{
		if( !pGroup->fPlayer && !pGroup->fVehicle &&  
				 pGroup->ubSectorX == gWorldSectorX && pGroup->ubSectorY == gWorldSectorY && !gbWorldSectorZ )
		{ //Process enemy group in sector.
			if( sNumSlots > 0 )
			{
				ubNumAdmins = (UINT8)(pGroup->pEnemyGroup->ubNumAdmins - pGroup->pEnemyGroup->ubAdminsInBattle);
				sNumSlots -= ubNumAdmins;
				if( sNumSlots < 0 )
				{ //adjust the value to zero
					ubNumAdmins += sNumSlots;
					sNumSlots = 0;
					gfPendingEnemies = TRUE;
				}
				pGroup->pEnemyGroup->ubAdminsInBattle += ubNumAdmins;
				ubTotalAdmins += ubNumAdmins;
			}
			if( sNumSlots > 0 )
			{ //Add regular army forces.
				ubNumTroops = (UINT8)(pGroup->pEnemyGroup->ubNumTroops - pGroup->pEnemyGroup->ubTroopsInBattle);
				sNumSlots -= ubNumTroops;
				if( sNumSlots < 0 )
				{ //adjust the value to zero
					ubNumTroops += sNumSlots; 
					sNumSlots = 0;
					gfPendingEnemies = TRUE;
				}
				pGroup->pEnemyGroup->ubTroopsInBattle += ubNumTroops;
				ubTotalTroops += ubNumTroops;
			}
			if( sNumSlots > 0 )
			{ //Add elite troops
				ubNumElites = (UINT8)(pGroup->pEnemyGroup->ubNumElites - pGroup->pEnemyGroup->ubElitesInBattle);
				sNumSlots -= ubNumElites;
				if( sNumSlots < 0 )
				{ //adjust the value to zero
					ubNumElites += sNumSlots;
					sNumSlots = 0;
					gfPendingEnemies = TRUE;
				}
				pGroup->pEnemyGroup->ubElitesInBattle += ubNumElites;
				ubTotalElites += ubNumElites;
			}
			//NOTE:
			//no provisions for profile troop leader or retreat groups yet.
		}
		if( pGroup->fPlayer && !pGroup->fVehicle && !pGroup->fBetweenSectors &&
				pGroup->ubSectorX == gWorldSectorX && pGroup->ubSectorY == gWorldSectorY && !gbWorldSectorZ )
		{ //TEMP:  The player path needs to get destroyed, otherwise, it'll be impossible to move the
			//			 group after the battle is resolved.

			// no one in the group any more continue loop
			if( pGroup->pPlayerList == NULL )
			{
				pGroup = pGroup->next;
				continue;
			}

			// clear the movt for this grunt and his buddies
			RemoveGroupWaypoints( pGroup->ubGroupID );
		}
		pGroup = pGroup->next;
	}

	//if there are no troops in the current groups, then we're done.
	if( !ubTotalAdmins && !ubTotalTroops && !ubTotalElites )
	{
		return FALSE;
	}

	AddSoldierInitListEnemyDefenceSoldiers( ubTotalAdmins, ubTotalTroops, ubTotalElites );


	//Now, we have to go through all of the enemies in the new map, and assign their respective groups if
	//in a mobile group, but only for the ones that were assigned from the 
	sNumSlots = 32 - ubStationaryEnemies;

	pGroup = gpGroupList;
	while( pGroup && sNumSlots )
	{
		i = gTacticalStatus.Team[ ENEMY_TEAM ].bFirstID; 
		pSoldier = &Menptr[ i ];
		if( !pGroup->fPlayer && !pGroup->fVehicle && pGroup->ubSectorX == gWorldSectorX && pGroup->ubSectorY == gWorldSectorY && !gbWorldSectorZ )
		{
			num = pGroup->ubGroupSize;
			ubNumAdmins = pGroup->pEnemyGroup->ubAdminsInBattle;
			ubNumTroops = pGroup->pEnemyGroup->ubTroopsInBattle;
			ubNumElites = pGroup->pEnemyGroup->ubElitesInBattle;
			while( num && sNumSlots && i <= gTacticalStatus.Team[ ENEMY_TEAM ].bLastID )
			{
				while( !pSoldier->bActive || pSoldier->ubGroupID )
				{
					pSoldier = &Menptr[ ++i ];
					if( i > gTacticalStatus.Team[ ENEMY_TEAM ].bLastID )
					{
						AssertMsg( 0, "Failed to assign battle counters for enemies properly. Please send save. KM:0." );
					}
				}
				switch( pSoldier->ubSoldierClass )
				{
					case SOLDIER_CLASS_ADMINISTRATOR:
						if( ubNumAdmins )
						{
							num--;
							sNumSlots--;
							ubNumAdmins--;
							pSoldier->ubGroupID = pGroup->ubGroupID;
						}
						break;
					case SOLDIER_CLASS_ARMY:
						if( ubNumTroops )
						{
							num--;
							sNumSlots--;
							ubNumTroops--;
							pSoldier->ubGroupID = pGroup->ubGroupID;
						}
						break;
					case SOLDIER_CLASS_ELITE:
						if( ubNumElites )
						{
							num--;
							sNumSlots--;
							ubNumElites--;
							pSoldier->ubGroupID = pGroup->ubGroupID;
						}
						break;
				}
				pSoldier = &Menptr[ ++i ];
			}
		}
		pGroup = pGroup->next;
	}

	ValidateEnemiesHaveWeapons();

	return TRUE;
}

BOOLEAN PrepareEnemyForUndergroundBattle()
{
	UNDERGROUND_SECTORINFO *pUnderground;
	UINT8 ubTotalAdmins, ubTotalTroops, ubTotalElites;
	pUnderground = gpUndergroundSectorInfoHead;
	while( pUnderground )
	{
		if( pUnderground->ubSectorX == gWorldSectorX && 
			  pUnderground->ubSectorY == gWorldSectorY &&
				pUnderground->ubSectorZ == gbWorldSectorZ )
		{ //This is the sector we are going to be fighting in.
			if( pUnderground->ubNumAdmins || pUnderground->ubNumTroops || pUnderground->ubNumElites )
			{
				ubTotalAdmins = (UINT8)(pUnderground->ubNumAdmins - pUnderground->ubAdminsInBattle);
				ubTotalTroops = (UINT8)(pUnderground->ubNumTroops - pUnderground->ubTroopsInBattle);
				ubTotalElites = (UINT8)(pUnderground->ubNumElites - pUnderground->ubElitesInBattle);
				pUnderground->ubAdminsInBattle += ubTotalAdmins;
				pUnderground->ubTroopsInBattle += ubTotalTroops;
				pUnderground->ubElitesInBattle += ubTotalElites;
				AddSoldierInitListEnemyDefenceSoldiers( pUnderground->ubNumAdmins, pUnderground->ubNumTroops, pUnderground->ubNumElites );
				ValidateEnemiesHaveWeapons();
			}
			return ( ( BOOLEAN) ( pUnderground->ubNumAdmins + pUnderground->ubNumTroops + pUnderground->ubNumElites > 0 ) );
		}
		pUnderground = pUnderground->next;
	}

	// underground sector not found in list
	Assert( FALSE );
	return FALSE;
}

//The queen AI layer must process the event by subtracting forces, etc.
void ProcessQueenCmdImplicationsOfDeath( SOLDIERTYPE *pSoldier )
{
	INT32 iNumEnemiesInSector;
	SECTORINFO *pSector;

	EvaluateDeathEffectsToSoldierInitList( pSoldier );

	switch( pSoldier->ubProfile )
	{
/*
JA25:  No mike or iggy
		case MIKE:
		case IGGY:
			if( pSoldier->ubProfile == IGGY && !gubFact[ FACT_IGGY_AVAILABLE_TO_ARMY ] )
			{ //Iggy is on our team!
				break;
			}
*/
		case MORRIS:

			if( !pSoldier->bSectorZ )
			{
				pSector = &SectorInfo[ SECTOR( pSoldier->sSectorX, pSoldier->sSectorY ) ];
				if( pSector->ubNumElites )
				{
					pSector->ubNumElites--;
				}
				if( pSector->ubElitesInBattle )
				{
					pSector->ubElitesInBattle--;
				}
			}
			 else
			{
				UNDERGROUND_SECTORINFO *pUnderground;
				pUnderground = FindUnderGroundSector( (UINT8)pSoldier->sSectorX, (UINT8)pSoldier->sSectorY, (UINT8)pSoldier->bSectorZ );
				Assert( pUnderground );
				if( pUnderground->ubNumElites )
				{
					pUnderground->ubNumElites--;
				}
				if( pUnderground->ubElitesInBattle )
				{
					pUnderground->ubElitesInBattle--;
				}
			}
			break;
	}

	if( pSoldier->bNeutral || pSoldier->bTeam != ENEMY_TEAM && pSoldier->bTeam != CREATURE_TEAM )
		return;

	//we are recording an enemy death
	if( pSoldier->ubGroupID )
	{ //The enemy was in a mobile group
		GROUP *pGroup;
		pGroup = GetGroup( pSoldier->ubGroupID );
		if( !pGroup )
		{
			return;
		}
		if( pGroup->fPlayer )
		{
			return;
		}
		switch( pSoldier->ubSoldierClass )
		{
			case SOLDIER_CLASS_ELITE:
				if( pGroup->pEnemyGroup->ubNumElites )
				{
					pGroup->pEnemyGroup->ubNumElites--;
				}
				if( pGroup->pEnemyGroup->ubElitesInBattle )
				{
					pGroup->pEnemyGroup->ubElitesInBattle--;
				}
				break;
			case SOLDIER_CLASS_ARMY:
				if( pGroup->pEnemyGroup->ubNumTroops )
				{
					pGroup->pEnemyGroup->ubNumTroops--;
				}
				if( pGroup->pEnemyGroup->ubTroopsInBattle )
				{
					pGroup->pEnemyGroup->ubTroopsInBattle--;
				}
				break;
			case SOLDIER_CLASS_ADMINISTRATOR:
				if( pGroup->pEnemyGroup->ubNumAdmins )
				{
					pGroup->pEnemyGroup->ubNumAdmins--;
				}
				if( pGroup->pEnemyGroup->ubAdminsInBattle )
				{
					pGroup->pEnemyGroup->ubAdminsInBattle--;
				}
				break;
		}
		if( pGroup->ubGroupSize )
			pGroup->ubGroupSize--;
		RecalculateGroupWeight( pGroup );
		if( !pGroup->ubGroupSize )
		{
			RemovePGroup( pGroup );
		}
	}
	else
	{ //The enemy was in a stationary defence group
		if( !gbWorldSectorZ || IsAutoResolveActive() )
		{ //ground level (SECTORINFO)
			SECTORINFO *pSector;
			
			if( !IsAutoResolveActive() )
			{
				pSector = &SectorInfo[ SECTOR( pSoldier->sSectorX, pSoldier->sSectorY ) ];
			}
			else
			{
				pSector = &SectorInfo[ GetAutoResolveSectorID() ];
			}


			switch( pSoldier->ubSoldierClass )
			{
				case SOLDIER_CLASS_ADMINISTRATOR:
					if( pSector->ubNumAdmins )
					{
						pSector->ubNumAdmins--;
					}
					if( pSector->ubAdminsInBattle )
					{
						pSector->ubAdminsInBattle--;
					}
					break;
				case SOLDIER_CLASS_ARMY:
					if( pSector->ubNumTroops )
					{
						pSector->ubNumTroops--;
					}
					if( pSector->ubTroopsInBattle )
					{
						pSector->ubTroopsInBattle--;
					}
					break;
				case SOLDIER_CLASS_ELITE:
					if( pSector->ubNumElites )
					{
						pSector->ubNumElites--;
					}
					if( pSector->ubElitesInBattle )
					{
						pSector->ubElitesInBattle--;
					}
					break;
				case SOLDIER_CLASS_CREATURE:
					if( pSoldier->ubBodyType != BLOODCAT )
					{
						if( pSector->ubNumCreatures )
						{
							pSector->ubNumCreatures--;
						}
						if( pSector->ubCreaturesInBattle )
						{
							pSector->ubCreaturesInBattle--;
						}
					}
					else
					{
						if( pSector->bBloodCats )
						{
							pSector->bBloodCats--;

							//handle anything important when bloodcats die
							HandleBloodCatDeaths( pSector );
						}
					}

					break;
			}
			RecalculateSectorWeight( (UINT8)SECTOR( pSoldier->sSectorX, pSoldier->sSectorY ) );
		}
		else
		{ //basement level (UNDERGROUND_SECTORINFO)
			UNDERGROUND_SECTORINFO *pSector = FindUnderGroundSector( gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
			if( pSector )
			{
				switch( pSoldier->ubSoldierClass )
				{
					case SOLDIER_CLASS_ADMINISTRATOR:
						if( pSector->ubNumAdmins )
						{
							pSector->ubNumAdmins--;
						}
						if( pSector->ubAdminsInBattle )
						{
							pSector->ubAdminsInBattle--;
						}
						break;
					case SOLDIER_CLASS_ARMY:
						if( pSector->ubNumTroops )
						{
							pSector->ubNumTroops--;
						}
						if( pSector->ubTroopsInBattle )
						{
							pSector->ubTroopsInBattle--;
						}
						break;
					case SOLDIER_CLASS_ELITE:
						if( pSector->ubNumElites )
						{
							pSector->ubNumElites--;
						}
						if( pSector->ubElitesInBattle )
						{
							pSector->ubElitesInBattle--;
						}
						break;
				}
			}
		}
	}

	
	if( !pSoldier->bSectorZ )
	{
		pSector = &SectorInfo[ SECTOR( pSoldier->sSectorX, pSoldier->sSectorY ) ];
		iNumEnemiesInSector = NumEnemiesInSector( pSoldier->sSectorX, pSoldier->sSectorY );
		if( iNumEnemiesInSector )
		{
			if( pSector->bLastKnownEnemies >= 0 )
			{
				pSector->bLastKnownEnemies = (INT8)iNumEnemiesInSector;
			}
		}
		else
		{
			pSector->bLastKnownEnemies = 0;
		}
	}
}

//Rarely, there will be more enemies than supported by the engine.  In this case, these
//soldier's are waiting for a slot to be free so that they can enter the battle.  This
//essentially allows for an infinite number of troops, though only 32 at a time can fight.
//This is also called whenever an enemy group's reinforcements arrive because the code is
//identical, though it is highly likely that they will all be successfully added on the first call.
void AddPossiblePendingEnemiesToBattle()
{
	UINT8 ubSlots, ubNumAvailable;
	UINT8 ubNumElites, ubNumTroops, ubNumAdmins;
	GROUP *pGroup;

  if( !gfPendingEnemies )
	{ //Optimization.  No point in checking if we know that there aren't any more enemies that can
		//be added to this battle.  This changes whenever a new enemy group arrives at the scene.
		return;
	}
	ubSlots = NumFreeEnemySlots();
	if( !ubSlots )
	{ //no available slots to add enemies to.  Try again later...
		return;
	}
	pGroup = gpGroupList;
	while( pGroup && ubSlots )
	{
		if( !pGroup->fPlayer && !pGroup->fVehicle && pGroup->ubSectorX == gWorldSectorX && pGroup->ubSectorY == gWorldSectorY && !gbWorldSectorZ )
		{ //This enemy group is currently in the sector.
			ubNumElites = ubNumTroops = ubNumAdmins = 0;
			ubNumAvailable = pGroup->ubGroupSize - pGroup->pEnemyGroup->ubElitesInBattle - pGroup->pEnemyGroup->ubTroopsInBattle - pGroup->pEnemyGroup->ubAdminsInBattle;
			while( ubNumAvailable && ubSlots )
			{ //This group has enemies waiting for a chance to enter the battle.
				if( pGroup->pEnemyGroup->ubTroopsInBattle < pGroup->pEnemyGroup->ubNumTroops )
				{ //Add a regular troop.
					pGroup->pEnemyGroup->ubTroopsInBattle++;
					ubNumAvailable--;
					ubSlots--;
					ubNumTroops++;
				}
				else if( pGroup->pEnemyGroup->ubElitesInBattle < pGroup->pEnemyGroup->ubNumElites )
				{ //Add an elite troop
					pGroup->pEnemyGroup->ubElitesInBattle++;
					ubNumAvailable--;
					ubSlots--;
					ubNumElites++;
				}
				else if( pGroup->pEnemyGroup->ubAdminsInBattle < pGroup->pEnemyGroup->ubNumAdmins )
				{ //Add an elite troop
					pGroup->pEnemyGroup->ubAdminsInBattle++;
					ubNumAvailable--;
					ubSlots--;
					ubNumAdmins++;
				}
				else
				{
					AssertMsg( 0, "AddPossiblePendingEnemiesToBattle():  Logic Error -- by Kris" );
				}
			}
			if( ubNumAdmins || ubNumTroops || ubNumElites )
			{ //This group has contributed forces, then add them now, because different
				//groups appear on different sides of the map.
				UINT8 ubStrategicInsertionCode=0;
				//First, determine which entrypoint to use, based on the travel direction of the group.
				if( pGroup->ubPrevX && pGroup->ubPrevY )
				{
					if( pGroup->ubSectorX < pGroup->ubPrevX )
						ubStrategicInsertionCode = INSERTION_CODE_EAST;
					else if( pGroup->ubSectorX > pGroup->ubPrevX )
						ubStrategicInsertionCode = INSERTION_CODE_WEST;
					else if( pGroup->ubSectorY < pGroup->ubPrevY )
						ubStrategicInsertionCode = INSERTION_CODE_SOUTH;
					else if( pGroup->ubSectorY > pGroup->ubPrevY )
						ubStrategicInsertionCode = INSERTION_CODE_NORTH;
					else if( gsGridNoForMapEdgePointInfo != -1 )
					{
						//Ja25: it doesnt matter the entry point at this point, it will become GRIDNO at a later point
						ubStrategicInsertionCode = INSERTION_CODE_NORTH;
					}
				}
				else if( pGroup->ubNextX && pGroup->ubNextY )
				{
					if( pGroup->ubSectorX < pGroup->ubNextX )
						ubStrategicInsertionCode = INSERTION_CODE_EAST;
					else if( pGroup->ubSectorX > pGroup->ubNextX )
						ubStrategicInsertionCode = INSERTION_CODE_WEST;
					else if( pGroup->ubSectorY < pGroup->ubNextY )
						ubStrategicInsertionCode = INSERTION_CODE_SOUTH;
					else if( pGroup->ubSectorY > pGroup->ubNextY )
						ubStrategicInsertionCode = INSERTION_CODE_NORTH;
				}
				//Add the number of each type of troop and place them in the appropriate positions
				AddEnemiesToBattle( pGroup, ubStrategicInsertionCode, ubNumAdmins, ubNumTroops, ubNumElites, FALSE );
			}
		}
		pGroup = pGroup->next;
	}
	if( ubSlots )
	{ //After going through the process, we have finished with some free slots and no more enemies to add.
    //So, we can turn off the flag, as this check is no longer needed.
		gfPendingEnemies = FALSE;
	}
}

void NotifyPlayersOfNewEnemies()
{
	INT32 iSoldiers, iChosenSoldier, i;
	SOLDIERTYPE *pSoldier;
	BOOLEAN fIgnoreBreath = FALSE;

	iSoldiers = 0;
	for( i = gTacticalStatus.Team[ OUR_TEAM ].bFirstID; i <= gTacticalStatus.Team[ OUR_TEAM ].bLastID; i++ )
	{ //find a merc that is aware.
		pSoldier = MercPtrs[ i ];
		if( pSoldier->bInSector && pSoldier->bActive && pSoldier->bLife >= OKLIFE && pSoldier->bBreath >= OKBREATH )
		{
			iSoldiers++;
		}
	}
	if( !iSoldiers )
	{ // look for an out of breath merc.
		fIgnoreBreath = TRUE;

		for( i = gTacticalStatus.Team[ OUR_TEAM ].bFirstID; i <= gTacticalStatus.Team[ OUR_TEAM ].bLastID; i++ )
		{ //find a merc that is aware.
			pSoldier = MercPtrs[ i ];
			if( pSoldier->bInSector && pSoldier->bActive && pSoldier->bLife >= OKLIFE )
			{
				iSoldiers++;
			}
		}
	}
	if( iSoldiers )
	{
		iChosenSoldier = Random( iSoldiers );
		for( i = gTacticalStatus.Team[ OUR_TEAM ].bFirstID; i <= gTacticalStatus.Team[ OUR_TEAM ].bLastID; i++ )
		{ //find a merc that is aware.
			pSoldier = MercPtrs[ i ];
			if( pSoldier->bInSector && pSoldier->bActive && pSoldier->bLife >= OKLIFE &&
				( ( pSoldier->bBreath >= OKBREATH ) || fIgnoreBreath ) )
			{
				if( !iChosenSoldier )
				{
					// ATE: This is to allow special handling of initial heli drop
					if ( !DidGameJustStart() )
					{
						TacticalCharacterDialogueWithSpecialEvent( pSoldier, QUOTE_ENEMY_PRESENCE, 0, 0, 0 );
					}
					return;
				}
				iChosenSoldier--;
			}
		}
	}
	else
	{ //There is either nobody here or our mercs can't talk

	}
}

void AddEnemiesToBattle( GROUP *pGroup, UINT8 ubStrategicInsertionCode, UINT8 ubNumAdmins, UINT8 ubNumTroops, UINT8 ubNumElites, BOOLEAN fMagicallyAppeared )
{
	SOLDIERTYPE *pSoldier;
	MAPEDGEPOINTINFO MapEdgepointInfo;
	UINT8 ubCurrSlot;
	UINT8 ubTotalSoldiers;
	UINT8 bDesiredDirection=0;
	UINT8	ubCnt;

	switch( ubStrategicInsertionCode )
	{
		case INSERTION_CODE_NORTH:	bDesiredDirection = SOUTHEAST;										break;
		case INSERTION_CODE_EAST:		bDesiredDirection = SOUTHWEST;										break;
		case INSERTION_CODE_SOUTH:	bDesiredDirection = NORTHWEST;										break;
		case INSERTION_CODE_WEST:		bDesiredDirection = NORTHEAST;										break;
		case INSERTION_CODE_GRIDNO:	bDesiredDirection = NORTHEAST;										break;
		default:  AssertMsg( 0, "Illegal direction passed to AddEnemiesToBattle()" );	break;
	}

	if( fMagicallyAppeared )
	{ //update the strategic counters 
		if( !gbWorldSectorZ )
		{
			SECTORINFO *pSector = &SectorInfo[ SECTOR( gWorldSectorX, gWorldSectorY ) ];
			pSector->ubNumAdmins			+= ubNumAdmins;
			pSector->ubAdminsInBattle	+= ubNumAdmins;
			pSector->ubNumTroops			+= ubNumTroops;
			pSector->ubTroopsInBattle	+= ubNumTroops;
			pSector->ubNumElites			+= ubNumElites;
			pSector->ubElitesInBattle	+= ubNumElites;
		}
		else
		{
			UNDERGROUND_SECTORINFO *pSector = FindUnderGroundSector( gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
			if( pSector )
			{
				pSector->ubNumAdmins			+= ubNumAdmins;
				pSector->ubAdminsInBattle	+= ubNumAdmins;
				pSector->ubNumTroops			+= ubNumTroops;
				pSector->ubTroopsInBattle	+= ubNumTroops;
				pSector->ubNumElites			+= ubNumElites;
				pSector->ubElitesInBattle	+= ubNumElites;
			}
		}
		//Because the enemies magically appeared, have one of our soldiers say something...
		NotifyPlayersOfNewEnemies();
	}

	ubTotalSoldiers = ubNumAdmins + ubNumTroops + ubNumElites;

	if( gsGridNoForMapEdgePointInfo != -1 )
	{
		ubStrategicInsertionCode = INSERTION_CODE_GRIDNO;
	}

	if( ubStrategicInsertionCode == INSERTION_CODE_GRIDNO )
	{
		if( gsGridNoForMapEdgePointInfo == -1 )
		{
			Assert( 0 );
			gsGridNoForMapEdgePointInfo=0;
		}

		for( ubCnt=0; ubCnt<32;ubCnt++)
		{
			MapEdgepointInfo.sGridNo[ ubCnt ] = gsGridNoForMapEdgePointInfo;
		}

		MapEdgepointInfo.ubNumPoints = 32;
		MapEdgepointInfo.ubStrategicInsertionCode = INSERTION_CODE_GRIDNO;
	}
	else
	{
		ChooseMapEdgepoints( &MapEdgepointInfo, ubStrategicInsertionCode, (UINT8)(ubNumAdmins+ubNumElites+ubNumTroops) );
	}

	ubCurrSlot = 0;
	while( ubTotalSoldiers )
	{
		if( ubNumElites && Random( ubTotalSoldiers ) < ubNumElites )
		{
			ubNumElites--;
			ubTotalSoldiers--;
			pSoldier = TacticalCreateEliteEnemy();
			if( pGroup )
			{
				pSoldier->ubGroupID = pGroup->ubGroupID;
			}

			pSoldier->ubInsertionDirection = bDesiredDirection;
			//Setup the position
			if( ubCurrSlot < MapEdgepointInfo.ubNumPoints )
			{ //using an edgepoint
				pSoldier->ubStrategicInsertionCode = INSERTION_CODE_GRIDNO;
				pSoldier->usStrategicInsertionData = MapEdgepointInfo.sGridNo[ ubCurrSlot++ ];
			}
			else 
			{ //no edgepoints left, so put him at the entrypoint.
				pSoldier->ubStrategicInsertionCode = ubStrategicInsertionCode;
			}
			UpdateMercInSector( pSoldier, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
		}
		else if( ubNumTroops && (UINT8)Random( ubTotalSoldiers ) < (UINT8)(ubNumElites + ubNumTroops) )
		{
			ubNumTroops--;
			ubTotalSoldiers--;
			pSoldier = TacticalCreateArmyTroop();
			if( pGroup )
			{
				pSoldier->ubGroupID = pGroup->ubGroupID;
			}

			pSoldier->ubInsertionDirection = bDesiredDirection;
			//Setup the position
			if( ubCurrSlot < MapEdgepointInfo.ubNumPoints )
			{ //using an edgepoint
				pSoldier->ubStrategicInsertionCode = INSERTION_CODE_GRIDNO;
				pSoldier->usStrategicInsertionData = MapEdgepointInfo.sGridNo[ ubCurrSlot++ ];
			}
			else 
			{ //no edgepoints left, so put him at the entrypoint.
				pSoldier->ubStrategicInsertionCode = ubStrategicInsertionCode;
			}
			UpdateMercInSector( pSoldier, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
		}
		else if( ubNumAdmins && (UINT8)Random( ubTotalSoldiers ) < (UINT8)(ubNumElites + ubNumTroops + ubNumAdmins) )
		{
			ubNumAdmins--;
			ubTotalSoldiers--;
			pSoldier = TacticalCreateAdministrator();
			if( pGroup )
			{
				pSoldier->ubGroupID = pGroup->ubGroupID;
			}

			pSoldier->ubInsertionDirection = bDesiredDirection;
			//Setup the position
			if( ubCurrSlot < MapEdgepointInfo.ubNumPoints )
			{ //using an edgepoint
				pSoldier->ubStrategicInsertionCode = INSERTION_CODE_GRIDNO;
				pSoldier->usStrategicInsertionData = MapEdgepointInfo.sGridNo[ ubCurrSlot++ ];
			}
			else 
			{ //no edgepoints left, so put him at the entrypoint.
				pSoldier->ubStrategicInsertionCode = ubStrategicInsertionCode;
			}
			UpdateMercInSector( pSoldier, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
		}
	}

	gsGridNoForMapEdgePointInfo = -1;
}




BOOLEAN SaveUnderGroundSectorInfoToSaveGame( HWFILE hFile )
{
	UINT32	uiNumBytesWritten;
	UINT32	uiNumOfRecords=0;
	UNDERGROUND_SECTORINFO *TempNode = gpUndergroundSectorInfoHead;

	//Loop through all the nodes to count how many there are
	while( TempNode )
	{
		uiNumOfRecords++;
		TempNode = TempNode->next;
	}


	//Write how many nodes there are
	FileWrite( hFile, &uiNumOfRecords, sizeof( UINT32 ), &uiNumBytesWritten );
	if( uiNumBytesWritten != sizeof( UINT32 ) )
	{
		return(FALSE);
	}

	TempNode = gpUndergroundSectorInfoHead;

	//Go through each node and save it.
	while( TempNode )
	{
		FileWrite( hFile, TempNode, sizeof( UNDERGROUND_SECTORINFO ), &uiNumBytesWritten );
		if( uiNumBytesWritten != sizeof( UNDERGROUND_SECTORINFO ) )
		{
			return(FALSE);
		}
		
		TempNode = TempNode->next;
	}

	return( TRUE );
}

BOOLEAN LoadUnderGroundSectorInfoFromSavedGame( HWFILE hFile )
{
	UINT32	uiNumBytesRead;
	UINT32	uiNumOfRecords=0;
	UINT32	cnt=0;
	UNDERGROUND_SECTORINFO *TempNode = NULL;
	UNDERGROUND_SECTORINFO *TempSpot = NULL;

	//Clear the current LL
	TrashUndergroundSectorInfo();

	//Read in the number of nodes stored
	FileRead( hFile, &uiNumOfRecords, sizeof( UINT32 ), &uiNumBytesRead );
	if( uiNumBytesRead != sizeof( UINT32 ) )
	{
		return(FALSE);
	}

	for( cnt = 0; cnt< uiNumOfRecords; cnt ++)
	{

		//Malloc space for the new node
		TempNode = MemAlloc( sizeof( UNDERGROUND_SECTORINFO ) );
		if( TempNode == NULL )
			return( FALSE );


		//read in the new node
		FileRead( hFile, TempNode, sizeof( UNDERGROUND_SECTORINFO ), &uiNumBytesRead );
		if( uiNumBytesRead != sizeof( UNDERGROUND_SECTORINFO ) )
		{
			return(FALSE);
		}

		//If its the first time in, assign the node to the list
		if( cnt == 0 )
		{
			gpUndergroundSectorInfoHead = TempNode;
			TempSpot = gpUndergroundSectorInfoHead;
			TempSpot->next = NULL;
		}
		else
		{
			//assign the new node to the LL
			TempSpot->next = TempNode;

			//advance to the next node
			TempSpot = TempSpot->next;
			TempSpot->next = NULL;
			gpUndergroundSectorInfoTail = TempSpot;
		}

	}


	return( TRUE );
}


UNDERGROUND_SECTORINFO* FindUnderGroundSector( INT16 sMapX, INT16 sMapY, UINT8 bMapZ )
{
	UNDERGROUND_SECTORINFO *pUnderground;
	pUnderground = gpUndergroundSectorInfoHead;

	//Loop through all the underground sectors looking for specified sector
	while( pUnderground )
	{
		//If the sector is the right one
		if( pUnderground->ubSectorX == sMapX && 
			  pUnderground->ubSectorY == sMapY &&
				pUnderground->ubSectorZ == bMapZ )
		{ 
			return( pUnderground );
		}
		pUnderground = pUnderground->next;
	}


	return( NULL );
}



/*

void BeginCaptureSquence( )
{
	if( !( gStrategicStatus.uiFlags & STRATEGIC_PLAYER_CAPTURED_FOR_RESCUE ) || !( gStrategicStatus.uiFlags & STRATEGIC_PLAYER_CAPTURED_FOR_ESCAPE ) )
	{
		gStrategicStatus.ubNumCapturedForRescue = 0;
	}
}

void EndCaptureSequence( )
{

	// Set flag...
	if( !( gStrategicStatus.uiFlags & STRATEGIC_PLAYER_CAPTURED_FOR_RESCUE ) || !(gStrategicStatus.uiFlags & STRATEGIC_PLAYER_CAPTURED_FOR_ESCAPE) )
	{
		// CJC Dec 1 2002: fixing multiple captures:
		//gStrategicStatus.uiFlags |= STRATEGIC_PLAYER_CAPTURED_FOR_RESCUE;

		if ( gubQuest[ QUEST_HELD_IN_ALMA ] == QUESTNOTSTARTED )
		{
			// CJC Dec 1 2002: fixing multiple captures:
			gStrategicStatus.uiFlags |= STRATEGIC_PLAYER_CAPTURED_FOR_RESCUE;
			StartQuest( QUEST_HELD_IN_ALMA, gWorldSectorX, gWorldSectorY );
		}
		// CJC Dec 1 2002: fixing multiple captures:
		//else if ( gubQuest[ QUEST_HELD_IN_ALMA ] == QUESTDONE )
		else if ( gubQuest[ QUEST_HELD_IN_ALMA ] == QUESTDONE && gubQuest[ QUEST_INTERROGATION ] == QUESTNOTSTARTED )
		{
			StartQuest( QUEST_INTERROGATION, gWorldSectorX, gWorldSectorY );
			// CJC Dec 1 2002: fixing multiple captures:
			gStrategicStatus.uiFlags |= STRATEGIC_PLAYER_CAPTURED_FOR_ESCAPE;

			// OK! - Schedule Meanwhile now!
			{
				MEANWHILE_DEFINITION MeanwhileDef;

				MeanwhileDef.sSectorX = 7;
				MeanwhileDef.sSectorY = 14;
				MeanwhileDef.ubNPCNumber = QUEEN;
				MeanwhileDef.usTriggerEvent = 0;
				MeanwhileDef.ubMeanwhileID = INTERROGATION;

				ScheduleMeanwhileEvent( &MeanwhileDef, 10 );
			}
		}
		// CJC Dec 1 2002: fixing multiple captures
		else 
		{
			// !?!? set both flags
			gStrategicStatus.uiFlags |= STRATEGIC_PLAYER_CAPTURED_FOR_RESCUE;
			gStrategicStatus.uiFlags |= STRATEGIC_PLAYER_CAPTURED_FOR_ESCAPE;
		}
	}

}
*/
/*
Ja25:	No surrendering

void EnemyCapturesPlayerSoldier( SOLDIERTYPE *pSoldier )
{
	INT32					i;
	WORLDITEM			WorldItem;
  BOOLEAN       fMadeCorpse;
  INT32         iNumEnemiesInSector;


	static INT16 sAlmaCaptureGridNos[] = { 9208, 9688, 9215 };
	static INT16 sAlmaCaptureItemsGridNo[] = { 12246, 12406, 13046 };

	static INT16 sInterrogationItemGridNo[] = { 12089, 12089, 12089 };
	

  // ATE: Check first if ! in player captured sequence already
	// CJC Dec 1 2002: fixing multiple captures
	if ( ( gStrategicStatus.uiFlags & STRATEGIC_PLAYER_CAPTURED_FOR_RESCUE ) && (gStrategicStatus.uiFlags & STRATEGIC_PLAYER_CAPTURED_FOR_ESCAPE) )
  {
    return;
  }

  // ATE: If maximum prisoners captured, return!
  if ( gStrategicStatus.ubNumCapturedForRescue > 3 )
  {
    return;
  }


  // If this is an EPC , just kill them...
  if ( AM_AN_EPC( pSoldier ) )
  {
	  pSoldier->bLife = 0;
    HandleSoldierDeath( pSoldier, &fMadeCorpse );
    return;
  }

  if ( pSoldier->uiStatusFlags & SOLDIER_VEHICLE )
  {
    return;
  }

  // ATE: Patch fix If in a vehicle, remove from vehicle...
  TakeSoldierOutOfVehicle( pSoldier ); 


  // Are there anemies in ALMA? ( I13 )
  iNumEnemiesInSector = NumEnemiesInSector( 13, 9 );

  // IF there are no enemies, and we need to do alma, skip!
  if ( gubQuest[ QUEST_HELD_IN_ALMA ] == QUESTNOTSTARTED && iNumEnemiesInSector == 0 )
  {
	  InternalStartQuest( QUEST_HELD_IN_ALMA, gWorldSectorX, gWorldSectorY, FALSE );
	  InternalEndQuest( QUEST_HELD_IN_ALMA, gWorldSectorX, gWorldSectorY, FALSE );
  }

	HandleMoraleEvent( pSoldier, MORALE_MERC_CAPTURED, pSoldier->sSectorX, pSoldier->sSectorY, pSoldier->bSectorZ );

	// Change to POW....
	//-add him to a POW assignment/group
	if( ( pSoldier->bAssignment != ASSIGNMENT_POW )  ) 
	{
		SetTimeOfAssignmentChangeForMerc( pSoldier );
	}

	ChangeSoldiersAssignment( pSoldier, ASSIGNMENT_POW );
	// ATE: Make them neutral!
	if ( gubQuest[ QUEST_HELD_IN_ALMA ] == QUESTNOTSTARTED )
	{
		pSoldier->bNeutral = TRUE;
	}
	
	RemoveCharacterFromSquads( pSoldier );

	// Is this the first one..?
	if ( gubQuest[ QUEST_HELD_IN_ALMA ] == QUESTNOTSTARTED )
	{
		//-teleport him to NE Alma sector (not Tixa as originally planned)
		pSoldier->sSectorX = 13;
		pSoldier->sSectorY = 9;
		pSoldier->bSectorZ = 0;

		// put him on the floor!!
		pSoldier->bLevel = 0;

		// OK, drop all items!
		for ( i = 0; i < NUM_INV_SLOTS; i++ )
		{ 
			if( pSoldier->inv[ i ].usItem != 0 )
			{
				WorldItem.fExists = TRUE;
				WorldItem.sGridNo = sAlmaCaptureItemsGridNo[ gStrategicStatus.ubNumCapturedForRescue ];
				WorldItem.ubLevel = 0;
				WorldItem.usFlags = 0;
				WorldItem.bVisible = FALSE;
				WorldItem.bRenderZHeightAboveLevel = 0;

				memcpy( &(WorldItem.o), &pSoldier->inv[i], sizeof( OBJECTTYPE ) );

				AddWorldItemsToUnLoadedSector( 13, 9, 0, sAlmaCaptureItemsGridNo[ gStrategicStatus.ubNumCapturedForRescue ], 1, &WorldItem, FALSE );
				DeleteObj( &( pSoldier->inv[ i ] ) );
			}
		}

		pSoldier->ubStrategicInsertionCode = INSERTION_CODE_GRIDNO;
		pSoldier->usStrategicInsertionData = sAlmaCaptureGridNos[ gStrategicStatus.ubNumCapturedForRescue ];

		gStrategicStatus.ubNumCapturedForRescue++;
	}
	else if ( gubQuest[ QUEST_HELD_IN_ALMA ] == QUESTDONE )
	{
		//-teleport him to N7
		pSoldier->sSectorX = 7;
		pSoldier->sSectorY = 14;
		pSoldier->bSectorZ = 0;
		// put him on the floor!!
		pSoldier->bLevel = 0;


		// OK, drop all items!
		for ( i = 0; i < NUM_INV_SLOTS; i++ )
		{ 
			if( pSoldier->inv[ i ].usItem != 0 )
			{
				WorldItem.fExists = TRUE;
				WorldItem.sGridNo = sInterrogationItemGridNo[ gStrategicStatus.ubNumCapturedForRescue ];
				WorldItem.ubLevel = 0;
				WorldItem.usFlags = 0;
				WorldItem.bVisible = FALSE;
				WorldItem.bRenderZHeightAboveLevel = 0;

				memcpy( &(WorldItem.o), &pSoldier->inv[i], sizeof( OBJECTTYPE ) );

				AddWorldItemsToUnLoadedSector( 7, 14, 0, sInterrogationItemGridNo[ gStrategicStatus.ubNumCapturedForRescue ], 1, &WorldItem, FALSE );
				DeleteObj( &( pSoldier->inv[ i ] ) );
			}
		}

		pSoldier->ubStrategicInsertionCode = INSERTION_CODE_GRIDNO;
		pSoldier->usStrategicInsertionData = gsInterrogationGridNo[ gStrategicStatus.ubNumCapturedForRescue ];

		gStrategicStatus.ubNumCapturedForRescue++;
	}

	//Bandaging him would prevent him from dying (due to low HP)
	pSoldier->bBleeding = 0;

	// wake him up
	if ( pSoldier->fMercAsleep )
	{
		PutMercInAwakeState( pSoldier );
		pSoldier->fForcedToStayAwake = FALSE;
	}

	//Set his life to 50% + or - 10 HP.
	pSoldier->bLife = pSoldier->bLifeMax / 2;
	if ( pSoldier->bLife <= 35 )
	{
		pSoldier->bLife = 35;
	}
	else if ( pSoldier->bLife >= 45 )
	{
		pSoldier->bLife += (INT8)(10 - Random( 21 ) );
	}

	// make him quite exhausted when found
	pSoldier->bBreath = pSoldier->bBreathMax = 50;
	pSoldier->sBreathRed = 0;
	pSoldier->fMercCollapsedFlag = FALSE;
}
*/

void HandleEnemyStatusInCurrentMapBeforeLoadingNewMap()
{
//	INT32 i;
//	BOOLEAN fMadeCorpse;
//	INT8 bKilledEnemies = 0, bKilledCreatures = 0, bKilledRebels = 0, bKilledCivilians = 0;
	return;
/*
	//If any of the soldiers are dying, kill them now.
	for( i = gTacticalStatus.Team[ ENEMY_TEAM ].bFirstID; i <= gTacticalStatus.Team[ ENEMY_TEAM ].bLastID; i++ )
	{
		if( MercPtrs[ i ]->bActive && MercPtrs[ i ]->bLife < OKLIFE && MercPtrs[ i ]->bLife )
		{
			MercPtrs[ i ]->bLife = 0;
			HandleSoldierDeath( MercPtrs[ i ], &fMadeCorpse );
			bKilledEnemies++;
		}
	}
	//Do the same for the creatures.
	for( i = gTacticalStatus.Team[ CREATURE_TEAM ].bFirstID; i <= gTacticalStatus.Team[ CREATURE_TEAM ].bLastID; i++ )
	{
		if( MercPtrs[ i ]->bActive && MercPtrs[ i ]->bLife < OKLIFE && MercPtrs[ i ]->bLife )
		{
			MercPtrs[ i ]->bLife = 0;
			HandleSoldierDeath( MercPtrs[ i ], &fMadeCorpse );
			bKilledCreatures++;
		}
	}
	//Militia
	for( i = gTacticalStatus.Team[ MILITIA_TEAM ].bFirstID; i <= gTacticalStatus.Team[ MILITIA_TEAM ].bLastID; i++ )
	{
		if( MercPtrs[ i ]->bActive && MercPtrs[ i ]->bLife < OKLIFE && MercPtrs[ i ]->bLife )
		{
			MercPtrs[ i ]->bLife = 0;
			HandleSoldierDeath( MercPtrs[ i ], &fMadeCorpse );
			bKilledRebels++;
		}
	}
	//Civilians
	for( i = gTacticalStatus.Team[ CIV_TEAM ].bFirstID; i <= gTacticalStatus.Team[ CIV_TEAM ].bLastID; i++ )
	{
		if( MercPtrs[ i ]->bActive && MercPtrs[ i ]->bLife < OKLIFE && MercPtrs[ i ]->bLife )
		{
			MercPtrs[ i ]->bLife = 0;
			HandleSoldierDeath( MercPtrs[ i ], &fMadeCorpse );
			bKilledCivilians++;
		}
	}

// TEST MESSAGES ONLY!
	if( bKilledCivilians )
		ScreenMsg( FONT_BLUE, MSG_TESTVERSION, L"%d civilians died after you left the sector.", bKilledCivilians );
	if( bKilledRebels )
		ScreenMsg( FONT_BLUE, MSG_TESTVERSION, L"%d militia died after you left the sector.", bKilledRebels );
	if( bKilledEnemies )
		ScreenMsg( FONT_BLUE, MSG_TESTVERSION, L"%d enemies died after you left the sector.", bKilledEnemies );
	if( bKilledCreatures )
		ScreenMsg( FONT_BLUE, MSG_TESTVERSION, L"%d creatures died after you left the sector.", bKilledCreatures );
	
	if( !gbWorldSectorZ )
	{
		SECTORINFO *pSector;
		pSector = &SectorInfo[ SECTOR(gWorldSectorX, gWorldSectorY) ];
		pSector->ubAdminsInBattle = 0;
		pSector->ubTroopsInBattle = 0;
		pSector->ubElitesInBattle = 0;
		pSector->ubCreaturesInBattle = 0;
		//RecalculateSectorWeight( 
	}
	else if( gbWorldSectorZ > 0 )
	{
		UNDERGROUND_SECTORINFO *pSector;
		pSector = FindUnderGroundSector( gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
		if( !pSector )
			return;
		pSector->ubAdminsInBattle = 0;
		pSector->ubTroopsInBattle = 0;
		pSector->ubElitesInBattle = 0;
		pSector->ubCreaturesInBattle = 0;
	}
*/
}


BOOLEAN PlayerSectorDefended( UINT8 ubSectorID )
{
	SECTORINFO *pSector;
	pSector = &SectorInfo[ ubSectorID ];
	if( pSector->ubNumberOfCivsAtLevel[ GREEN_MILITIA ] +
			pSector->ubNumberOfCivsAtLevel[ REGULAR_MILITIA ] +
			pSector->ubNumberOfCivsAtLevel[ ELITE_MILITIA ] )
	{ //militia in sector
		return TRUE;
	}
	if( FindMovementGroupInSector( (UINT8)SECTORX( ubSectorID ), (UINT8)SECTORY( ubSectorID ), TRUE ) )
	{ //player in sector
		return TRUE;
	}
	return FALSE;
}

//Assumes gTacticalStatus.fEnemyInSector
BOOLEAN OnlyHostileCivsInSector()
{
	SOLDIERTYPE *pSoldier;
	INT32 i;
	BOOLEAN fHostileCivs = FALSE;

	//Look for any hostile civs.
	for( i = gTacticalStatus.Team[ CIV_TEAM ].bFirstID; i <= gTacticalStatus.Team[ CIV_TEAM ].bLastID; i++ )
	{
		pSoldier = MercPtrs[ i ];
		if( pSoldier->bActive && pSoldier->bInSector && pSoldier->bLife )
		{
			if( !pSoldier->bNeutral )
			{
				fHostileCivs = TRUE;
				break;
			}
		}
	}
	if( !fHostileCivs )
	{ //No hostile civs, so return FALSE
		return FALSE;
	}
	//Look for anybody else hostile.  If found, return FALSE immediately.
	for( i = gTacticalStatus.Team[ ENEMY_TEAM ].bFirstID; i <= gTacticalStatus.Team[ ENEMY_TEAM ].bLastID; i++ )
	{
		pSoldier = MercPtrs[ i ];
		if( pSoldier->bActive && pSoldier->bInSector && pSoldier->bLife )
		{
			if( !pSoldier->bNeutral )
			{
				return FALSE;
			}
		}
	}
	for( i = gTacticalStatus.Team[ CREATURE_TEAM ].bFirstID; i <= gTacticalStatus.Team[ CREATURE_TEAM ].bLastID; i++ )
	{
		pSoldier = MercPtrs[ i ];
		if( pSoldier->bActive && pSoldier->bInSector && pSoldier->bLife )
		{
			if( !pSoldier->bNeutral )
			{
				return FALSE;
			}
		}
	}
	for( i = gTacticalStatus.Team[ MILITIA_TEAM ].bFirstID; i <= gTacticalStatus.Team[ MILITIA_TEAM ].bLastID; i++ )
	{
		pSoldier = MercPtrs[ i ];
		if( pSoldier->bActive && pSoldier->bInSector && pSoldier->bLife )
		{
			if( !pSoldier->bNeutral )
			{
				return FALSE;
			}
		}
	}
	//We only have hostile civilians, don't allow time compression.
	return TRUE;
}

void HandleBloodCatDeaths( SECTORINFO *pSector )
{
	//if the current sector is the first part of the town
	if( gWorldSectorX == 10 && gWorldSectorY == 9 && gbWorldSectorZ == 0 )
	{
		//if ALL the bloodcats are killed
		if( pSector->bBloodCats == 0 )
		{
			INT8 bId1=-1;
			INT8 bId2=-1;
			INT8 bNum=0;

			SetFactTrue( FACT_PLAYER_KILLED_ALL_BETTYS_BLOODCATS );

			//Instantly have betties missing items show up
			DailyCheckOnItemQuantities( TRUE );

			// Now have a merc say the killed bloodcat quote
			bNum = Get3RandomQualifiedMercs( &bId1, &bId2, NULL );

			//if there are some qualified mercs
			if( bNum != 0 )
			{
				//must make sure TEX doesnt say the quote
				if( bId1 != NOBODY && Menptr[ bId1 ].ubProfile != TEX )
				{
					TacticalCharacterDialogue( &Menptr[ bId1 ], QUOTE_RENEW_REFUSAL_DUE_TO_LACK_OF_FUNDS );
				}
				else if( bId2 != NOBODY && Menptr[ bId2 ].ubProfile != TEX )
				{
					TacticalCharacterDialogue( &Menptr[ bId2 ], QUOTE_RENEW_REFUSAL_DUE_TO_LACK_OF_FUNDS );
				}
			}
		}
	}
}

