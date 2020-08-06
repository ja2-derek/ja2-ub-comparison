#ifdef PRECOMPILEDHEADERS
	#include "Tactical All.h"
	#include "Ja25_Tactical.h"
	#include "_Ja25EnglishText.h"
	#include "Ja25 Strategic Ai.h"
	#include "end game.h"
#else
#endif


//*******************************************************************
//
// Local Defines
//
//*******************************************************************
;

//*******************************************************************
//
// Global Variables
//
//******************************************************************

//*******************************************************************
//
// Function Prototypes
//
//*******************************************************************
void		StopPowerGenFan();
void		HandlePickingUpMorrisInstructionNote( SOLDIERTYPE *pSoldier, INT32 iIndex );
UINT32	GetNumberOfTurnsPowerGenFanWillBeStoppedFor();
//ppp

//*******************************************************************
//
// Functions
//
//*******************************************************************

BOOLEAN	IsSoldierQualifiedMercForSeeingPowerGenFan( SOLDIERTYPE *pSoldier )
{
	if( pSoldier->ubProfile == MANUEL		||
			pSoldier->ubProfile ==  53 || //PGCMale3
			pSoldier->ubProfile ==  55 || //PGCFem2
			pSoldier->ubProfile ==  57		//PGCMale4
		)
	{
		return( TRUE );
	}
	else
	{
		return( FALSE );
	}
}



void StopPowerGenFan()
{
	UINT16 usTileIndex;
	UINT16 usGridNo = 10980;
	SOLDIERTYPE *pSoldier=NULL;

	// ATE: If destroyed, don't go into here
	if( gJa25SaveStruct.ubStateOfFanInPowerGenSector == PGF__BLOWN_UP )
	{
		return;
	}

	//If the fan is already stopped, or has been stopped in the past
	if( gJa25SaveStruct.ubStateOfFanInPowerGenSector == PGF__STOPPED ||
			IsJa25GeneralFlagSet( JA_GF__POWER_GEN_FAN_HAS_BEEN_STOPPED ) )
	{
		//leave
		return;
	}

	//if we are in turn based combat
	if( !((gTacticalStatus.uiFlags & TURNBASED) && (gTacticalStatus.uiFlags & INCOMBAT)) )
	{
		return;
	}

	//Remeber how the player got through
	SetJa25GeneralFlag( JA_GF__POWER_GEN_FAN_HAS_BEEN_STOPPED );

	gJa25SaveStruct.ubStateOfFanInPowerGenSector = PGF__STOPPED;

	//Set the fact that player stopped the fan
	SetFactTrue( FACT_FAN_STOPPPED );

	//Is biggens on the team
	pSoldier = FindSoldierByProfileID( BIGGENS, TRUE );
	if( pSoldier != NULL )
	{
		SetFactTrue( FACT_BIGGENS_ON_TEAM_AND_FAN_STOPPED );
	}

	//Turn off the power gen fan sound
	HandleRemovingPowerGenFanSound();

	//remeber which turn the fan stopped on
	gJa25SaveStruct.uiTurnPowerGenFanWentDown = gJa25SaveStruct.uiTacticalTurnCounter;


	//
	// Replace the Fan graphic
	//

	// Turn on permenant changes....
	ApplyMapChangesToMapTempFile( TRUE );

	//Add the exit grid to the power gen fan
	AddExitGridForFanToPowerGenSector();

	// Remove it!
	// Get index for it...
	GetTileIndexFromTypeSubIndex( FIFTHOSTRUCT, (INT8)( 1 ), &usTileIndex );
	RemoveStruct( usGridNo, usTileIndex );
	

	// Add the new one
	// Get index for it...
	GetTileIndexFromTypeSubIndex( FIFTHOSTRUCT, (INT8)( 7 ), &usTileIndex );
	AddStructToHead( usGridNo, usTileIndex );

	ApplyMapChangesToMapTempFile( FALSE );

	//Recompile the movement costs since we have added a exit grid
	RecompileLocalMovementCosts( usGridNo );

	gpWorldLevelData[ usGridNo ].uiFlags |= MAPELEMENT_REVEALED;

	// Re-render the world!
	gTacticalStatus.uiFlags |= NOHIDE_REDUNDENCY;

	// FOR THE NEXT RENDER LOOP, RE-EVALUATE REDUNDENT TILES
	SetRenderFlags(RENDER_FLAG_FULL);
}

void HandleStartingFanBackUp()
{
	//if the fan has been stopped for the right amount of time
	if( gJa25SaveStruct.uiTacticalTurnCounter == ( gJa25SaveStruct.uiTurnPowerGenFanWentDown + GetNumberOfTurnsPowerGenFanWillBeStoppedFor() ) )
	{
		//if it is stopped
		if( gJa25SaveStruct.ubStateOfFanInPowerGenSector == PGF__STOPPED )
		{
			StartFanBackUpAgain();
		}
	}
}


void StartFanBackUpAgain()
{
	UINT16 usTileIndex;
	UINT16 usGridNo=10980;

	// ATE: If destroyed, don't go into here
	if( gJa25SaveStruct.ubStateOfFanInPowerGenSector == PGF__BLOWN_UP )
	{
		return;
	}

	//Remeber how the player got through
	gJa25SaveStruct.ubStateOfFanInPowerGenSector = PGF__RUNNING_NORMALLY;


	//Start the fan sound
	HandleAddingPowerGenFanSound();


	//
	// Replace the Fan graphic
	//

	// Turn on permenant changes....
	ApplyMapChangesToMapTempFile( TRUE );

	// Remove it!
	// Get index for it...
	GetTileIndexFromTypeSubIndex( FIFTHOSTRUCT, (INT8)( 7 ), &usTileIndex );
	RemoveStruct( usGridNo, usTileIndex );
	

	// Add the new one
	// Get index for it...
	GetTileIndexFromTypeSubIndex( FIFTHOSTRUCT, (INT8)( 1 ), &usTileIndex );
	AddStructToHead( usGridNo, usTileIndex );

	ApplyMapChangesToMapTempFile( FALSE );

	//Recompile the movement costs since we have added a exit grid
	RecompileLocalMovementCosts( usGridNo );

	gpWorldLevelData[ usGridNo ].uiFlags |= MAPELEMENT_REVEALED;

	// Re-render the world!
	gTacticalStatus.uiFlags |= NOHIDE_REDUNDENCY;

	//Remove the exit grid
	RemoveExitGridFromWorld( PGF__FAN_EXIT_GRID_GRIDNO );

	// FOR THE NEXT RENDER LOOP, RE-EVALUATE REDUNDENT TILES
	SetRenderFlags(RENDER_FLAG_FULL);
}

void HandlePowerGenAlarm()
{
	static UINT32 uiLastTime=0;
	static UINT32 uiAlarmCounter=0;
	UINT32 uiCurTime=0;

	//if its not the right sector
	if( !( gWorldSectorX == 13 && gWorldSectorY == MAP_ROW_J && gbWorldSectorZ == 0 ) )
	{
		//leave
		return;
	}

	//if the fan is not stopped
	if( gJa25SaveStruct.ubStateOfFanInPowerGenSector != PGF__STOPPED )
	{
		uiAlarmCounter = 0;
		//leave
		return;
	}

	uiCurTime = GetJA2Clock();

	if( ( uiCurTime - uiLastTime ) >= TCTL__DELAY_BETWEEN_ALARM_SOUNDS )
	{
		uiAlarmCounter++;

		PlayJA2SampleFromFile( "SOUNDS\\AlarmFan.wav", RATE_11025, MIDVOLUME, 1, MIDDLE );

		uiLastTime = uiCurTime;

	
		if( uiAlarmCounter == 2 && !IsJa25GeneralFlagSet( JA_GF__BIGGENS_SAID_QUOTE_117 ) )
		{
			//
			// Have merc say the quote, if the conditions are right
			//
			if( gubFact[ FACT_PLAYER_KNOWS_ABOUT_FAN_STOPPING ] )
			{
				INT8 bID = RandomSoldierIdFromNewMercsOnPlayerTeam();

				if( bID != -1 )
				{
					TacticalCharacterDialogue( &Menptr[ bID ], QUOTE_PERSONALITY_BIAS_WITH_MERC_2 );
				}
			}
			else
			{
				UINT8 bSoldierId1, bSoldierId2, bSoldierId3;
				Get3RandomQualifiedMercs( &bSoldierId1, &bSoldierId2, &bSoldierId3 );

				if( bSoldierId1 != -1 && Menptr[ bSoldierId1 ].ubProfile != BIGGENS )
				{
					TacticalCharacterDialogue( &Menptr[ bSoldierId1 ], QUOTE_PERSONALITY_BIAS_WITH_MERC_1 );
				}
				else if( bSoldierId2 != -1 && Menptr[ bSoldierId2 ].ubProfile != BIGGENS )
				{
					TacticalCharacterDialogue( &Menptr[ bSoldierId2 ], QUOTE_PERSONALITY_BIAS_WITH_MERC_1 );
				}
				else if( bSoldierId3 != -1 && Menptr[ bSoldierId3 ].ubProfile != BIGGENS )
				{
					TacticalCharacterDialogue( &Menptr[ bSoldierId3 ], QUOTE_PERSONALITY_BIAS_WITH_MERC_1 );
				}
			}
		}
	}
}




void HandleAddingPowerGenFanSound()
{
	INT16 sGridNo;

	//if its not already playing
	if( gJa25SaveStruct.iPowerGenFanPositionSndID != -1 )
	{
		return;
	}

	if( gbWorldSectorZ == 0 )
		sGridNo = 10979;
	else
		sGridNo = 19749;

	//Create the new ambient fan sound
	gJa25SaveStruct.iPowerGenFanPositionSndID = NewPositionSnd( sGridNo, POSITION_SOUND_STATIONARY_OBJECT, 0, POWER_GEN_FAN_SOUND );

	SetPositionSndsInActive( );
	SetPositionSndsActive( );
}

void HandleRemovingPowerGenFanSound()
{
	//if there is an ambience sound playing
	if( gJa25SaveStruct.iPowerGenFanPositionSndID != -1 )
	{
		//delete it
		DeletePositionSnd( gJa25SaveStruct.iPowerGenFanPositionSndID );

		gJa25SaveStruct.iPowerGenFanPositionSndID = -1;
	}
}


void AddExitGridForFanToPowerGenSector()
{
	EXITGRID ExitGrid;

	memset( &ExitGrid, 0, sizeof( EXITGRID ) );

	ExitGrid.ubGotoSectorX = 14;
	ExitGrid.ubGotoSectorY = MAP_ROW_J;
	ExitGrid.ubGotoSectorZ = 1;
	ExitGrid.usGridNo = 19749;

	//Add the exit grid when the fan is either stopped or blown up
	AddExitGridToWorld( PGF__FAN_EXIT_GRID_GRIDNO, &ExitGrid );
}

void HandleHowPlayerGotThroughFan()
{
		switch( gJa25SaveStruct.ubStateOfFanInPowerGenSector )
		{
			case PGF__STOPPED:
				gJa25SaveStruct.ubHowPlayerGotThroughFan = PG__PLAYER_STOPPED_FAN_TO_GET_THROUGH;
				break;
			
			case PGF__BLOWN_UP:
				gJa25SaveStruct.ubHowPlayerGotThroughFan = PG__PLAYER_BLEW_UP_FAN_TO_GET_THROUGH;
			
				//If the player blew up the fan, then the enemies can hear it in the tunnel and prepare for it.
				gJa25SaveStruct.uiJa25GeneralFlags |= JA_GF__DID_PLAYER_MAKE_SOUND_GOING_THROUGH_TUNNEL_GATE;
				break;
		}

}
void HandlePickingUpMorrisInstructionNote( SOLDIERTYPE *pSoldier, INT32 iIndex )
{
	INT8	bID=-1;

	if( iIndex != MORRIS_INSTRUCTION_NOTE )
	{
		return;
	}

	//if the note has already been picked up by a new merc, or the not ehas been displayed
	if( gJa25SaveStruct.ubMorrisNoteState == MN__PICKED_UP_BY_NEW_MERC ||
			gJa25SaveStruct.ubMorrisNoteState == MN__FINISHED )
	{
		return;
	}

	bID = RandomSoldierIdFromNewMercsOnPlayerTeam();

	//if this is a qualified merc on the team
	if( IsSoldierQualifiedMerc( pSoldier ) )
	{
		//Remember we said the quote
		gJa25SaveStruct.ubMorrisNoteState = MN__PICKED_UP_BY_NEW_MERC;

		//Delaying the merc to say the note
		DelayedMercQuote( pSoldier->ubProfile, DQ__NEW_MERC_SAY_NOTE_QUOTES, GetWorldTotalSeconds()+1 );
	}

	//else if there is a new merc on the team
	else if( bID != -1 )
	{
		UINT32 uiTime=0;

		if( gJa25SaveStruct.ubMorrisNoteState == MN__PICKED_UP_BY_OLD_MERC_SAID_QUOTE_ALREADY )
		{
			return;
		}

		//the merc is not a new merc
		gJa25SaveStruct.ubMorrisNoteState = MN__PICKED_UP_BY_OLD_MERC_SAID_QUOTE_ALREADY;

		TacticalCharacterDialogue( &Menptr[ bID ], QUOTE_RENEWING_CAUSE_BUDDY_1_ON_TEAM );

		gJa25SaveStruct.bNewMercProfileIDForSayingMorrisNote = bID;

		//if we are in turnbased combat
		if( (gTacticalStatus.uiFlags & TURNBASED) && (gTacticalStatus.uiFlags & INCOMBAT) )
		{
			uiTime = GetWorldTotalSeconds() + 3;
		}
		else
		{
			uiTime = GetWorldTotalSeconds() + 10;
		}

		DelayedMercQuote( NOBODY, DQ__MORRIS_NOTE_NEW_MERC_DELAY, uiTime );
	}

	else
	{
		//
		// Create a merc popup box that will display a message telling player what to do
		//

		gJa25SaveStruct.bNewMercProfileIDForSayingMorrisNote = pSoldier->ubProfile;

		DelayedMercQuote( pSoldier->ubProfile, DQ__MORRIS_NOTE_DISPLAY_NOTE_1, GetWorldTotalSeconds() + 1 );

		gJa25SaveStruct.ubMorrisNoteState = MN__FINISHED;
	}
}

void HandleDeathInPowerGenSector( SOLDIERTYPE *pSoldier )
{
	//if this is NOT the power gen sector
	if( gWorldSectorX != 13 || gWorldSectorY != 10 || gbWorldSectorZ != 0 )
	{
		return;
	}
	else
	{
		BOOLEAN fFoundValidEnemy=FALSE;
		UINT8		ubNumFlagedEnemiesInSector=0;
		INT16		sRandomSlotGridNo;
		UINT8		uiCnt;
		SOLDIERINITNODE	*pInitListSoldier;

		#define NUM_ENEMIES_SLOTS			4

		INT16	sEnemyPlacementGridNo[ NUM_ENEMIES_SLOTS ]=
						{
							15100,
							12220,
							14155,
							13980,
						};

		//
		// Count number of Flaged guys still left
		//
		for( uiCnt=0; uiCnt<NUM_ENEMIES_SLOTS; uiCnt++ )
		{
			//Choose a random ID
			sRandomSlotGridNo = sEnemyPlacementGridNo[ uiCnt ];

			//is this soldier still alive
			if( IsSoldierAliveWithInitListGridNo( sRandomSlotGridNo ) )
			{
				ubNumFlagedEnemiesInSector++;
			}
		}


		//
		// Chooose a random slot ID for the enemy
		//


		for( uiCnt=0; uiCnt<50; uiCnt++ )
		{
			//Choose a random ID
			sRandomSlotGridNo = sEnemyPlacementGridNo[ Random( NUM_ENEMIES_SLOTS ) ];

			//is this soldier still alive
			if( IsSoldierAliveWithInitListGridNo( sRandomSlotGridNo ) )
			{
				pInitListSoldier = FindSoldierInitNodeWithID( pSoldier->ubID );

				//is this the same soldier
				if( pInitListSoldier && pInitListSoldier->pBasicPlacement->usStartingGridNo == sRandomSlotGridNo )
				{
					fFoundValidEnemy = TRUE;
				}

				//we are done, 
				break;
			}
		}

		// if we found an enemy, or this is the 2nd last enemy in the sector
		if( fFoundValidEnemy || ubNumFlagedEnemiesInSector <= 2 )
		{
			StopPowerGenFan();
		}
	}		
}


BOOLEAN IsSoldierAliveWithInitListGridNo( INT16 sInitListGridNo )
{
	SOLDIERINITNODE *curr;
	curr = gSoldierInitHead;
	while( curr )
	{
		if( curr->pBasicPlacement->usStartingGridNo == sInitListGridNo && curr->pSoldier != NULL )
		{
			if ( curr->pSoldier->bLife >= OKLIFE )
			{
				return( TRUE );
			}
		}
		curr = curr->next;
	}
	return( FALSE );
}
void HandleFanStartingAtEndOfCombat()
{
	//if its not the right sector
	if( !( gWorldSectorX == 13 && gWorldSectorY == MAP_ROW_J && gbWorldSectorZ == 0 ) )
	{
		//leave
		return;
	}

	//if the fan is not stopped
	if( gJa25SaveStruct.ubStateOfFanInPowerGenSector != PGF__STOPPED )
	{
		//leave
		return;
	}

	StartFanBackUpAgain();
}void HandleFanStartingAtEndOfCombat()

void HandleFanStartingAtEndOfCombat()
{
	//if its not the right sector
	if( !( gWorldSectorX == 13 && gWorldSectorY == MAP_ROW_J && gbWorldSectorZ == 0 ) )
	{
		//leave
		return;
	}

	//if the fan is not stopped
	if( gJa25SaveStruct.ubStateOfFanInPowerGenSector != PGF__STOPPED )
	{
		//leave
		return;
	}

	StartFanBackUpAgain();
}


void HandleInitialEventsInHeliCrash()
{
	SOLDIERTYPE *pSoldier=NULL;
	INT32 cnt;

	//first, loop through all the mercs and injure them
	cnt = gTacticalStatus.Team[ OUR_TEAM ].bFirstID;
	for ( pSoldier = MercPtrs[ cnt ]; cnt <= gTacticalStatus.Team[ OUR_TEAM ].bLastID; cnt++,pSoldier++)
	{    
		//if the merc is alive
		if( pSoldier->bActive )
		{
			pSoldier->bLife -= ( 3 + Random( 5 ) );
			pSoldier->sBreathRed = ( 15 + Random( 15 ) ) * 100;
			pSoldier->bActionPoints = 0;
		}
	}
}


UINT32 GetNumberOfTurnsPowerGenFanWillBeStoppedFor()
{
	UINT32 uiNumTurns = PGF__NUM_TURNS_TILL_START_FAN_BACK_UP_EASY;

	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			uiNumTurns = PGF__NUM_TURNS_TILL_START_FAN_BACK_UP_EASY;
			break;
		case DIF_LEVEL_MEDIUM:
			uiNumTurns = PGF__NUM_TURNS_TILL_START_FAN_BACK_UP_NORMAL;
			break;
		case DIF_LEVEL_HARD:
			uiNumTurns = PGF__NUM_TURNS_TILL_START_FAN_BACK_UP_HARD;
			break;
	}

	return( uiNumTurns );
}


void DisplayCommanderMorrisNote( SOLDIERTYPE *pSoldier )
{
	CHAR16	zString[1024];

	if( gJa25SaveStruct.ubDisplayCommanderMorrisNote == DMN__FINISHED )
	{
		return;
	}

	if( pSoldier == NULL )
	{
		return;
	}

	//if it si the first part of the note to be displayed
	if( gJa25SaveStruct.ubDisplayCommanderMorrisNote == DMN__NOT_TO_DISPLAY_IT )
	{
		gJa25SaveStruct.ubDisplayCommanderMorrisNote = DMN__DISPLAY_PART_1; 

		swprintf( zString, zNewTacticalMessages[ TCTL_MSG__END_GAME_POPUP_TXT_1 ], pSoldier->name );
	}

	//if it si the second part of the note to be displayed
	else if( gJa25SaveStruct.ubDisplayCommanderMorrisNote == DMN__DISPLAY_PART_1 )
	{
		gJa25SaveStruct.ubDisplayCommanderMorrisNote = DMN__DISPLAY_PART_2; 

		swprintf( zString, zNewTacticalMessages[ TCTL_MSG__END_GAME_POPUP_TXT_2 ], pSoldier->name, pSoldier->name );
	}

	//Display it
	ExecuteTacticalTextBox( 110, zString );
}



void HandleCommanderMorrisNewMercWantsNoteDelayedSpeech()
{
	INT8 bID=-1;

	//if the note has already been picked up by a new merc, or the note has been displayed
	if( gJa25SaveStruct.ubMorrisNoteState == MN__PICKED_UP_BY_NEW_MERC ||
			gJa25SaveStruct.ubMorrisNoteState == MN__FINISHED )
	{
		return;
	}

	//if the original merc who said the quote is valid
	bID = gJa25SaveStruct.bNewMercProfileIDForSayingMorrisNote;
	if( bID != -1 && IsSoldierQualifiedMerc( &Menptr[ bID ] ) )
	{
	}
	else
	{
		bID = RandomSoldierIdFromNewMercsOnPlayerTeam();

		if( bID != -1 )
		{
			gJa25SaveStruct.bNewMercProfileIDForSayingMorrisNote = bID;
		}
		else
		{
			return;
		}
	}

	TacticalCharacterDialogue( &Menptr[ bID ], QUOTE_RENEWING_CAUSE_BUDDY_1_ON_TEAM );
}


void HandleShowingRadioLocatorsInMorrisArea()
{
	//Reveal the switch in Morris area
	HandleOpenControlPanelToRevealSwitchInMorrisArea();

	//control panel locater
	BeginMultiPurposeLocator( 15070, 0, TRUE );

	//if we are not in combat
	if( !((gTacticalStatus.uiFlags & TURNBASED) && (gTacticalStatus.uiFlags & INCOMBAT)) )
	{
		DelayedMercQuote( 14744, DQ__SHOW_RADIO_LOCATOR, GetWorldTotalSeconds() + 3 );
	}
}

void HandleNewMercSayingContentsOfMorrisNote( SOLDIERTYPE *pSoldier )
{
	TacticalCharacterDialogue( pSoldier, QUOTE_MERC_LEAVING_ALSUCO_SOON );

	TacticalCharacterDialogue( pSoldier, QUOTE_MERC_GONE_UP_IN_PRICE );
	SpecialCharacterDialogueEvent( DIALOGUE_SPECIAL_EVENT_JERRY_MILO, JM_SE__SHOW_RADIO_LOCATOR, 0, 0, 0, DIALOGUE_TACTICAL_UI );
}

void HandlePlayerHittingSwitchToLaunchMissles()
{
	INT32 cnt;
	SOLDIERTYPE *pSoldier=NULL;

	//
	// The player Just won the game, remeber this
	//
	gGameSettings.fPlayerFinishedTheGame = TRUE;
	SaveGameSettings();

	//Update the quest log
	InternalEndQuest( QUEST_DESTROY_MISSLES, -1, -1, TRUE );

	//lock the interface
	guiPendingOverrideEvent = LU_BEGINUILOCK;
	HandleTacticalUI( );

	//
	// Loop through all the mercs in the sector and make them run to the elevator
	//
	cnt = gTacticalStatus.Team[ OUR_TEAM ].bFirstID;
	for ( pSoldier = MercPtrs[ cnt ]; cnt <= gTacticalStatus.Team[ OUR_TEAM ].bLastID; cnt++,pSoldier++)
	{       
		// if the soldier was in the complex
		if( pSoldier->bActive && pSoldier->bLife >= OKLIFE && pSoldier->bInSector &&
				pSoldier->sSectorX == 15 && pSoldier->sSectorY == 12 && pSoldier->bSectorZ == 3 )
		{
			if( PythSpacesAway( pSoldier->sGridNo, 14268 ) < PythSpacesAway( pSoldier->sGridNo, 15708 ) )
				EVENT_InternalGetNewSoldierPath( pSoldier, 14268, RUNNING, TRUE, TRUE );
			else
				EVENT_InternalGetNewSoldierPath( pSoldier, 15708, RUNNING, TRUE, TRUE );
		}
	}

	//Start a timer so we can fade out
	DelayedMercQuote( NOBODY, DQ__FADE_OUT_TO_END_GAME_CINEMATIC, GetWorldTotalSeconds()+2 );
}
void HandleFadingOutToEndGameCinematics()
{
	gFadeOutDoneCallback = DoneFadeOutEndCinematic;

	FadeOutGameScreen( );	

	// UnLock UI!
	guiPendingOverrideEvent = LU_ENDUILOCK;
	HandleTacticalUI( );
}


void HandleOpenControlPanelToRevealSwitchInMorrisArea()
{
	//if the note has already been picked up by a new merc, or the note has been displayed
	if( gJa25SaveStruct.ubMorrisNoteState == MN__PICKED_UP_BY_NEW_MERC ||
			gJa25SaveStruct.ubMorrisNoteState == MN__FINISHED )
	{
		STRUCTURE *		pStructure;
		INT32					iItemIndex;
		ITEM_POOL *		pItemPool;
		

		// first, find the switch item and turn off its trap level

		if ( ItemTypeExistsAtLocation( 15231, SWITCH, 0, &iItemIndex ) )
		{
			gWorldItems[ iItemIndex ].o.bTrap = 0;
			GetItemPool( 15231, &pItemPool, 0 );
			if ( pItemPool )
			{
				SetItemPoolVisibilityOn( pItemPool, ANY_VISIBILITY_VALUE, FALSE );
			}
		}

		//Open up the Control Panel so the switch is visibile
		pStructure = FindStructure( 15231, STRUCTURE_GENERIC );
		if( pStructure == NULL )
		{
			return;
		}

		if( SwapStructureForPartnerForcingGraphicalChange( 15231, pStructure ) == NULL )
		{
			//an error occured
			return;
		}
	}
}

