#pragma once

#ifndef _FUNCTIONS_HPP
#define _FUNCTIONS_HPP
#pragma once
#include <iostream>
#include <vector>
#include <basetsd.h>
#include <thread>
#include <locale>
#include <codecvt>
#include <D3DX10math.h>
#include "../math.hpp"


#define print(c, b, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c); printf(b); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);  printf(__VA_ARGS__)

class Functions
{
public:
	/* getting Gom adress */
	bool getGom(ULONG64& targetGom);
	bool getOptic(ULONG64& fps_camera);
	bool Readbool(ULONG64 address);
	void WriteFloat(ULONG64 address, float value);
	void WriteBool(ULONG64 address, bool value);
	void Write32t(ULONG64 address, int32_t value);
	void Writebt(ULONG64 address, BYTE value);
	void Writevec(ULONG64 address, Vector2 value);

	float ReadFloat(ULONG64 address);
	Vector2 Readvec2(ULONG64 address);

	D3DXVECTOR3 ReadVector(ULONG64 address);
	
	bool Read(ULONG64 address, void* buffer, SIZE_T size);

	template<typename type>
	type Read(ULONG64 Address) {
		type T;
		Read(Address, &T, sizeof(T));
		return T;
	}

	
	/* simple func to read 1by1 offsets in chain */
	ULONG64 read_chain(ULONG64& base, const std::vector<ULONG64>& offsets);

	/* read string (doesn't work for item Id) */
	std::string readString(ULONG64 GameObject);
	/* read string for ID */
	std::string readString2(ULONG64 GameObject);
	/* read unityString for nicknames */
	std::string GetUnicodeString(uint64_t addr, int stringLength);

	/* distance calculator */
	int CalculateDistance(D3DXVECTOR3 p1, D3DXVECTOR3 p2);
	
	/* getting PID of ETF and UnityPlayer Module, store it */
	bool attach();

	/* get localGameWorld PTR + store player Count immediately */
	bool findGameWorld(ULONG64& targetGom, ULONG64& localGameWorld);

	/* getting fps camera module */
	bool getFpsCamera(ULONG64& fps_camera);

	/* store viewMatrixPtr of player */
	ULONG64 getViewMatrixPtr(ULONG64& fpsCamera);
	
	bool checkItems(float& r, float& g, float& b, float& a, int& itemPrice, int& itemDistance);

	
private:

	/* process Id */
	ULONG64 pid;

	/* UnityPlayer address */
	ULONG64 uPlayer;
};



#endif // _FUNCTIONS_HPP


