


#include <iostream>
#include <thread>
#include <cstdint>

#include "threads.h"
#include "../memory/extern.h"
#include "../math.hpp"

#define M_PI 3.14159265358979323846f
#define M_RADPI    57.295779513082f
#define M_PI_F ((float)(M_PI))
#define radiansToDegrees 57.29578f
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))


bool visorStatus = false; // flase = default state, true = modified state;
ULONG64 visorPtr = 0;
bool isAiming(uint64_t instance)
{
	uint64_t m_pbreath = func->read_chain(instance, { 0x190, 0x28 });
	return func->Readbool(m_pbreath + 0xA0); //0xA0 is aiming
}
bool operator==(const ContainerItem& elem, const ULONG64& url)
{
	return elem.itemPtr == url;
}

bool operator==(const DiscoveredItem& elem, const ULONG64& url)
{
	return elem.Entity == url;
}
bool operator==(const Threads::DiscoveredPlayer& elem, const ULONG64& url)
{
	return elem.Entity == url;
}

threadData <Threads::DiscoveredPlayer> players = {};
threadData <DiscoveredItem> items = {};

uint64_t tablePlayerExperience[] = {
            1000,     4017,    8432,    14256,    21477,    30023,    39936,    51204,    63723, 
  77563,    92713,    111881,  134674,  161139,   191417,   225194,   262366,   302484,   301534, 
  345751,   391649,   426190,  440444,  524580,   492366,   547896,   609066,   675913,   748474, 
  826786,   910885,   1000809, 1096593, 1198275,  1309251,  1429580,  1559321,  1698532,  1847272, 
  2005600,  2173575,  2351255, 2538699, 2735966,  2946585,  3170637,  3408202,  3659361,  3924195,
  4202784,  4495210,  4801553, 5121894, 5456314,  5809667,  6182063,  6984426,  7414613,  7864284,
  8333549,  8831052,  9360623, 9928578, 10541848, 11206300, 11946977, 12789143, 13820522, 15229487,
  17206065, 19706065, 22706065
};


bool Threads::isMe(ULONG64 localPointer)
{
	if (localPointer == 0)
		return false;
	return true;
}

std::vector<ContainerItem> Threads::updateContainerItem(ULONG64& Entries, int containerCount)
{
	std::vector<ContainerItem> container;
	for (int y = 0; y < containerCount; y++)
	{
		ULONG64 itemInCointainerPtr = func->Read<ULONG64>(Entries + 0x28 + y * 0x18);
		if (itemInCointainerPtr != 0)
		{
			ContainerItem containerItem;
			containerItem.itemPtr = itemInCointainerPtr;
			LONG64 itemTemplate = func->Read<ULONG64>(itemInCointainerPtr + 0x40);
			ULONG64 itemIdPtr = func->Read<ULONG64>(itemTemplate + 0x50);
			containerItem.name = func->readString2(itemIdPtr + 0x14);
			for (auto x = 0; x < trkMarketId.size(); x++)
			{
				if (trkMarketId[x] == containerItem.name)
				{
					containerItem.name = trkMarketName[x];
					containerItem.price = trkMarketPrice[x];
				}
			}

			container.push_back(containerItem);
		}
	}
	return container;

}

DiscoveredItem Threads::updateItem(ULONG64& ent)
{
	DiscoveredItem discoveredItem;
	discoveredItem.Entity = ent;
	ULONG64 loot = func->Read<ULONG64>(ent + 0x10);

	ItemProfile* itemProfile = new ItemProfile[sizeof(ItemProfile)];
	func->Read(loot, itemProfile, sizeof(ItemProfile));

	Interactive* itemInteractive = new Interactive[sizeof(Interactive)];
	func->Read(itemProfile->Interactive, itemInteractive, sizeof(Interactive));

	GameObject* gameObject = new GameObject[sizeof(GameObject)];
	func->Read(itemProfile->GameObject, gameObject, sizeof(GameObject));

	TransformOne* transformOne = new TransformOne[sizeof(TransformOne)];
	func->Read(gameObject->TransformOne, transformOne, sizeof(TransformOne));

	TransformTwo* transformTwo = new TransformTwo[sizeof(TransformTwo)];
	func->Read(transformOne->TransformTwo, transformTwo, sizeof(TransformTwo));

	if (itemInteractive->RE_ItemOwner != 0)
	{
		discoveredItem.container = true;
		discoveredItem.itemName = "";

		TransformContainer* transformContainer = new TransformContainer[sizeof(TransformContainer)];
		func->Read(transformTwo->TransformContainer, transformContainer, sizeof(TransformContainer));

		discoveredItem.positionInWorld = this->GetPosition2(transformContainer->TransformData);

		discoveredItem.containerGrid = func->Read<ULONG64>(itemInteractive->RE_ItemOwner + 0xA0);
		discoveredItem.containerGrid = func->Read<ULONG64>(discoveredItem.containerGrid + 0x68);
		discoveredItem.containerGrid = func->Read<ULONG64>(discoveredItem.containerGrid + 0x20);
		discoveredItem.containerGrid = func->Read<ULONG64>(discoveredItem.containerGrid + 0x40);
		discoveredItem.containerGrid = func->Read<ULONG64>(discoveredItem.containerGrid + 0x10);
		discoveredItem.countContainerItems = func->Read<int>(discoveredItem.containerGrid + 0x40);
		if (discoveredItem.countContainerItems == 0)
		{
			
			delete[] itemProfile;
			delete[] itemInteractive;
			delete[] gameObject;
			delete[] transformOne;
			delete[] transformTwo;
			delete[] transformContainer;
			return discoveredItem;
		}
		ULONG64 Entries = func->Read<ULONG64>(discoveredItem.containerGrid + 0x18);
		if (Entries != 0)
		discoveredItem.itemsInContainer = this->updateContainerItem(Entries, discoveredItem.countContainerItems);
		delete[] transformContainer;
	}
	else
	{
		discoveredItem.container = false;
		ULONG64 item = func->Read<ULONG64>(itemInteractive->Item + 0x40);
		ULONG64 itemIdPtr = func->Read<ULONG64>(item + 0x50);
		discoveredItem.itemName = func->readString2(itemIdPtr + 0x14);
		for (auto x = 0; x < trkMarketId.size(); x++)
		{
			if (trkMarketId[x] == discoveredItem.itemName)
			{
				discoveredItem.itemName = trkMarketName[x];
				discoveredItem.price = trkMarketPrice[x];
			}
		}
		ULONG64 itemVectorPtr = func->Read<ULONG64>(transformOne->TransformTwo + 0x38);
		discoveredItem.positionInWorld = func->ReadVector(itemVectorPtr + 0x90);

	}	
	delete[] itemProfile;
	delete[] itemInteractive;
	delete[] gameObject;
	delete[] transformOne;
	delete[] transformTwo;
	return discoveredItem;
}



D3DXVECTOR3 Threads::WorldToScreen(D3DXVECTOR3 input_coordinates, uint64_t opticreal, uint64_t optic, uint64_t fps)
{
	auto isoptic = func->Readbool(optic + 0x39);
	auto isaim1 = func->Read<ULONG64>(this->localPlayerPtr + 0x190);
	isaim1 = func->Read<ULONG64>(isaim1 + 0x28);
	auto isaim = func->Readbool(isaim1 + 0xA0);
	D3DXMATRIX viewmatrix_transposed;
	D3DXMATRIX matrix;
	float fov;
	float zfv;
	float ratio;
	if (isoptic && isaim) {
		func->Read(opticreal + 0xDC, &matrix, sizeof(D3DXMATRIX));
		this->MatrixTranspose(&viewmatrix_transposed, &matrix);
		fov = func->ReadFloat(fps + 0x15C);
		zfv = func->ReadFloat(opticreal + 0x15C);
		ratio = func->ReadFloat(fps + 0x4C8);
	}
	else {
		func->Read(fps + 0xDC, &matrix, sizeof(D3DXMATRIX));
		this->MatrixTranspose(&viewmatrix_transposed, &matrix);
	}

	D3DXVECTOR3 translationVector = D3DXVECTOR3(viewmatrix_transposed._41, viewmatrix_transposed._42, viewmatrix_transposed._43);
	D3DXVECTOR3 up = D3DXVECTOR3(viewmatrix_transposed._21, viewmatrix_transposed._22, viewmatrix_transposed._23);
	D3DXVECTOR3 right = D3DXVECTOR3(viewmatrix_transposed._11, viewmatrix_transposed._12, viewmatrix_transposed._13);
	D3DXVECTOR3 vector3;
	float w = D3DXVec3Dot(&translationVector, &input_coordinates) + viewmatrix_transposed._44;


	if (w < 0.098f)
		w = 1;



	float y = D3DXVec3Dot(&input_coordinates, &up) + viewmatrix_transposed._24;
	float x = D3DXVec3Dot(&input_coordinates, &right) + viewmatrix_transposed._14;
	if (isoptic && isaim) {
		if (fov == 35 && zfv == 19.4f)
			fov = 50;
		auto angleRadHalf = (float)(M_PI / 180) * fov * 0.5f;
		auto angleCtg = (float)(cos(angleRadHalf) / sin(angleRadHalf));
		x /= angleCtg * ratio * 0.5f;
		y /= angleCtg * 0.5f;
	}
	float ScreenX = (1920 / 2) * (1.f + x / w);
	float ScreenY = (1080 / 2) * (1.f - y / w);
	float ScreenZ = w;
	vector3 = { ScreenX, ScreenY, ScreenZ };
	return vector3;
};

D3DMATRIX* Threads::MatrixTranspose(D3DMATRIX* pOut, const D3DMATRIX* pM)
{

	// Legacy Function
	pOut->_11 = pM->_11;
	pOut->_12 = pM->_21;
	pOut->_13 = pM->_31;
	pOut->_14 = pM->_41;

	pOut->_21 = pM->_12;
	pOut->_22 = pM->_22;
	pOut->_23 = pM->_32;
	pOut->_24 = pM->_42;

	pOut->_31 = pM->_13;
	pOut->_32 = pM->_23;
	pOut->_33 = pM->_33;
	pOut->_34 = pM->_43;

	pOut->_41 = pM->_14;
	pOut->_42 = pM->_24;
	pOut->_43 = pM->_34;
	pOut->_44 = pM->_44;

	return pOut;
};



struct Matrix34
{
	D3DXVECTOR4 vec0;
	D3DXVECTOR4 vec1;
	D3DXVECTOR4 vec2;
};

/* this is old transform function, used only for containers, since im lazy to adapt it, and it will be called once */
/* getTransform are overloaded in total */
D3DXVECTOR3 Threads::GetPosition2(ULONG64& bone)
{
	__m128 result;

	/* _m128 -> Vec4 */
	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	/* Read the transform index for this transform */
	auto index = func->Read<int>(bone + 0x40);

	/* Getting the transform data ptr */
	TransformAccessReadOnly pTransformAccessReadOnly = func->Read<TransformAccessReadOnly>(bone + 0x38);
	TransformData transformData = func->Read<TransformData>(pTransformAccessReadOnly.pTransformData + 0x18);


	/* Set the size of the matricies & indicies buffers */
	std::size_t size_matricies_buffer = sizeof(Matrix34) * index + sizeof(Matrix34);
	std::size_t size_indices_buffer = sizeof(int) * index + sizeof(int);

	/* Allocate memory for the matricies & indicies buffers */
	void* pMatriciesBuf = malloc(size_matricies_buffer);
	void* pIndicesBuf = malloc(size_indices_buffer);

	/* count position with some magic mathematic*/
	if (pMatriciesBuf && pIndicesBuf) {

		/* read matricies data to buffer, copy to allocated memory */
		func->Read(transformData.pTransformArray, pMatriciesBuf, size_matricies_buffer);

		/* read indicies data to buffer, copy to allocated memory */
		func->Read(transformData.pTransformIndices, pIndicesBuf, size_indices_buffer);

		/* some magic mathematic here and downside */
		result = *(__m128*)((uint64_t)pMatriciesBuf + 0x30 * index);
		int transform_index = *(int*)((uint64_t)pIndicesBuf + 0x4 * index);


		for (int i = 0; transform_index >= 0 && i < 30 && transform_index < size_indices_buffer; i++)
		{
			Matrix34 matrix34;

			matrix34 = *(Matrix34*)((uint64_t)pMatriciesBuf + 0x30 * transform_index);



			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), _mm_load_ps(&matrix34.vec0.x));



			transform_index = *(int*)((uint64_t)pIndicesBuf + 0x4 * transform_index);
		}


	}

	free(pMatriciesBuf);
	free(pIndicesBuf);
	D3DXVECTOR3 vector3 = { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
	return vector3;
}

FVector Threads::GetPosition3(ULONG64& bone)
{
	__m128 result;

	/* _m128 -> Vec4 */
	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	/* Read the transform index for this transform */
	auto index = func->Read<int>(bone + 0x40);

	/* Getting the transform data ptr */
	TransformAccessReadOnly* pTransformAccessReadOnly = new TransformAccessReadOnly[sizeof(TransformAccessReadOnly)];
	func->Read(bone + 0x38, pTransformAccessReadOnly, sizeof(TransformAccessReadOnly));
	TransformData* transformData = new TransformData[sizeof(transformData)];
	func->Read(pTransformAccessReadOnly->pTransformData + 0x18, transformData, sizeof(TransformData));


	/* Set the size of the matricies & indicies buffers */
	std::size_t size_matricies_buffer = sizeof(Matrix34) * index + sizeof(Matrix34);
	std::size_t size_indices_buffer = sizeof(int) * index + sizeof(int);

	/* Allocate memory for the matricies & indicies buffers */
	void* pMatriciesBuf = malloc(size_matricies_buffer);
	void* pIndicesBuf = malloc(size_indices_buffer);

	/* count position with some magic mathematic*/
	if (pMatriciesBuf && pIndicesBuf)
	{

		/* read matricies data to buffer, copy to allocated memory */
		func->Read(transformData->pTransformArray, pMatriciesBuf, size_matricies_buffer);

		/* read indicies data to buffer, copy to allocated memory */
		func->Read(transformData->pTransformIndices, pIndicesBuf, size_indices_buffer);

		/* some magic mathematic here and downside */
		result = *(__m128*)((uint64_t)pMatriciesBuf + 0x30 * index);
		int transform_index = *(int*)((uint64_t)pIndicesBuf + 0x4 * index);


		for (int i = 0; transform_index >= 0 && i < 30 && transform_index < size_indices_buffer; i++)
		{


			Matrix34 matrix34 = *(Matrix34*)((uint64_t)pMatriciesBuf + 0x30 * transform_index);



			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), _mm_load_ps(&matrix34.vec0.x));




			transform_index = *(int*)((uint64_t)pIndicesBuf + 0x4 * transform_index);


		}


	}
	delete[] pTransformAccessReadOnly;
	delete[] transformData;
	free(pMatriciesBuf);
	free(pIndicesBuf);
	FVector vector3 = { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
	return vector3;
}
struct Matrix3X4
{
	D3DXVECTOR3 vec0;
	D3DXVECTOR3 vec1;
	D3DXVECTOR3 vec2;
	D3DXVECTOR3 vec3;
};
/* this is new transform function for bones, will reduce read attempts on bone-updating*/
D3DXVECTOR3 Threads::GetPosition2(BonesStruct& bone)
{
	__m128 result;

	/* _m128 -> Vec4 */
	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	/* Read the transform index for this transform */
	auto index = func->Read<int>(bone.bonePTR + 0x40);

	/* Getting the transform data ptr */
	TransformAccessReadOnly pTransformAccessReadOnly = func->Read<TransformAccessReadOnly>(bone.bonePTR + 0x38);
	TransformData transformData = func->Read<TransformData>(pTransformAccessReadOnly.pTransformData + 0x18);


	/* Set the size of the matricies & indicies buffers */
	std::size_t size_matricies_buffer = sizeof(Matrix34) * index + sizeof(Matrix34);
	std::size_t size_indices_buffer = sizeof(int) * index + sizeof(int);

	/* Allocate memory for the matricies & indicies buffers */
	void* pMatriciesBuf = malloc(size_matricies_buffer);
	void* pIndicesBuf = malloc(size_indices_buffer);

	/* count position with some magic mathematic*/
	if (pMatriciesBuf && pIndicesBuf) {

		/* read matricies data to buffer, copy to allocated memory */
		func->Read(transformData.pTransformArray, pMatriciesBuf, size_matricies_buffer);

		/* read indicies data to buffer, copy to allocated memory */
		func->Read(transformData.pTransformIndices, pIndicesBuf, size_indices_buffer);

		/* some magic mathematic here and downside */
		result = *(__m128*)((uint64_t)pMatriciesBuf + 0x30 * index);
		int transform_index = *(int*)((uint64_t)pIndicesBuf + 0x4 * index);


		for (int i = 0; transform_index >= 0 && i < 30 && transform_index < size_indices_buffer; i++)
		{
			Matrix34 matrix34;

			matrix34 = *(Matrix34*)((uint64_t)pMatriciesBuf + 0x30 * transform_index);



			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), _mm_load_ps(&matrix34.vec0.x));



			transform_index = *(int*)((uint64_t)pIndicesBuf + 0x4 * transform_index);
		}


	}

	free(pMatriciesBuf);
	free(pIndicesBuf);
	D3DXVECTOR3 vector3 = { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
	return vector3;
	
}

FVector Threads::GetPosition4(BonesStruct& bone)
{
	__m128 result;

	/* _m128 -> Vec4 */
	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };



	/* Getting the transform data ptr */


	if (bone.transformAccess.index == 0 && bone.transformAccess.pTransformData == 0)
	{
		func->Read(bone.bonePTR + 0x38, &bone.transformAccess, sizeof(TransformAccessReadOnly));
		func->Read(bone.transformAccess.pTransformData + 0x18, &bone.transformData, sizeof(TransformData));
	}




	/* Set the size of the matricies & indicies buffers */
	std::size_t size_matricies_buffer = sizeof(Matrix34) * bone.transformAccess.index + sizeof(Matrix34);
	std::size_t size_indices_buffer = sizeof(int) * bone.transformAccess.index + sizeof(int);

	/* Allocate memory for the matricies & indicies buffers */
	void* pMatriciesBuf = malloc(size_matricies_buffer);
	void* pIndicesBuf = malloc(size_indices_buffer);

	/* count position with some magic mathematic*/
	if (pMatriciesBuf && pIndicesBuf)
	{

		/* read matricies data to buffer, copy to allocated memory */
		func->Read(bone.transformData.pTransformArray, pMatriciesBuf, size_matricies_buffer);

		/* read indicies data to buffer, copy to allocated memory */
		func->Read(bone.transformData.pTransformIndices, pIndicesBuf, size_indices_buffer);

		/* some magic mathematic here and downside */
		result = *(__m128*)((uint64_t)pMatriciesBuf + 0x30 * bone.transformAccess.index);
		int transform_index = *(int*)((uint64_t)pIndicesBuf + 0x4 * bone.transformAccess.index);


		for (int i = 0; transform_index >= 0 && i < 30 && transform_index < size_indices_buffer; i++)
		{


			Matrix34 matrix34 = *(Matrix34*)((uint64_t)pMatriciesBuf + 0x30 * transform_index);



			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), _mm_load_ps(&matrix34.vec0.x));




			transform_index = *(int*)((uint64_t)pIndicesBuf + 0x4 * transform_index);


		}


	}

	free(pMatriciesBuf);
	free(pIndicesBuf);
	FVector vector3 = { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
	return vector3;
}


FVector getAngle(const FVector& origin, const FVector& dest) {
	FVector diff = origin - dest;
	FVector ret;

	float length = diff.GetLength();
	ret.y = asinf(diff.y / length);
	ret.x = -atan2f(diff.x, -diff.z);

	return ret * radiansToDegrees;
}

float calcFov(const FVector& viewAngle, const FVector& aimAngle) {
	FVector diff = viewAngle - aimAngle;
	if (diff.x < -180.f)
		diff.x += 360.f;
	if (diff.x > 180.f)
		diff.x -= 360.f;
	return fabsf(diff.GetLength());
}

uint64_t matrix_list_base1;
uint64_t dependency_index_table_base1;
FVector Threads::GetPositionaim(uint64_t transform)
{
	auto transform_internal = func->Read<uint64_t>(transform + 0x10);

	auto matrices = func->Read<uint64_t>(transform_internal + 0x38);
	auto index = func->Read<int>(transform_internal + 0x40);

	func->Read((matrices + 0x18), &matrix_list_base1, sizeof(matrix_list_base1));

	func->Read((matrices + 0x20), &dependency_index_table_base1, sizeof(dependency_index_table_base1));

	static auto get_dependency_index = [this](uint64_t base, int32_t index)
	{
		func->Read((base + index * 4), &index, sizeof(index));
		return index;
	};

	static auto get_matrix_blob = [this](uint64_t base, uint64_t offs, float* blob, uint32_t size) {
		func->Read((base + offs), blob, size);
	};

	int32_t index_relation = get_dependency_index(dependency_index_table_base1, index);

	FVector ret_value;
	{
		float* base_matrix3x4 = (float*)malloc(64),
			* matrix3x4_buffer0 = (float*)((uint64_t)base_matrix3x4 + 16),
			* matrix3x4_buffer1 = (float*)((uint64_t)base_matrix3x4 + 32),
			* matrix3x4_buffer2 = (float*)((uint64_t)base_matrix3x4 + 48);

		get_matrix_blob(matrix_list_base1, index * 48, base_matrix3x4, 16);

		__m128 xmmword_1410D1340 = { -2.f, 2.f, -2.f, 0.f };
		__m128 xmmword_1410D1350 = { 2.f, -2.f, -2.f, 0.f };
		__m128 xmmword_1410D1360 = { -2.f, -2.f, 2.f, 0.f };

		while (index_relation >= 0)
		{
			uint32_t matrix_relation_index = 6 * index_relation;

			// paziuret kur tik 3 nureadina, ten translationas, kur 4 = quatas ir yra rotationas.
			get_matrix_blob(matrix_list_base1, 8 * matrix_relation_index, matrix3x4_buffer2, 16);
			__m128 v_0 = *(__m128*)matrix3x4_buffer2;

			get_matrix_blob(matrix_list_base1, 8 * matrix_relation_index + 32, matrix3x4_buffer0, 16);
			__m128 v_1 = *(__m128*)matrix3x4_buffer0;

			get_matrix_blob(matrix_list_base1, 8 * matrix_relation_index + 16, matrix3x4_buffer1, 16);
			__m128i v9 = *(__m128i*)matrix3x4_buffer1;

			__m128* v3 = (__m128*)base_matrix3x4; // r10@1
			__m128 v10; // xmm9@2
			__m128 v11; // xmm3@2
			__m128 v12; // xmm8@2
			__m128 v13; // xmm4@2
			__m128 v14; // xmm2@2
			__m128 v15; // xmm5@2
			__m128 v16; // xmm6@2
			__m128 v17; // xmm7@2

			v10 = _mm_mul_ps(v_1, *v3);
			v11 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 0));
			v12 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 85));
			v13 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -114));
			v14 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -37));
			v15 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -86));
			v16 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 113));

			v17 = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(v11, (__m128)xmmword_1410D1350), v13),
								_mm_mul_ps(_mm_mul_ps(v12, (__m128)xmmword_1410D1360), v14)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), -86))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(v15, (__m128)xmmword_1410D1360), v14),
								_mm_mul_ps(_mm_mul_ps(v11, (__m128)xmmword_1410D1340), v16)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 85)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(v12, (__m128)xmmword_1410D1340), v16),
								_mm_mul_ps(_mm_mul_ps(v15, (__m128)xmmword_1410D1350), v13)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 0))),
						v10)),
				v_0);

			*v3 = v17;

			index_relation = get_dependency_index(dependency_index_table_base1, index_relation);
		}

		ret_value = *(FVector*)base_matrix3x4;
		delete[] base_matrix3x4;
	}

	return ret_value;
}
/* getting info about new players in list */
Threads::DiscoveredPlayer Threads::updatePlayer(ULONG64& entity)
{
	Threads::DiscoveredPlayer discoveredPlayer;
	discoveredPlayer.Entity = entity;

	/* create EFT_Player struct */
	EFT_Player* player = new EFT_Player[sizeof(EFT_Player)];

	/* create Player profile struct */
	PlayerProfile* playerProfile = new PlayerProfile[sizeof(PlayerProfile)];

	/* create playerInfo struct */
	PlayerInfo* playerInfo = new PlayerInfo[sizeof(PlayerInfo)];

	/* read all data to created structs above */
	func->Read(entity, player, sizeof(EFT_Player));
	func->Read(player->PlayerProfile, playerProfile, sizeof(PlayerProfile));
	func->Read(playerProfile->PlayerInfo, playerInfo, sizeof(PlayerInfo));

	/* get bones adresses */
	std::vector<ULONG64> bonesChain{ this->SkeletonRootJoin, this->BoneEnumerator, this->TransformArray };
	ULONG64 allBonesPTR = func->read_chain(player->PlayerBody, { bonesChain });

	BOOL local = func->Readbool(entity + 0x7F3);

	if (local)
	{
		this->localPlayerPtr = entity;
		// get localPlayer HeadPTR
		BonesStruct bone; bone.bonePTR = func->read_chain(allBonesPTR, { ULONG64(0x20 + (92 * 0x8)), 0x10 });

		//get bone's transform's data
		if (bone.transformAccess.index == 0 && bone.transformAccess.pTransformData == 0)
		{
			func->Read(bone.bonePTR + 0x38, &bone.transformAccess, sizeof(TransformAccessReadOnly));
			func->Read(bone.transformAccess.pTransformData + 0x18, &bone.transformData, sizeof(TransformData));
		}
		discoveredPlayer.bones.push_back(bone);
		discoveredPlayer.local = true;
		delete[] player; delete[] playerProfile; delete[] playerInfo;
		return discoveredPlayer;
	}

	/* special numbers for bones in EFT memory
	* i give no shit how exactly they counted in EFT xD */
	int bone_array[] = { 113, 112, 111, 132, 90, 91, 92, 133, 37, 36, 29, 14, 22, 23, 17,18 };
	for (int i : bone_array)
	{
		// get player bonePtr
		BonesStruct bone; bone.bonePTR = func->read_chain(allBonesPTR, { ULONG64(0x20 + (i * 0x8)), 0x10 });

		//get bone's transform's data
		if (bone.transformAccess.index == 0 && bone.transformAccess.pTransformData == 0)
		{
			func->Read(bone.bonePTR + 0x38, &bone.transformAccess, sizeof(TransformAccessReadOnly));
			func->Read(bone.transformAccess.pTransformData + 0x18, &bone.transformData, sizeof(TransformData));
		}
		discoveredPlayer.bones.push_back(bone);
	}

	/* =============================== */
	/* add inventory read here */
	/* =============================== */


	/* checking if it's real player or bot
	* if not - grabbing botRole */
	if (playerInfo->RegistrationDate <= 0)
	{
		// it's a bot!
		discoveredPlayer.Side = func->Read<int>(playerInfo->RE_Settings + 0x10);
	}
	else
	{
		// it's a player! 
		// checking if it's BEAR or USEC 
		if (playerInfo->Side == 1 || playerInfo->Side == 2)
		{
			// using custom value, to not create new variable in class 
			if (playerInfo->Side == 1)
				discoveredPlayer.Side = 55;
			else
				discoveredPlayer.Side = 56;

			// get level
			for (size_t currentlevel = 0; currentlevel < 71; currentlevel++)
			{
				auto currentLevelExp = tablePlayerExperience[currentlevel];

				if (playerInfo->Experience < currentLevelExp)
				{
					discoveredPlayer.Level = currentlevel - 1;
					break;
				}
			}

			// get group ID 
			discoveredPlayer.GroupId = func->readString(playerInfo->UnityEngineString_GroupId + 0x14);
			int nickSize = func->Read<int>(playerInfo->UnityEngineString_Nickname + 0x10);
			// get nickname 
			discoveredPlayer.NickName = func->GetUnicodeString(playerInfo->UnityEngineString_Nickname + 0x14, nickSize);

			/* get KDa */
			/* =============================== */
			/* UNFINISHED SHIT I HATE THIS KD RATIO FUCK THIS FUCK */
			PlayerStats* playerStats = new PlayerStats[sizeof(PlayerStats)];
			func->Read(playerProfile->Stats, playerStats, sizeof(PlayerStats));
			delete[] playerStats;
			/* count KDa ends */
			/* =============================== */


		}
		/* it's a player Scav! we don't need any shit above */
		else if (playerInfo->Side == 4)
		{
			discoveredPlayer.Side = 4;
			/* get group ID*/
			discoveredPlayer.GroupId = func->readString(playerInfo->UnityEngineString_GroupId + 0x14);
		}
	}
	delete[] player; delete[] playerProfile; delete[] playerInfo;
	return discoveredPlayer;
}
void Threads::GetPlayers2(ULONG64 oprl, ULONG64 viewMatrixPtr, ULONG64 opticptr)
{
	//set delay
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	// read current playerThread
	this->playerCount = func->Read<int>(data.playerListPtr + 0x18);

	// stupid check, does playerCount has right value
	if (this->playerCount > 0 && this->playerCount < 50)
	{
		this->online = true;
	}

	// playerCount changed (+1 player, or -1 player, or we just started )
	if (this->playerCount != data.playerCount)
	{
		/* allocating storage for players ptr */
		ULONG64* entityBuffer = new ULONG64[this->playerCount * sizeof(ULONG64)];

		/* grab all entities with readBuffer */
		func->Read(data.playerListArray + offsets->firstPlayer, entityBuffer, this->playerCount * sizeof(ULONG64));

		/* store all data in vector of ULONG64 */
		std::vector<ULONG64> foundPlayers = std::vector<ULONG64>(entityBuffer, entityBuffer + this->playerCount);

		/* stop memory leak  */
		delete[] entityBuffer;

		// if we already have some players in vector == we search for changes
		if (players.vector.size() != 0)
		{
			// players decreasedw
			if (this->playerCount < data.playerCount)
			{
				for (auto x = 0; x < players.vector.size();)
				{
					if (std::find(foundPlayers.begin(), foundPlayers.end(), players.vector[x].Entity) != foundPlayers.end())
					{
						x++;
						continue;
					}
					else
					{
						players.N.lock();
						players.M.lock();
						players.N.unlock();
						players.vector.erase(players.vector.begin() + x);
						players.M.unlock();
						continue;
					}
				}
			}

			//players increased, adding new player
			if (this->playerCount > data.playerCount)
			{
				for (auto x = 0; x < foundPlayers.size(); x++)
				{
					if (std::find(players.vector.begin(), players.vector.end(), foundPlayers[x]) != players.vector.end())
					{
						continue;
					}
					else
					{
						players.N.lock();
						players.M.lock();
						players.N.unlock();
						players.vector.resize(players.vector.size() + 1);
						players.vector[players.vector.size() - 1] = this->updatePlayer(foundPlayers[x]);
						players.M.unlock();
						continue;
					}
				}
			}
		}
		else
		{
			// we have empty players storage = collect data on start
			for (auto x = 0; x < foundPlayers.size(); x++)
			{
				players.N.lock();
				players.M.lock();
				players.N.unlock();
				players.vector.push_back(this->updatePlayer(foundPlayers[x]));
				players.M.unlock();
			}
		}
		data.playerCount = this->playerCount;
	}
	else
	{
		// updating current bone-pos for each player;

		std::vector<DrawPlayer> temporaryDrawStorage;

		for (auto ent : players.vector)
		{
			/* collect all data to send it for draw-cycle */
			DrawPlayer player;
			if (ent.local == true)
			{
				this->localPlayerHead = this->GetPosition2(ent.bones[0]);
				player.local = ent.local;
				player.bonesVectors = ent.bonesVectors;
				temporaryDrawStorage.push_back(player);
				continue;
			}

			player.side = ent.Side;
			player.GroupId = ent.GroupId;
			player.level = ent.Level;
			player.NickName = ent.NickName;

			/* update bones */
			/* getPosition2 will do ONLY calculations, since internal data already saved in ent.bone structure with threads::updatePlayer function */
			player.distance = func->CalculateDistance(this->GetPosition2(ent.bones[7]), this->localPlayerHead);
			if (player.distance > 300)
				continue;
			for (auto bone : ent.bones)
			{
				player.bonesVectors.push_back(this->WorldToScreen(this->GetPosition2(bone), oprl, opticptr, viewMatrixPtr));
			}
			temporaryDrawStorage.push_back(player);
		}
		data.playerCount = this->playerCount;
		globals->drawingPlayers.N.lock();
		globals->drawingPlayers.M.lock();
		globals->drawingPlayers.N.unlock();
		globals->drawingPlayers.vector = temporaryDrawStorage;
		globals->drawingPlayers.M.unlock();
	}
}



void Threads::AngleWriter()
{ 
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	if (this->playerCount == 0)
		return;
	if (!isAiming(this->localPlayerPtr))
		return;
	std::vector<DiscoveredPlayer> aimPlayer;

	// locking all players data with low-priority, to dublicate it's storage inside current thread
	players.L.lock();
	players.N.lock();
	players.M.lock();
	players.N.unlock();
	aimPlayer = players.vector;
	players.M.unlock();
	players.L.unlock();

	FVector localView;
	uint64_t view1 = func->Read<uint64_t>(this->localPlayerPtr + 0x40);
	Vector2 view = func->Readvec2(view1 + 0x22C);
	localView.x = view.x;
	localView.y = view.y;
	localView.z = 0.f;
	float fov;
	float bestFov = 500.0f;
	FVector rotationAngle;
	DiscoveredPlayer aimtg;
	
	for (DiscoveredPlayer ent : aimPlayer)
	{
		if (ent.local == true)
			continue;
		auto transform = func->read_chain(this->localPlayerPtr, { 0x500, 0x20 });
		auto abcd = this->GetPositionaim(transform);
		auto PBONE = this->GetPosition4(ent.bones[7]);
		FVector angle = getAngle(abcd, PBONE);
		fov = calcFov(localView, angle);
		if (func->CalculateDistance(this->localPlayerHead, this->GetPosition2(ent.bones[7])) > 100)
			continue;
		/*if (fov < bestFov) { //fov 가까운 순
			bestFov = fov;
			rotationAngle = angle;
		}*/

		if (fov > 4.f)
			continue;

		if (aimtg.Entity == 0) { //거리순
			aimtg = ent;
		}
		if (func->CalculateDistance(this->localPlayerHead, this->GetPosition2(aimtg.bones[7])) > func->CalculateDistance(this->localPlayerHead, this->GetPosition2(ent.bones[7]))) {
			aimtg = ent;
		}
		auto PBONEreal = this->GetPosition4(aimtg.bones[7]);
		FVector realangle = getAngle(abcd, PBONEreal);
		rotationAngle = realangle;

	}
	if (rotationAngle.x != 0 && rotationAngle.y != 0) {
		func->Writevec(view1 + 0x22C, { rotationAngle.x, rotationAngle.y });
	}

}



void Threads::UpdateItems2()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	if (this->online == false)
		return;

	this->itemsCount = func->Read<int>(data.itemListPtr + 0x18);
	if (this->itemsCount == data.itemsCount)
	{
		std::vector<DrawItem> drawStorage;

		for (auto ent : items.vector)
		{
			DrawItem item;
			item.itemPosition = ent.positionInWorld;
			if (ent.container == true)
			{
				int itemCount = func->Read<int>(ent.containerGrid + 0x40);
				ULONG64 Entries = func->Read<ULONG64>(ent.containerGrid + 0x18);
				ent.itemsInContainer = updateContainerItem(Entries, itemCount);

				item.isContainer = ent.container;
				item.itemsInContainer = ent.itemsInContainer;
			}
			else
			{
				item.itemName = ent.itemName;
				item.price = ent.price;
			}
			drawStorage.push_back(item);
		}
		globals->drawingItems.N.lock();
		globals->drawingItems.M.lock();
		globals->drawingItems.N.unlock();
		globals->drawingItems.vector = drawStorage;
		globals->drawingItems.M.unlock();
		return;
	}


	// updating items 
	if (this->itemsCount > data.itemsCount && data.itemsCount != 0)
	{
		ULONG64* entityBuffer = new ULONG64[this->itemsCount * sizeof(ULONG64)];
		/* grab all entities with readBuffer */
		func->Read(data.itemListArray + offsets->firstItem, entityBuffer, this->itemsCount * sizeof(ULONG64));
		/* store all data in vector of ULONG64 */
		std::vector<ULONG64> foundItems = std::vector<ULONG64>(entityBuffer, entityBuffer + this->itemsCount);
		delete[] entityBuffer;

		for (auto x = 0; x < foundItems.size(); x++)
		{
			if (std::find(items.vector.begin(), items.vector.end(), foundItems[x]) != items.vector.end())
			{
				continue;
			}
			else
			{
				items.M.lock();
				items.vector.resize(items.vector.size() + 1);
				items.vector[items.vector.size() - 1] = updateItem(foundItems[x]);
				items.M.unlock();
				continue;
			}
		}
		data.itemsCount = this->itemsCount;
	}
	else if (this->itemsCount == data.itemsCount && data.itemsCount != 0)
		return;
	else if (data.itemsCount > this->itemsCount && data.itemsCount != 0)
	{
		ULONG64* entityBuffer = new ULONG64[this->itemsCount * sizeof(ULONG64)];
		/* grab all entities with readBuffer */
		func->Read(data.itemListArray + offsets->firstItem, entityBuffer, this->itemsCount * sizeof(ULONG64));
		/* store all data in vector of ULONG64 */
		std::vector<ULONG64> foundItems = std::vector<ULONG64>(entityBuffer, entityBuffer + this->itemsCount);
		delete[] entityBuffer;
		for (auto x = 0; x < items.vector.size();)
		{

			if (std::find(foundItems.begin(), foundItems.end(), items.vector[x].Entity) != foundItems.end())
			{
				x++;
				continue;
			}
			else
			{
				items.M.lock();
				items.vector.erase(items.vector.begin() + x);
				items.M.unlock();
				continue;
			}
		}
		data.itemsCount = this->itemsCount;
	}


	//collecting all items on start
	if (data.itemsCount == 0) 
	{
		std::vector<DiscoveredItem> temporaryStorage;
		ULONG64* entityBuffer = new ULONG64[this->itemsCount * sizeof(ULONG64)];
		/* grab all entities with readBuffer */
		func->Read(data.itemListArray + offsets->firstItem, entityBuffer, this->itemsCount * sizeof(ULONG64));
		/* store all data in vector of ULONG64 */
		std::vector<ULONG64> foundItems = std::vector<ULONG64>(entityBuffer, entityBuffer + this->itemsCount);
		delete[] entityBuffer;

		for (ULONG64 ent : foundItems)
		{
			temporaryStorage.push_back(updateItem(ent));
		}
		data.itemsCount = this->itemsCount;
		items.M.lock();
		items.vector = temporaryStorage;
		items.M.unlock();
	}
}

void Threads::ButtonCatcher()
{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (visorPtr != 0) 
		{
			float intensity = func->ReadFloat(visorPtr + 0xb8);
			if (intensity != 0.000f)
			{
				visorStatus = false;
			}
		}
		if (visorStatus == false)
		{
			if (visorPtr == 0)
				// grab VisorPtr;	
			{
				auto compList = func->read_chain(data.cameraModule, { 0x30, 0x30 });

				ULONG64 Visor = 0;

				for (int i = 0x8; i < 0x300; i = i + 0x10)
				{
					ULONG64 fields = func->Read<ULONG64>(compList + i);
					ULONG64 fields_ = func->Read<ULONG64>(fields + 0x28);
					ULONG64 temp = func->read_chain(fields_, { 0x0, 0x0, 0x48 });
					auto objectName = func->readString(temp);
					if (objectName == "VisorEffect")
					{
						visorPtr = fields_;
						break;
					}
				}				
			}	
			
			// set visor status		
			func->WriteFloat(visorPtr + 0xB8, 0.000f);
			visorStatus = true;			
		}

		if (auto physical = func->Read<uint64_t>(this->localPlayerPtr + 0x4C0)) {
			if (auto stamina = func->Read<uint64_t>(physical + 0x38)) {
				func->WriteFloat(stamina + 0x48, 92.f);
			}
		}
		if (auto physical = func->Read<uint64_t>(this->localPlayerPtr + 0x4C0)) {
			if (auto stamina = func->Read<uint64_t>(physical + 0x40)) {
				func->WriteFloat(stamina + 0x48, 92.f);
			}
		}
		if (auto physical = func->Read<uint64_t>(this->localPlayerPtr + 0x4C0)) {
			if (auto stamina = func->Read<uint64_t>(physical + 0x48)) {
				func->WriteFloat(stamina + 0x48, 92.f);
			}
		}
		if (this->localPlayerPtr)
		{
			auto animation = func->Read<uint64_t>(this->localPlayerPtr + 0x190);
			auto fire = func->Read<uint64_t>(animation + 0x80);

			uint64_t breath = animation + 0x28;
			breath = func->Read<uint64_t>(breath);
			func->WriteFloat(breath + 0xA4, 0.f);//breath intensity

			uint64_t mask = animation + 0x100;
			func->Write32t(mask, 1);

			uint64_t ads = animation + 0x198;
			func->WriteFloat(ads, 10.f);//ads intensity

			uint64_t alignToZeroFloat22 = animation + 0x230;
			func->WriteFloat(alignToZeroFloat22, 0.f);

			uint64_t mask1 = animation + 0x2F8;
			func->WriteFloat(mask1, 0.f);

			//func->WriteFloat(fire + 0x168, 5.f);
			//func->WriteFloat(fire + 0x15C, 0.f);
			//func->WriteFloat(fire + 0x164, 0.f);
		}

		/* end of no recoil */

		/*auto playerProfile = func->Read<ULONG64>(this->localPlayerPtr + 0x458);
		auto playerSkills = func->Read<ULONG64>(playerProfile + 0x60);


		auto EnduranceBuffEnduranceInc = func->Read<ULONG64>( playerSkills + 0x18);		 // Elite increase Stamina 1.5 max
		func->WriteBool(EnduranceBuffEnduranceInc + 0x20, true);
		func->WriteFloat(EnduranceBuffEnduranceInc + 0x28, 1.5f);

		auto EnduranceHands = func->Read<ULONG64>(playerSkills + 0x20);					 // Elite hands Stamina 0.5 max		
		func->WriteFloat(EnduranceHands + 0x28, 0.5f);		


		auto EnduranceBuffJumpCostRed = func->Read<ULONG64>(playerSkills + 0x28);         // reduce jump cost 0.5 max		
		func->WriteFloat(EnduranceBuffJumpCostRed + 0x28, 0.5f);


		auto EnduranceBuffBreathTimeInc = func->Read<ULONG64>(playerSkills + 0x30);		 // breath time while scoping 1.0 max		
		func->WriteFloat(EnduranceBuffBreathTimeInc + 0x28, 1.0f);

		auto EnduranceBuffRestoration = func->Read<ULONG64>(playerSkills + 0x38);			// stamina recover (hidden) 1.0 max		
		func->WriteFloat(EnduranceBuffRestoration + 0x28, 1.5f);

		auto EnduranceBreathElite = func->Read<ULONG64>(playerSkills + 0x40);			// removing apnea breath 1.0 max		
		func->WriteFloat(EnduranceBreathElite + 0x28, 1.0f);*/

		/* some strenght buffs passed cause of server-checking side*/ /* or not passevd xD */

	/*	auto StrengthBuffLiftWeightInc = driver.ReadVirtualMemory<ULONG64>(pid, playerSkills + 0x48, sizeof(ULONG64));
		driver.WriteVirtualMemory<bool>(pid, StrengthBuffLiftWeightInc + 0x20, true, sizeof(true));
		driver.WriteVirtualMemory<float>(pid, StrengthBuffLiftWeightInc + 0x28, 1.5f, sizeof(1.5f));
		Sleep(1);*/
	/*	auto StrengthBuffSprintSpeedInc = func->Read<ULONG64>(playerSkills + 0x50);
		func->WriteFloat(StrengthBuffSprintSpeedInc + 0x28, 1.0f);

		auto StrengthBuffJumpHeightInc = func->Read<ULONG64>(playerSkills + 0x58);		
		func->WriteFloat(StrengthBuffJumpHeightInc + 0x28, 0.2f);


		auto StrengthBuffAimFatigue = func->Read<ULONG64>( playerSkills + 0x60);        // drain hand stamina while scoping 0.2 max		
		func->WriteFloat(StrengthBuffAimFatigue + 0x28, 0.2f);

		auto StrengthBuffElite = func->Read<ULONG64>(playerSkills + 0x78);				// strength elite - decrease weight for using weapons				
		func->WriteFloat(StrengthBuffElite + 0x28, 2.0f);

		auto StressBerserk = func->Read<ULONG64>(playerSkills + 0xE8);					// berserk mode
		func->WriteBool(StressBerserk + 0x20, true);
		func->WriteFloat(StressBerserk + 0x28, 1.0f);

		auto MetabolismEliteBuffNoDyhydration = func->Read<ULONG64>(playerSkills + 0x108);
		func->WriteBool(MetabolismEliteBuffNoDyhydration + 0x20, true);
		func->WriteFloat(MetabolismEliteBuffNoDyhydration + 0x28, 1.0f);

		auto AttentionLootSpeed = func->Read<ULONG64>( playerSkills + 0x150);			// speed search 1.0 max
		func->WriteBool(AttentionLootSpeed + 0x20, true);
		func->WriteFloat(AttentionLootSpeed + 0x28, 1.0f);

		auto AttentionEliteLuckySearch = func->Read<ULONG64>(playerSkills + 0x160);		// elite insta search 0.5 max
		func->WriteBool(AttentionEliteLuckySearch + 0x20, true);
		func->WriteFloat(AttentionEliteLuckySearch + 0x28, 1.0f);

		auto MagDrillsLoadSpeed = func->Read<ULONG64>(playerSkills + 0x170);			// loading bullets speed
		func->WriteBool(MagDrillsLoadSpeed + 0x20, true);
		func->WriteFloat(MagDrillsLoadSpeed + 0x28, 4.0f);


		auto MagDrillsUnloadSpeed = func->Read<ULONG64>(playerSkills + 0x178);			// UNloading bullets speed
		func->WriteBool(MagDrillsUnloadSpeed + 0x20, true);
		func->WriteFloat(MagDrillsUnloadSpeed + 0x28, 4.0f);

		auto SearchBuffSpeed = func->Read<ULONG64>( playerSkills + 0x640);				// speed of inspecting 
		func->WriteBool( SearchBuffSpeed + 0x20, true);
		func->WriteFloat( SearchBuffSpeed + 0x28, 0.5f);

		auto SearchDouble = func->Read<ULONG64>( playerSkills + 0x648);					// double inspecting
		func->WriteBool( SearchDouble + 0x20, true);
		func->WriteFloat( SearchDouble + 0x28, 1.0f);

		auto BotReloadSpeed = func->Read<ULONG64>( playerSkills + 0x708);        // reloading magazines speed		
		func->WriteBool( BotReloadSpeed + 0x20, true);
		func->WriteFloat( BotReloadSpeed + 0x28, 1.3f);

		auto SurgerySpeed = func->Read<ULONG64>(playerSkills + 0x658);
		func->WriteBool( SurgerySpeed + 0x20, true);
		func->WriteFloat( SurgerySpeed + 0x28, 1.5f);

		auto AttentionExamine = func->Read<ULONG64>(playerSkills + 0x158);
		func->WriteBool( AttentionExamine + 0x20, true);
		func->WriteFloat( AttentionExamine + 0x28, 2.0f);*/

}
