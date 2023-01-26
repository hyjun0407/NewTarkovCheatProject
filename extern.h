#pragma once

#include <D3DX10.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <functional>
#include <iostream>
#include <mutex>

#include "functions.h"
#include "offsets.h"
#include "../memory/json.hpp"

extern struct Data data;
extern Functions* func;
extern Offsets* offsets;
extern nlohmann::json* tarkovMarketPTR;
extern nlohmann::json TarkovMarket;

extern std::vector<std::string> trkMarketId;
extern std::vector<std::string>trkMarketName;
extern std::vector<int>trkMarketPrice;

extern std::vector
	<std::pair
	<std::string, std::pair
	<std::string, int>>> trkMarket;



inline bool online;

template <typename Type>
struct threadData
{
	std::vector<Type> vector;
	std::mutex L;
	std::mutex N;
	std::mutex M;	
};
struct DrawPlayer
{
	std::string NickName;
	int level = 0;
	int side = 0;
	bool local = false;
	std::string GroupId;
	std::vector<D3DXVECTOR3> bonesVectors;
	int distance = 0;
};


struct ContainerItem
{
	std::string name = "";
	int price = 0;
	ULONG64 itemPtr = 0;
};
struct DrawItem
{
	std::string itemName = "";
	bool isContainer = false;
	D3DXVECTOR3 itemPosition = { 0,0,0 };
	int distance = 0;
	int price = 0;
	std::vector<ContainerItem> itemsInContainer;
};





class Globals
{
public:

	threadData<DrawPlayer> drawingPlayers = {};
	threadData<DrawItem> drawingItems = {};
};
extern Globals* globals;
