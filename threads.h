#pragma once
#include <basetsd.h>
#include <D3DX10.h>
#include <functional>
#include <iostream>
#include <mutex>
#include <xmmintrin.h>
#include <emmintrin.h>
#include "../math.hpp"

#include "../memory/extern.h"

/* i converted clased from reclass to structs
*  because i wanna collect data straight with bytes-read in one r/w attempt for each struct */



enum Bones : int
{
	HumanRForearm2 = 113,
	HumanRForearm1 = 112,
	HumanRUpperarm = 111,
	HumanNeck = 132,
	HumanLUpperarm = 90,
	HumanLForearm1 = 91,
	HumanLForearm2 = 92,
	HumanHead = 133,
	HumanSpine3 = 37,
	HumanSpine2 = 36,
	HumanSpine1 = 29,
	HumanPelvis = 14,
	HumanRCalf = 22,
	HumanRFoot = 23,
	HumanLCalf = 17,
	HumanLFoot = 18,

};


struct TransformAccessReadOnly
{
	ULONG64	pTransformData = 0;
	int		index = 0;
};
struct TransformData
{
	ULONG64 pTransformArray = 0;
	ULONG64 pTransformIndices = 0;
};
struct BonesStruct
{
	struct TransformAccessReadOnly transformAccess;
	struct TransformData transformData;	
	ULONG64 bonePTR = 0;
};

struct DiscoveredPlayer
{
	FVector headpos;
	std::string NickName = "";
	FVector view;
	ULONG64 Entity = 0;
	bool local = false;
	int Side = 0;
	int Level = 0;
	std::string GroupId;
	std::vector<BonesStruct> bones;
	std::vector<D3DXVECTOR3> bonesVectors;
};



struct DiscoveredItem
{
	ULONG64 Entity = 0;
	std::string itemName = "";
	int price = 0;
	D3DXVECTOR3 positionInWorld = { 0,0,0 };

	bool container = false;
	int countContainerItems = 0;
	ULONG64 containerGrid = 0;
	std::vector<ContainerItem> itemsInContainer;

};





class Threads
{
public:
	struct DiscoveredPlayer
	{
		FVector headpos;
		std::string NickName = "";
		FVector view;
		ULONG64 Entity = 0;
		bool local = false;
		int Side = 0;
		int Level = 0;
		std::string GroupId;
		std::vector<BonesStruct> bones;
		std::vector<D3DXVECTOR3> bonesVectors;
	};
	//update single entity
	DiscoveredPlayer updatePlayer(ULONG64& entity);

	FVector GetPositionaim(uint64_t transform);

	//collect all players
	void GetPlayers(ULONG64 viewMatrixPtr);

	//collect all players
	void GetPlayers2(ULONG64 oprl,ULONG64 viewMatrixPtr,ULONG64 opticptr);


public:

	void UpdateItems();
	void AngleWriter();
	void UpdateItems2();
	void ButtonCatcher();

	DiscoveredItem updateItem(ULONG64& ent);
	std::vector<ContainerItem> updateContainerItem(ULONG64& containerGrid, int containerCount);
	D3DXVECTOR3 GetPosition2(ULONG64& bone);
	FVector GetPosition3(ULONG64& bone);
	D3DXVECTOR3 GetPosition2(BonesStruct& bone);
	FVector GetPosition4(BonesStruct& bone);
	D3DMATRIX* MatrixTranspose(D3DMATRIX* pOut, const D3DMATRIX* pM);
	D3DXVECTOR3 WorldToScreen(D3DXVECTOR3 input_coordinates, uint64_t opticreal,uint64_t optic, uint64_t fps);
	bool isMe(ULONG64 localPointer);
	D3DXVECTOR3 localPlayerHead;
	D3DXMATRIX transposed;




private:
	/* used for internal counters */
	ULONG64 localPlayerPtr;
	int playerCount;
	int itemsCount;
	bool online;

	/* probably this shit will never changed */
#ifndef BONES_CHAIN
	ULONG64 SkeletonRootJoin = 0x28;
	ULONG64 BoneEnumerator = 0x28;
	ULONG64 TransformArray = 0x10;
#endif BONES_CHAIN

};



#ifndef _ITEM_SHIT
struct ItemProfile
{
	char pad_0000[40]; //0x0000
	ULONG64 Interactive; //0x0028
	ULONG64 GameObject; //0x0030
	char pad_0038[16]; //0x0038
};

struct Interactive
{
	char pad_0000[16]; //0x0000
	ULONG64 item_pointer; //0x0010
	char pad_0018[56]; //0x0018
	ULONG64 Item; //0x0050
	char pad_0058[176]; //0x0058
	ULONG64 RE_ItemOwner; //0x0108
};

struct GameObject
{
	char pad_0000[48]; //0x0000
	ULONG64 TransformOne; //0x0030
	char pad_0038[40]; //0x0038
	char ObjectName[8]; //0x0060	
}; 

struct TransformOne
{
	char pad_0000[8]; //0x0000
	ULONG64 TransformTwo; //0x0008
};

struct TransformTwo
{
	char pad_0000[40]; //0x0000
	ULONG64 TransformContainer; //0x0038
};

struct TransformContainer
{
	char pad_0000[16]; //0x0000
	ULONG64 TransformData; //0x0010
};

#endif _ITEM_SHIT

#ifndef _PLAYER_SHIT
struct EFT_Player
{
	char pad_0000[24]; //0x0000
	ULONG64 LocalPlayerChecker; //0x0018
	ULONG64 CharacterController; //0x0020
	char pad_0028[24]; //0x0028
	ULONG64 MovementContext; //0x0040
	char pad_0048[96]; //0x0048
	ULONG64 PlayerBody; //0x00A8
	char pad_00B0[224]; //0x00B0
	ULONG64 ProceduralWeaponAnimation; //0x0190
	char pad_0198[792]; //0x0198
	ULONG64 PlayerProfile; //0x4b0
	char pad_0460[8]; //0x4b8
	ULONG64 Physical; //0x4c0
	char pad_0470[20]; 
	ULONG64 HealthController; //0x4D0	
}; 

struct PlayerProfile
{

	char pad_0000[16]; //0x0000
	ULONG64 UnityEngineString_Id; //0x0010
	ULONG64 UnityEngineString_AccountId; //0x0018
	ULONG64 UnityEngineString_PetId; //0x0020
	ULONG64 PlayerInfo; //0x0028
	char pad_0030[16]; //0x0030
	ULONG64 PlayerHealth; //0x0040
	char pad_0048[24]; //0x0048
	ULONG64 Skills; //0x0060
	char pad_0068[128]; //0x0068
	ULONG64 Stats; //0x00E8	

}; //Size: 0x0068

struct PlayerStats
{
	char pad_0000[16]; //0x0000
	ULONG64 Counters_current; //0x0010
	ULONG64 Counters_overall; //0x0018
	ULONG64 Agressor; //0x0020	
}; //Size: 0x0108

struct PlayerInfo
{
	char pad_0000[16]; //0x0000
	ULONG64 UnityEngineString_Nickname; //0x10
	ULONG64 UnityEngineString_GroupId; //0x18
	ULONG64 UnityEngineString_EntryPoint; //0x20
	char pad_0030[16]; //0x28
	ULONG64 RE_Settings; //0x38
	char pad_0048[24]; //0x0048
	int32_t Side; //0x0058
	int32_t RegistrationDate; //0x005C
	float SavageLockTime; //0x0060
	char pad_0064[4]; //0x0064
	int32_t NicknameChangeDate; //0x0068
	bool IsStreamerModeAvailable; //0x006C
	char pad_006D[3]; //0x006D
	int32_t MemberCategory; //0x0070
	int32_t Experience; //0x0074
	char pad_0078[28]; //0x0078
}; //Size: 0x0094

#endif _PLAYER_SHIT



#ifndef _ITEM_SHIT





#endif _ITEM_SHIT