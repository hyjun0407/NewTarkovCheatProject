

#ifndef _FUNCTIONS_CPP
#define _FUNCTIONS_CPP

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "../math.hpp"
#include "Kernel.hpp"
#include "extern.h"
typedef unsigned char BYTE;
/* this stuff is for reading nicknames only */
std::string Functions::GetUnicodeString(uint64_t addr, int stringLength)
{
	static char16_t wcharTemp[64];
	ZeroMemory(&wcharTemp, sizeof(wcharTemp));
	 this->Read(addr, wcharTemp, stringLength );
	std::string u8_conv = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(wcharTemp);
	return u8_conv;
}

/* this stuff to read templates, i won't ever use it for sure, but keep just in case */
std::string Functions::readString(ULONG64 GameObject)
{
	static char Buffer[256];
	ZeroMemory(&Buffer, sizeof(Buffer));
	kernel.ReadBuffer(this->pid, &Buffer, GameObject, sizeof(Buffer));
	return std::string(Buffer);
};

/* reading ID of item, workaround for each null-terminator after each symbol */
std::string Functions::readString2(ULONG64 GameObject)
{
	static char Buffer[256];
	ZeroMemory(&Buffer, sizeof(Buffer));
	kernel.ReadBuffer(this->pid, &Buffer, GameObject, sizeof(Buffer));
	std::string Buffer2;

	// bcs IDs stored with /0 after each symbol - needed to skip them and collect all letters/numbers in one storage
	for (auto x = 0; x < 48; x += 2)
	{
		Buffer2.push_back(Buffer[x]);
	}
	return Buffer2;
}


/* getting PID of ETF and UnityPlayer Module, store it */
bool Functions::attach()
{
	CurrentPid = GetCurrentProcessId();
	GamePid = kernel.GetProcessId("EscapeFromTarkov.exe");
	kernel.InitializeHook();
	if (GamePid == 0)
		return false;

	this->uPlayer = kernel.GetClientAddress();
	printf("Unit: %llx \n", this->uPlayer);
	if (this->uPlayer == 0)
		return false;


	return true;
};


/* template used over already exists template in Kernel.hpp to reduce writes by hands "pid" and "sizeof" */
/* im just being too lazy to write this stuff again and again so created one more template over old one */
bool Functions::Readbool(ULONG64 address)
{
	bool buffer;
	buffer = kernel.ReadVirtualMemory<bool>(this->pid, address, sizeof(buffer));
	return buffer;
};

float Functions::ReadFloat(ULONG64 address)
{
	float buffer;
	buffer = kernel.ReadVirtualMemory<float>(this->pid, address, sizeof(float));
	return buffer;
}

Vector2 Functions::Readvec2(ULONG64 address)
{
	Vector2 buffer;
	buffer = kernel.ReadVirtualMemory<Vector2>(this->pid, address, sizeof(Vector2));
	return buffer;
}

void Functions::Write32t(ULONG64 address, int32_t value)
{
	kernel.write32(address, value, sizeof(int32_t));
};
void Functions::Writebt(ULONG64 address, BYTE value)
{
	kernel.write32(address, value, sizeof(BYTE));
};
void Functions::Writevec(ULONG64 address, Vector2 value)
{
	kernel.writeVec(address, value, sizeof(Vector2));
};
void Functions::WriteFloat(ULONG64 address, float value)
{	
	 kernel.writeft(address, value, sizeof(float));	
};

void Functions::WriteBool(ULONG64 address, bool value)
{
	kernel.writebl(address, value, sizeof(bool));
};


D3DXVECTOR3 Functions::ReadVector(ULONG64 address)
{
	D3DXVECTOR3 vector3 = kernel.ReadVirtualMemory<D3DXVECTOR3>(this->pid, address, sizeof(D3DXVECTOR3));
	return vector3;
}

/* read bunch of data (mostly structs) */
bool Functions::Read(ULONG64 address, void* buffer, SIZE_T size)
{
	if (kernel.ReadBuffer(this->pid, buffer, address, size))
	{
		return true;
	}
	return false;
};

/* just simple shit about GOM */
bool Functions::getGom(ULONG64& targetGom)
{
	targetGom = this->uPlayer + offsets->gameObjectManager;

	targetGom = this->Read<ULONG64>(targetGom);
	if (targetGom == NULL)
		return false;
	return true;
};

/* searching gameworld and checking if it's real or not */
bool Functions::findGameWorld(ULONG64& targetGom, ULONG64& localGameWorld)
{
	ULONG64 buffer_gom;	int playerCount;
	buffer_gom = Read<ULONG64>(targetGom + 0x28);
	
	if (buffer_gom != 0)
	{
		int loop_count = 1000;
		for (int i = 0; i < loop_count; i++)
		{
			ULONG64 object_ptr = this->Read<ULONG64>(buffer_gom + 0x10);
			ULONG64 object_name_ptr = this->Read<ULONG64>(object_ptr + 0x60);
			if (this->readString(object_name_ptr) == "GameWorld")
			{
				localGameWorld = this->read_chain(object_ptr, { 0x30, 0x18, 0x28 });
				auto m_pPlayerList = this->Read<ULONG64>(localGameWorld + 0x80);
				playerCount = this->Read<int>(m_pPlayerList + 0x18);
				if (playerCount >= 1 && playerCount <= 60)
				{
					std::cout << "gameworld found hooray " << std::endl;
					std::cout << playerCount << std::endl;
					return true;
				}
			}
			buffer_gom = this->Read<ULONG64>(buffer_gom + 0x8);
		}
	}
	return false;
};

/* getting fps camera with rolling trough all entities */
bool Functions::getFpsCamera(ULONG64& fps_camera)
{
	
	auto temp = this->uPlayer + offsets->camerMainModule;
	temp = this->Read<ULONG64>(temp + 0x0);
	temp = this->Read<ULONG64>(temp + 0x0);

	auto y = 0;	auto const loop_count = 200;
	do
	{
		ULONG64 camera_object;
		camera_object = this->Read<ULONG64>(temp + 0x0);
		if (camera_object != NULL)
		{
			auto camera_name_ptr = this->Read<ULONG64>(camera_object + 0x30);
			camera_name_ptr = this->Read<ULONG64>(camera_name_ptr + 0x60);
			auto camera_target_name = this->readString(camera_name_ptr);
			if (camera_target_name.empty() != TRUE)
			{
				if (camera_target_name == "FPS Camera")
				{
					fps_camera = camera_object;
					break;
				}
			}
		}
		temp = temp + 0x8;	y++;
	}while (y < loop_count);

	return fps_camera;
};
bool Functions::getOptic(ULONG64& fps_camera)
{

	auto temp = this->uPlayer + offsets->camerMainModule;
	temp = this->Read<ULONG64>(temp + 0x0);
	temp = this->Read<ULONG64>(temp + 0x0);

	auto y = 0;	auto const loop_count = 200;
	do
	{
		ULONG64 camera_object;
		camera_object = this->Read<ULONG64>(temp + 0x0);
		if (camera_object != NULL)
		{
			auto camera_name_ptr = this->Read<ULONG64>(camera_object + 0x30);
			camera_name_ptr = this->Read<ULONG64>(camera_name_ptr + 0x60);
			auto camera_target_name = this->readString(camera_name_ptr);
			if (camera_target_name.empty() != TRUE)
			{
				if (camera_target_name == "BaseOpticCamera(Clone)")
				{
					printf("optic fd\n");
					fps_camera = camera_object;
					break;
				}
			}
		}
		temp = temp + 0x8;	y++;
	} while (y < loop_count);

	return fps_camera;
};
/* read chain function to reduce amount of code-lines */
ULONG64 Functions::read_chain(ULONG64& base, const std::vector<ULONG64>& offsets)
{
	auto buffer = this->Read<ULONG64>(base + offsets.at(0));
	if (buffer != 0)
	{
		for (auto y = 1; y < offsets.size(); y++)
		{
			buffer = this->Read<ULONG64>(buffer + offsets.at(y));
			if (buffer != 0)
			{
				continue;
			}
		}
	}
	return buffer;
};

	/* simple reading */
ULONG64 Functions::getViewMatrixPtr(ULONG64& fpsCamera)
{
	ULONG64 viewMatrixPtr;
	std::vector<ULONG64> viewMatrixChain{ 0x30, 0x30, 0x18 };	
	viewMatrixPtr = this->read_chain(fpsCamera, viewMatrixChain);
	return viewMatrixPtr;
};

int Functions::CalculateDistance(D3DXVECTOR3 p1, D3DXVECTOR3 p2)
{
	float diffY = p1.y - p2.y;
	float diffX = p1.x - p2.x;
	float diffZ = p1.z - p2.z;
	return  sqrt((diffX * diffX) + (diffY * diffY) + (diffZ * diffZ));
}

bool Functions::checkItems(float& r, float& g, float& b, float& a, int& price, int& distance)
{
	
	if (price > 0 && price <= 10000)
	{
		if (distance > 0 && distance < 25)
		{
			r = 255; g = 255; b = 255; a = 0.4;
			return true;
		}
		else
		{
			return false;
		}
		
	}
	else if (price > 22000 && price <= 30000)
	{
		if (distance > 0 && distance < 25)
		{
			r = 255; g = 196; b = 0; a = 0.4;
			return true;
		}
		else if (distance >= 25 && distance <= 150)
			{
				r = 255; g = 196; b = 0; a = 0.2;
				return true;
			}
		else
		{
			return false;
		}

	} else	if (price > 30000)
	{
		if (distance > 0 && distance < 25)
		{
			r = 166; g = 0;	b = 255; a = 0.4;
				return true;
		}else if (distance >= 25 && distance <= 150)
			{
				r = 166; g = 0;	b = 255; a = 0.3;
				return true;
			}
			else if (distance > 150 && distance < 300)
			{
				r = 166; g = 0;	b = 255; a = 0.2;
				return true;
			}
				else 
					return false;
	}



}

#endif   // _FUNCTIONS_CPP

