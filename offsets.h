


#ifndef _OFFSETS_HPP
#define _OFFSETS_HPP

#include <basetsd.h>

/* static - gives variable time-life during software execution */
/* constexpr - declaration of variable on software starting */

struct Offsets
{

#ifndef _GLOBAL_OFFSETS

		/* stuff not related to current game-session */	
	
		/* main game module */
	static constexpr  ULONG64 gameObjectManager = 0x17F1CE8;

		/*main camera module */
	static constexpr  ULONG64 camerMainModule = 0x1791540;

		/* item list Ptr */
	static constexpr ULONG64 itemListPtr = 0x60;
	static constexpr ULONG64 firstItem = 0x20;
	static constexpr ULONG64 firstContainerItem = 0x28;
		/* player list Ptr */
	static constexpr  ULONG64 playerListPtr = 0x80;	
	static constexpr  ULONG64 firstPlayer = 0x20;

#endif // _GLOBAL_OFFSETS

		
};


typedef struct Data
{

#ifndef _GLOBAL_GAME_DATA

	/* created to store variables-data only, because i didn't found a better place to do it KEK */


	/* gom module */
	ULONG64 gameObjectManager;

	/* global camera module */
	ULONG64 cameraModule;

	/*fps camera itself*/
	ULONG64 fpsCamera;

#endif // _GLOBAL_GAME_DATA



#ifndef _GAME_SESSION_DATA

	/* variables to store curent game session stuff */

	/*local game world itself keklmao */
	ULONG64 localGameWorld;

	/* ptr to viewMatrix of local player */
	ULONG64 viewMatrixPTR;
	ULONG64 opPTR;
	ULONG64 optricrl;
	/* ptr to itemList struct and to array of items */
	ULONG64 itemListPtr;
	ULONG64 itemListArray;

	/* ptr to playerList struct and to array of players */
	ULONG64 playerListPtr;
	ULONG64 playerListArray;

	int playerCount;
	int itemsCount;
};
#endif // _GAME_SESSION_DATA



#endif // _OFFSETS_HPP