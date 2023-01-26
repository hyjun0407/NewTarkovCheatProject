#pragma once


#include <basetsd.h>
#include <thread>
#include <fstream>
#include "Drawing/DirectOverlay.h"

#include "../NewTarkovProject/threads/threads.h"
#include "../NewTarkovProject/memory/extern.h"
#include "../NewTarkovProject/memory/json.hpp"
#include "reg.h"

/* handle to game window */
HWND handleGame;
HANDLE event_handle = NULL;
HANDLE event_handle2 = NULL;
//Read/Write Buffer struct
struct RWbuf {
	int command;
	int struct_size = 0;
	DWORD64 offset = 0;
	DWORD64 structure = 0;
	//int done = 1;
};
RWbuf buffer;


Threads* threads = new Threads();

nlohmann::json baselineOpenAndRead(std::string fileName)
{
	nlohmann::json baseJObject;
	std::string filePath = fileName;

	std::ifstream inFileJSON(fileName);
	inFileJSON >> baseJObject;
	return baseJObject;
}

struct EntitySides
{
	float colors[3];
	int side;
	std::string name;
};

std::vector <EntitySides> entitySides =
{
	{{0,6,255},     1, "Sniper"},
	{{0, 6, 255},   2, "Scav"},
	{{255,234,0,},  4, "P_Scav"},
	{{183,0,255},   8, "Reshala"},
	{{0,0,255},     16, "Guard"},
	{{0,0,255},     32, "Guard"},
	{{255, 0, 0},   55, ""},
	{{255, 0,0},    56, ""},
	{{183,0,255},   64, "Killa"},
	{{183,0,255},   128, "Shturman"},
	{{0,0,255},     256, "Guard"},
	{{0,0,255},     512, "Raider"},
	{{183,0,255},   2048, "Glukhar"},
	{{0,0,255},     4096, "Guard"},
	{{ 0,0,255},    8192, "Guard"} ,
	{{ 0,0,255},	16384, "Guard"} ,
	{{ 0,0,255},    65536, "Guard"} ,
	{{ 183,0,255},  131072, "Sanitar"} ,
	{{ 183,0,255},  2097152, "Sectarian"},
	{{ 183,0,255},  4194304, "Tagilla"},
};



/* render loop */
void drawLoop(int width, int height)
{  

	
	/* used to limit fps */
	//Sleep(1);
	DrawBox(1815, 220, 100, 150, 0, 0, 0, 0, 0.6, 1);
	
	
	globals->drawingPlayers.L.lock();
	globals->drawingPlayers.N.lock();
	globals->drawingPlayers.M.lock();
	globals->drawingPlayers.N.unlock();
	std::vector<DrawPlayer> temporaryStorage = globals->drawingPlayers.vector;
	globals->drawingPlayers.M.unlock();
	globals->drawingPlayers.L.unlock();
	
	float r, g, b; 
	
	int botCount = 0;

	/* draw players */
	for (DrawPlayer ent : temporaryStorage)
	{
		if (ent.local == true)
			continue;
		

		/* ugly function to search trough entitysides vector struct and find matching playerSide of current entity */
		const int currentSide = ent.side;
		const auto p = std::find_if(entitySides.begin(), entitySides.end(),
			[currentSide](const EntitySides& a) { return a.side == currentSide; });

		/* allocate colors for entity */
		r = p[0].colors[0];	g = p[0].colors[1];	b = p[0].colors[2];
		/* allocate name for entity */
		std::string name;
		ZeroMemory(&name, sizeof(name));
		if (p[0].name == "")
		{
			if (p[0].side == 55)
				name = std::to_string(ent.level) + "  [" + ent.NickName + "]" + "(U)";

			else if (p[0].side == 56)
				name = std::to_string(ent.level) + "  [" + ent.NickName + "]" + "(B)";
			/* it's a player here, grab nickname from storage of entity */
		}
		else 
		{
			/* it's a bot here, grab name from bot EntitySides struct */
			botCount++;
			 name = "[" + p[0].name + "]";
		}


		if (ent.distance < 300)
		{
			int bone_array[] = { 0, 1, 2, 3, 4, 5 };
			int bone_array2[] = { 8, 9, 10,11,12 };
			for (int bone : bone_array)
			{
				if (ent.bonesVectors[bone].x != 0 && ent.bonesVectors[bone].y != 0)

				{
					DrawLine(ent.bonesVectors[bone].x, ent.bonesVectors[bone].y, ent.bonesVectors[bone + 1].x, ent.bonesVectors[bone + 1].y, 0.5, r, g, b, 1);
				}
				else
				{
					continue;
				}
			}
			for (int bone : bone_array2)
			{
				if (ent.bonesVectors[bone].x != 0 && ent.bonesVectors[bone].y != 0)
					DrawLine(ent.bonesVectors[bone].x, ent.bonesVectors[bone].y, ent.bonesVectors[bone + 1].x, ent.bonesVectors[bone + 1].y, 0.5, r, g, b, 1);
			}
			if ((ent.bonesVectors[11].x != 0 && ent.bonesVectors[11].y != 0) && (ent.bonesVectors[14].x != 0 && ent.bonesVectors[14].y != 0))
			DrawLine(ent.bonesVectors[11].x, ent.bonesVectors[11].y, ent.bonesVectors[14].x, ent.bonesVectors[14].y, 0.5, r, g, b, 1);
			if ((ent.bonesVectors[14].x != 0 && ent.bonesVectors[14].y != 0) && (ent.bonesVectors[15].x != 0 && ent.bonesVectors[15].y != 0))
			DrawLine(ent.bonesVectors[14].x, ent.bonesVectors[14].y, ent.bonesVectors[15].x, ent.bonesVectors[15].y, 0.5, r, g, b, 1);
			if ((ent.bonesVectors[3].x != 0 && ent.bonesVectors[3].y != 0) && (ent.bonesVectors[7].x != 0 && ent.bonesVectors[7].y != 0))
			DrawLine(ent.bonesVectors[3].x, ent.bonesVectors[3].y, ent.bonesVectors[7].x, ent.bonesVectors[7].y, 0.5, r, g, b, 1);
			if ((ent.bonesVectors[3].x != 0 && ent.bonesVectors[3].y != 0) && (ent.bonesVectors[8].x != 0 && ent.bonesVectors[8].y != 0))
			DrawLine(ent.bonesVectors[3].x, ent.bonesVectors[3].y, ent.bonesVectors[8].x, ent.bonesVectors[8].y, 0.5, r, g, b, 1);

			if (ent.bonesVectors[13].x != 0 && ent.bonesVectors[13].y != 0)
			{
				DrawString(name, 8.5, ent.bonesVectors[13].x, ent.bonesVectors[13].y + 15, 192, 192, 192, 0.9);
				DrawString("(" + std::to_string(ent.distance) + ")", 8.5, ent.bonesVectors[13].x - 20, ent.bonesVectors[13].y + 15, 192, 192, 192, 0.9);
			}
			
			/*DrawCircle(ent.bonesVectors[7].x, ent.bonesVectors[7].y-2, ent.distance, 0.5, 0, 6, 255, 1, false);*/
		}		
	}
	DrawString(std::to_string(botCount), 10, 1815 + 50, 220 + 30, 0, 128, 0, 1);

	globals->drawingItems.L.lock();
	globals->drawingItems.N.lock();
	globals->drawingItems.M.lock();
	globals->drawingItems.N.unlock();
	std::vector<DrawItem> temporaryStorage1 = globals->drawingItems.vector;
	globals->drawingItems.M.unlock();
	globals->drawingItems.L.unlock();
	for (DrawItem ent : temporaryStorage1)
	{
		int distance = func->CalculateDistance(ent.itemPosition, threads->localPlayerHead);
		if (distance > 300)
			continue;
		float a;
		int x = 0;
		D3DXVECTOR3 screenItemPos = threads->WorldToScreen(ent.itemPosition, data.optricrl, data.opPTR, data.viewMatrixPTR);
		if (screenItemPos.z < 1.5f)
			continue;
		if (ent.isContainer == false)
		{
			if (func->checkItems(r, g, b, a, ent.price, distance) == true)
				DrawString(ent.itemName+"  ("+std::to_string(distance)+")", 8.5, screenItemPos.x, screenItemPos.y, 133, g, b, a);
		}
		else if (ent.isContainer == true && ent.itemsInContainer.size() != 0)
		{
			for (ContainerItem ent2 : ent.itemsInContainer)
			{
				if (ent2.price == 0)
					continue;
				if (func->checkItems(r, g, b, a, ent2.price, distance) == true)
				{

					DrawString("[ " + ent2.name +"  (" +std::to_string(distance) + ")"+"  ]", 8.5, screenItemPos.x, screenItemPos.y + x, r, g, b, a);
					x += 10;
				}
			}
		}
	}
	
	
}

nlohmann::json TarkovMarket;
nlohmann::json* tarkovMarketPTR;
std::vector<std::string> trkMarketId;
std::vector<std::string>trkMarketName;
std::vector<int>trkMarketPrice;

std::vector<
	std::pair
	<std::string, std::pair
	<std::string, int>>> trkMarket;
bool MemoryCompare(const BYTE* bData, const BYTE* bMask, const char* szMask) {
	for (; *szMask; szMask += 1, ++bData, ++bMask)
	{
		if ((*szMask == 'x' && *(szMask + 1) == 'x' && *bData != *bMask) ||
			(*szMask == '?' && *(szMask + 1) == 'x' && (*bData & 0b00001111) != (*bMask & 0b00001111)) ||
			(*szMask == 'x' && *(szMask + 1) == '?' && (*bData & 0b11110000) != (*bMask & 0b11110000)))
		{
			return false;
		}
	}
	return (*szMask == NULL);
}

bool CompareData(const BYTE* pbData, const BYTE* pattern, const char* XsAndQuestionMarks)
{
	for (; *XsAndQuestionMarks; ++XsAndQuestionMarks, ++pbData, ++pattern)
	{
		if (*XsAndQuestionMarks == 'x' && *pbData != *pattern)
		{
			return FALSE;
		}
	}
	return (*XsAndQuestionMarks) == NULL;
}
std::vector<LPCVOID> SigScan(const std::vector<BYTE> sig, const char* mask, HANDLE hd)
{
	uint64_t startAddr = 0;
	MEMORY_BASIC_INFORMATION info;
	std::vector<LPCVOID> retValues;

	for (startAddr = 0x6e0000;
		VirtualQueryEx(hd, (PBYTE)startAddr, &info, sizeof(info)) == sizeof(info);
		startAddr += info.RegionSize)
	{
		std::vector<BYTE> buffer;

		if (info.State == MEM_COMMIT &&
			(info.Type == MEM_MAPPED || info.Type == MEM_PRIVATE) &&
			info.AllocationProtect == PAGE_EXECUTE_READWRITE)
		{

			buffer.resize(info.RegionSize);
			//SIZE_T bytes_read;
			//bypass.RWVM(bypass.GetTargetHandle(), startAddr, &buffer[0], info.RegionSize, &bytes_read);
			//ReadProcessMemory(hd, (LPCVOID)startAddr, &buffer[0], info.RegionSize, &bytes_read);
			//buffer.resize(bytes_read);

			func->Read((uint64_t)startAddr, &buffer[0], info.RegionSize);

			/*if (bytes_read < sig.size())
				continue;*/

			if (buffer.size() < sig.size())
				continue;

			for (SIZE_T i = 0; i < buffer.size() - sig.size(); /*bytes_read - sig.size();*/ i++)
			{
				if (CompareData(&buffer[i], &sig[0], mask))
				{
					retValues.push_back((LPCVOID)(startAddr + i));
				}
			}
		}
	}

	return retValues;
}
DWORD64 GetProcessIdT(const char* process_name)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (!Process32First(snapshot, &entry) == TRUE)
	{
		return 0;
	}

	while (Process32Next(snapshot, &entry) == TRUE)
	{
		if (strcmp(entry.szExeFile, process_name) == 0)
		{
			return (DWORD64)entry.th32ProcessID;
		}
	}

	return 0;
}
int main()
{


	online = false;

	/* loading json tarkov market file */

	std::string FileAddress = "C:\\codebeautify.json";
	TarkovMarket = baselineOpenAndRead(FileAddress);
	tarkovMarketPTR = &TarkovMarket;
		
	for (const auto& item : TarkovMarket.items())
	{		
		std::string itemId = "";
		std::string itemShortName = "";
		std::string curl = "";
		int itemPrice = 0;
		for (const auto& val : item.value().items())
		{
			if (val.key() == "shortName")
			{
				trkMarketName.push_back(val.value().get < std::string>());
				itemShortName = val.value().get<std::string>();
			}
			if (val.key() == "bsgId")
			{
				trkMarketId.push_back(val.value().get < std::string>());
				itemId = val.value().get<std::string>();
			}
			if (val.key() == "traderPriceCur")
			{
				curl = val.value().get<std::string>();
			}
			if (val.key() == "traderPrice")
			{
				trkMarketPrice.push_back(val.value().get <int>());
				itemPrice = val.value().get<int>();
				if (curl == "$") {
					itemPrice = itemPrice * 125;
				}
			}
		}
	
		trkMarket.resize(trkMarket.size() + 1);
		trkMarket[trkMarket.size() - 1].first = itemId;
		trkMarket[trkMarket.size() - 1].second.first = itemShortName;
		trkMarket[trkMarket.size() - 1].second.second = itemPrice;
	}




	/* first check does the game exists already */
	if (!(handleGame = FindWindowA(NULL, "EscapeFromTarkov")))
	{
		print(4, "Warning: ", "No game found! Terminating. \n");
		Sleep(4000);
		exit(0);
	}
	/* second check - does driver started before the game */
	if (!func->attach())
	{
		print(4, "Warning: ", "Driver not attached, please restart driver and game! \n");
		Sleep(4000);
		exit(0);
	}
	/*const std::vector<BYTE> sig{0x0f,0xb6,0x86,0x00,0x00,0x00,0x00,0x85,0xc0,0x0f,0x84,0x00,0x00,0x00,0x00,0xf3,0x0f,0x10,0x05,0x0};
	const std::vector<BYTE> sig1{ 0x55 ,0x48 ,0x8b ,0xec ,0x48,0x83,0xec,0x00,0x48,0x89,0x75,0x00,0x48,0x89,0x7d,0x00,0x48,0x8b,0xf9,0x48,0x8b,0xf2,0xf3,0x0f,0x10,0x4f };
	const std::vector<BYTE> sig2{ 0x49,0x8b,0xf0,0xf3,0x0f,0x10,0x4f };
	//const std::vector<BYTE> sig3{ 0xf2,0x0f,0x5a,0xe8,0xf3,0x41,0x0f,0x11,0xad,0x00,0x00,0x00,0x00,0x4d,0x8b,0x75 };
	const std::vector<BYTE> sig4{ 0xf2,0x0f,0x5a,0xe8,0xf3,0x41,0x0f,0x11,0xad,0x00,0x00,0x00,0x00,0xf3,0x0f,0x10,0x45,0x00,0xf3,0x0f,0x5a,0xc0,0xf2,0x0f,0x5a,0xe8,0xf3,0x41,0x0f,0x11,0xad,0x00,0x00,0x00,0x00,0xf3,0x0f,0x10,0x45,0x00,0xf3,0x0f,0x5a,0xc0,0xf2,0x0f,0x5a,0xe8,0xf3,0x41,0x0f,0x11,0xad,0x00,0x00,0x00,0x00,0x48,0x63,0x85 };
	const std::vector<BYTE> sig5{ 0xf2,0x0f,0x5a,0xe8,0xf3,0x41,0x0f,0x11,0xad,0x00,0x00,0x00,0x00,0xf3,0x0f,0x10,0x45,0x00,0xf3,0x0f,0x5a,0xc0,0xf2,0x0f,0x5a,0xe8,0xf3,0x41,0x0f,0x11,0xad,0x00,0x00,0x00,0x00,0x48,0x63,0x85 };
	const std::vector<BYTE> sig6{ 0x66, 0x66, 0xa6, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x9a, 0x99, 0x19, 0x3e };
	//int pid = GetPID("EscapeFromTarkov.exe");
	int pid = GetProcessIdT("EscapeFromTarkov.exe");
	auto hd = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid); // nvm
	//BOOL issuc = memio->Attach("EscapeFromTarkov.exe");
	auto B = SigScan(sig, "xxx????xxxx????xxxx?", hd);
	auto A = SigScan(sig1, "xxxxxxx?xxx?xxx?xxxxxxxxxx", hd);
	auto C = SigScan(sig2, "xxxxxxx", hd);
	//auto D = SigScan(sig3, "xxxxxxxxx????xxx",hd);
    auto E = SigScan(sig4, "xxxxxxxxx????xxxx?xxxxxxxxxxxxx????xxxx?xxxxxxxxxxxxx????xxx",hd);
    auto F = SigScan(sig5, "xxxxxxxxx????xxxx?xxxxxxxxxxxxx????xxx",hd);
	if (hd)
	{
		CloseHandle(hd);
	}

	uint64_t patch = 0;
	uint64_t patch1 = 0;
	uint64_t patch2 = 0;
	uint64_t patch3 = 0;
	uint64_t patch4 = 0;
	uint64_t patch5 = 0;
	for (auto r : A)
	{
		patch1 = (uint64_t)r;
	}
	for (auto r : B)
	{
		patch = (uint64_t)r;
	}
	for (auto r : C)
	{
		patch2 = (uint64_t)r - 0x18;
	}
	for (auto r : E)
	{
		patch4 = (uint64_t)r;
	}
	for (auto r : F)
	{
		patch5 = (uint64_t)r;
	}

	if (patch1 && patch && patch2 && patch4 && patch5)
		printf("SC\n");
	func->Writebt(patch + 0, 0xB8);
	func->Writebt(patch + 1, 00);
	func->Writebt(patch + 2, 00);
	func->Writebt(patch + 3, 00);
	func->Writebt(patch + 4, 00);
	func->Writebt(patch + 5, 0x90);
	func->Writebt(patch + 6, 0x90);

	func->Writebt(patch1 + 0, 0x48);
	func->Writebt(patch1 + 1, 0xB8);
	func->Writebt(patch1 + 2, 0x01);
	func->Writebt(patch1 + 3, 00);
	func->Writebt(patch1 + 4, 00);
	func->Writebt(patch1 + 5, 00);
	func->Writebt(patch1 + 6, 00);
	func->Writebt(patch1 + 7, 00);
	func->Writebt(patch1 + 8, 00);
	func->Writebt(patch1 + 9, 00);
	func->Writebt(patch1 + 10, 0xC3);

	func->Writebt(patch2 + 0, 0x48);
	func->Writebt(patch2 + 1, 0xB8);
	func->Writebt(patch2 + 2, 00);
	func->Writebt(patch2 + 3, 00);
	func->Writebt(patch2 + 4, 00);
	func->Writebt(patch2 + 5, 00);
	func->Writebt(patch2 + 6, 00);
	func->Writebt(patch2 + 7, 00);
	func->Writebt(patch2 + 8, 00);
	func->Writebt(patch2 + 9, 00);
	func->Writebt(patch2 + 10, 0xC3);

	func->Writebt(patch4 + 0, 0x41);
	func->Writebt(patch4 + 1, 0xC7);
	func->Writebt(patch4 + 2, 0x85);
	func->Writebt(patch4 + 3, 0xB8);
	func->Writebt(patch4 + 4, 00);
	func->Writebt(patch4 + 5, 00);
	func->Writebt(patch4 + 6, 00);
	func->Writebt(patch4 + 7, 00);
	func->Writebt(patch4 + 8, 00);
	func->Writebt(patch4 + 9, 00);
	func->Writebt(patch4 + 10, 00);
	func->Writebt(patch4 + 11, 0x90);
	func->Writebt(patch4 + 12, 0x90);

	func->Writebt(patch5 + 0, 0x41);
	func->Writebt(patch5 + 1, 0xC7);
	func->Writebt(patch5 + 2, 0x85);
	func->Writebt(patch5 + 3, 0xBC);
	func->Writebt(patch5 + 4, 00);
	func->Writebt(patch5 + 5, 00);
	func->Writebt(patch5 + 6, 00);
	func->Writebt(patch5 + 7, 00);
	func->Writebt(patch5 + 8, 00);
	func->Writebt(patch5 + 9, 00);
	func->Writebt(patch5 + 10, 00);
	func->Writebt(patch5 + 11, 0x90);
	func->Writebt(patch5 + 12, 0x90);*/
	/* not sure what can goes wrong here, but anyway */
	if (!func->getGom(data.gameObjectManager))
	{
		print(4, "Warning: ", "No gameObjectManager found! \n");
		Sleep(4000);
		exit(0);
	}

	/* ============================ */

	/*todo: add proper checking, does world created, and if yes - do all stuff below. */

	/* ============================ */




	/* looking for localGameWorld */
	if (!func->findGameWorld(data.gameObjectManager, data.localGameWorld))
	{
		print(4, "Warning:", "No Local gameworld Found, probably you not started Game Session \n");
		Sleep(4000);
		exit(0);
	}
	/*getting fps cam module */
	
	if (!func->getFpsCamera(data.cameraModule))
	{
		print(4, "Warning: ", "No fps camera found, something goes wrong! Check offsets! \n");
		Sleep(4000);
		exit(0);
	}
	else
		/* setting online flag */
		online = true;

	/* getting viewMatrixPtr */
	data.viewMatrixPTR = func->getViewMatrixPtr(data.cameraModule);
	func->getOptic(data.opPTR);
	data.optricrl = func->getViewMatrixPtr(data.opPTR);
	data.itemListPtr = func->Read<ULONG64>(data.localGameWorld + offsets->itemListPtr);
	data.itemListArray = func->Read<ULONG64>(data.itemListPtr + 0x10);

	data.playerListPtr = func->Read<ULONG64>(data.localGameWorld + offsets->playerListPtr);
	data.playerListArray = func->Read<ULONG64>(data.playerListPtr + 0x10);

	DirectOverlaySetOption(D2DOV_DRAW_FPS);
	DirectOverlaySetup(drawLoop, handleGame);




	auto getPlayersThread = []() {while (true)  threads->GetPlayers2(data.optricrl, data.viewMatrixPTR, data.opPTR); };
	std::thread t1(getPlayersThread);
	auto buttonCatcher = []() {while (true) threads->ButtonCatcher(); };
	std::thread t2(buttonCatcher);
	auto getItemsThread = []() {while (true)  threads->UpdateItems2(); };
	std::thread t3(getItemsThread);
	auto AngleWriting = []() {while (true) threads->AngleWriter(); };
	std::thread t4(AngleWriting);
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	


};

