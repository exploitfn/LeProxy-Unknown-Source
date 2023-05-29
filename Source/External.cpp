#include "funct.h"
#include "d3d9_x.h"
#include <dwmapi.h>
#include <vector>
#include "skStr.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "D3DX/d3dx9math.h"
#include <thread>
#include <Windows.h>
#include <direct.h>
#include <string>
#include "skStr.h"
#include "Driver.h"
#include "settings.h"
#include <intrin.h>
#include <list>
#include "lib.hpp"
#include "globals.h"
#include "icons.h"
#include "font.h"
#include "utils.h"

#define MENU_TAB_DEFAULT 0
#define MENU_TAB_ASSULTRIFLE 1
#define MENU_TAB_SHOTGUN 2
#define MENU_TAB_SMG 3
#define MENU_TAB_PISTOL 4
#define MENU_TAB_SNIPER 5


RECT GameRect = { NULL };
D3DPRESENT_PARAMETERS d3dpp;
ImFont* fn = nullptr;
ImFont* gta = nullptr;
ImFont* SkeetFont = nullptr;
ImFont* DefaultFont = nullptr;

MSG Message = { NULL };
const MARGINS Margin = { -1 };
HWND game_wnd;
int screen_width;
bool streamproof;
int screen_height;
static void xCreateWindow();
static void xInitD3d();
static void xMainLoop();                     
static void xShutdown();
static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND Window = NULL;
IDirect3D9Ex* p_Object = NULL;                         
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;

int center_x = GetSystemMetrics(0) / 2 - 3;
int center_y = GetSystemMetrics(1) / 2 - 3;

DWORD UDPID;

int AimFOV = 180;


namespace settings
{
	DWORD_PTR Uworld;
	DWORD_PTR LocalPawn;
	DWORD_PTR PlayerState;
	DWORD_PTR Localplayer;
	DWORD_PTR Rootcomp;
	DWORD_PTR PlayerController;
	DWORD_PTR Persistentlevel;
	DWORD_PTR Gameinstance;
	DWORD_PTR LocalPlayers;
	uint64_t PlayerCameraManager;
	uint64_t WorldSettings;
	Vector3 localactorpos;
	Vector3 relativelocation;
	DWORD_PTR AActors;
	DWORD ActorCount;
}

//STRUCTURES
typedef struct _TT {
	uint64_t Actor;
	int ID;
	uint64_t mesh;
	uint64_t root;
	uint64_t PlayerState;
	uint64_t LocalPlayerState;
	uint32_t LocalTeamID;
	uint32_t TeamID69;
	Vector3 pos;
	std::string name;
	bool Spotted;
}TT;

typedef struct _FNlEntity {
	uint64_t Actor;
	uint64_t mesh;
	int id;
}FNlEntity;

struct FVector {
	double X; // 0x00(0x08)
	double Y; // 0x08(0x08)
	double Z; // 0x10(0x08)
};

// Size: 0x18 (Inherited: 0x00)
struct FRotator {
	double Pitch; // 0x00(0x08)
	double Yaw; // 0x08(0x08)
	double Roll; // 0x10(0x08)
};

// Size: 0x770 (Inherited: 0x00)
struct FMinimalViewInfo {
	struct Vector3 Location; // 0x00(0x18)
	struct Vector3 Rotation; // 0x18(0x18)
	float FOV; // 0x30(0x04)
	float DesiredFOV; // 0x34(0x04)
	float OrthoWidth; // 0x38(0x04)
	float OrthoNearClipPlane; // 0x3c(0x04)
	float OrthoFarClipPlane; // 0x40(0x04)
	float AspectRatio; // 0x44(0x04)
	char bConstrainAspectRatio : 1; // 0x48(0x01)
	char bUseFieldOfViewForLOD : 1; // 0x48(0x01)
	char pad_48_2 : 6; // 0x48(0x01)
	char pad_49[0x3]; // 0x49(0x03)
	char pad_4D[0x3]; // 0x4d(0x03)
	float PostProcessBlendWeight; // 0x50(0x04)
	char pad_54[0xc]; // 0x54(0x0c)
	char pad_700[0x70]; // 0x700(0x70)
};

// ScriptStruct FortniteGame.FortPlayerAttributeSets
// Size: 0x50 (Inherited: 0x00)
struct FFortPlayerAttributeSets {
	struct UFortRegenHealthSet* HealthSet; // 0x00(0x08)
	struct UFortControlResistanceSet* ControlResistanceSet; // 0x08(0x08)
	struct UFortDamageSet* DamageSet; // 0x10(0x08)
	struct UFortMovementSet* MovementSet; // 0x18(0x08)
	struct UFortAdvancedMovementSet* AdvancedMovementSet; // 0x20(0x08)
	struct UFortConstructionSet* ConstructionSet; // 0x28(0x08)
	struct UFortPlayerAttrSet* PlayerAttrSet; // 0x30(0x08)
	struct UFortCharacterAttrSet* CharacterAttrSet; // 0x38(0x08)
	struct UFortWeaponAttrSet* WeaponAttrSet; // 0x40(0x08)
	struct UFortHomebaseSet* HomebaseSet; // 0x48(0x08)
};

typedef struct _LootEntity {
	std::string GNames;
	uintptr_t ACurrentItem;
}LootEntity;

// ScriptStruct FortniteGame.FortPlayerPawnStats
// Size: 0xd8 (Inherited: 0xc0)
struct FFortPlayerPawnStats {
	float MaxJumpTime; // 0xc0(0x04)
	float MaxStamina; // 0xc4(0x04)
	float StaminaRegenRate; // 0xc8(0x04)
	float StaminaRegenDelay; // 0xcc(0x04)
	float SprintingStaminaExpenditureRate; // 0xd0(0x04)
};

struct Camera
{
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
}; Camera vCamera;

// ScriptStruct Engine.TViewTarget
// Size: 0x790 (Inherited: 0x00)
struct FTViewTarget {
	struct FMinimalViewInfo POV; // 0x10(0x770)
};

FRotator* Rotator(FQuat* Current, struct FRotator* retstr)
{
	float Z; // xmm4_4
	float Y; // xmm0_4
	float W; // xmm2_4
	float v7; // xmm5_4
	float v8; // xmm7_4
	float v9; // xmm8_4
	float v10; // xmm0_4
	float v11; // xmm1_4
	float v12; // xmm7_4
	float X; // xmm0_4
	float v14; // xmm0_4
	float v15; // xmm0_4
	float v16; // xmm0_4
	float v17; // xmm1_4
	float v18; // xmm5_4
	float v19; // xmm2_4
	float v20; // xmm4_4
	float v21; // xmm0_4
	float v22; // xmm0_4
	float v23; // xmm2_4
	float v24; // xmm4_4

	Z = Current->z;
	Y = Current->y;
	W = Current->w;
	v7 = (float)(Current->x * Z) - (float)(Y * W);
	v8 = (float)((float)(Y * Current->x) + (float)(Y * Current->x)) + (float)((float)(W * Z) + (float)(W * Z));
	v9 = 1.0 - (float)((float)((float)(Y * Y) + (float)(Y * Y)) + (float)((float)(Z * Z) + (float)(Z * Z)));
	if (v7 >= -0.49999949)
	{
		if (v7 <= 0.49999949)
		{
			v18 = v7 + v7;
			v19 = v18;
			v20 = sqrtf(fmaxf(1.0 - v19, 0.0))
				* (float)((float)((float)((float)((float)((float)((float)((float)((float)((float)((float)((float)((float)(0.0066700899 - (float)(v19 * 0.0012624911)) * v19)
					- 0.017088126)
					* v19)
					+ 0.03089188)
					* v19)
					- 0.050174303)
					* v19)
					+ 0.088978991)
					* v19)
					- 0.2145988)
					* v19)
					+ 1.5707963);
			if (v18 < 0.0)
				v21 = v20 - 1.5707963;
			else
				v21 = 1.5707963 - v20;
			retstr->Pitch = v21 * 57.295776;
			v22 = atan2(v8, v9);
			v23 = Current->x * Current->x;
			v24 = Current->x * Current->w;
			retstr->Yaw = v22 * 57.295776;
			v15 = atan2(
				(float)((float)(Current->y * Current->z) * -2.0) - (float)(v24 + v24),
				1.0 - (float)((float)(v23 + v23) + (float)((float)(Current->y * Current->y) + (float)(Current->y * Current->y))))
				* 57.295776;
			goto LABEL_13;
		}
		retstr->Pitch = 90.0;
		v16 = atan2(v8, v9);
		v17 = Current->w;
		retstr->Yaw = v16 * 57.295776;
		v14 = retstr->Yaw - (float)((float)(atan2(Current->x, v17) * 2.0) * 57.295776);
	}
	else
	{
		retstr->Pitch = -90.0;
		v10 = atan2(v8, v9);
		v11 = Current->w;
		v12 = v10 * 57.295776;
		X = Current->x;
		retstr->Yaw = v12;
		//v14 = COERCE_FLOAT(LODWORD(v12) ^ _xmm) - (float)((float)(FGenericPlatformMath::Atan2(X, v11) * 2.0) * 57.295776);
	}
LABEL_13:
	retstr->Roll = 0;
	return retstr;
}

struct FFortGameplayAttributeData {
	float Minimum; // 0x10(0x04)
	float Maximum; // 0x14(0x04)
	bool bIsCurrentClamped; // 0x18(0x01)
	bool bIsBaseClamped; // 0x19(0x01)
	bool bShouldClampBase; // 0x1a(0x01)
	char pad_1B[0x1]; // 0x1b(0x01)
	float UnclampedBaseValue; // 0x1c(0x04)
	float UnclampedCurrentValue; // 0x20(0x04)
	char pad_24[0x4]; // 0x24(0x04)
};

// Class FortniteGame.AthenaBigBaseWall
// Size: 0xa58 (Inherited: 0x9c8)
struct AAthenaBigBaseWall{
	float WallGravity; // 0x9c8(0x04)
	float TimeUntilWallComesDown; // 0x9cc(0x04)
	bool bResetBool; // 0x9d0(0x01)
	//enum class EBarrierState BarrierState; // 0x9d1(0x01)
	char pad_9D2[0x6]; // 0x9d2(0x06)
	//struct FScalableFloat ZKillLevel; // 0x9d8(0x28)
	char pad_A00[0x58]; // 0xa00(0x58)

	void WallStartingToComeDown(bool bIsOnBurgerSide); // Function FortniteGame.AthenaBigBaseWall.WallStartingToComeDown // (Event|Protected|BlueprintEvent) // @ game+0x1036c24
	void UpdateWallTime(float, float, float, float); // Function FortniteGame.AthenaBigBaseWall.UpdateWallTime // (Event|Protected|BlueprintEvent) // @ game+0x1036c24
	void ShowOrHideTimer(bool bNewVisibleState); // Function FortniteGame.AthenaBigBaseWall.ShowOrHideTimer // (Event|Protected|BlueprintEvent) // @ game+0x1036c24
	void OnRep_WallGravity(); // Function FortniteGame.AthenaBigBaseWall.OnRep_WallGravity // (Final|Native|Protected) // @ game+0x78b3450
	void OnRep_TimeUntilWallComesDown(); // Function FortniteGame.AthenaBigBaseWall.OnRep_TimeUntilWallComesDown // (Final|Native|Protected) // @ game+0x78b32cc
	void OnRep_ResetBool(); // Function FortniteGame.AthenaBigBaseWall.OnRep_ResetBool // (Final|Native|Protected) // @ game+0x24b6bc4
	void OnRep_BarrierState(); // Function FortniteGame.AthenaBigBaseWall.OnRep_BarrierState // (Final|Native|Protected) // @ game+0x78b2e88
	void OnNewBarrierState(enum class EBarrierState NewState); // Function FortniteGame.AthenaBigBaseWall.OnNewBarrierState // (Event|Protected|BlueprintEvent) // @ game+0x1036c24
};


D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

D3DMATRIX matrixx(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}


bool IsVec3Valid(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}

bool IsInScreen(Vector3 pos, int over = 30) {
	if (pos.x > -over && pos.x < Width + over && pos.y > -over && pos.y < Height + over) {
		return true;
	}
	else {
		return false;
	}
}

FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	__int64 rdi, rdx, rcx;
	rdi = mesh;
	rdx = Drv.read<int>(rdi + 0x6D0);
	rcx = Drv.read<__int64>(rdi + 0x688 + (rdx * 0x10));
	return Drv.read<FTransform>(rcx + (0x30 * index));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = Drv.read<FTransform>(mesh + 0x180);

	D3DMATRIX Matrix;
	Matrix = matrixx(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

bool isVisible(uint64_t mesh)
{
	float tik = Drv.read<float>(mesh + 0x358);
	float tok = Drv.read<float>(mesh + 0x360);
	const float tick = 0.06f;
	return tok + tick >= tik;
}

Camera GetCamera(__int64 LocalRoot)
{
	Camera VirtualCamera;
	int64 v1;
	int64 v6;
	int64 v7;
	int64 v8;

	v1 = Drv.read<int64>(settings::Localplayer + 0xC8);
	int64 v9 = Drv.read<int64>(v1 + 0x8);

	//if (niggerfovchanger)
	//{
	//	VirtualCamera.FieldOfView = FOVChangerValue;
	//}
	//else
	//{
	VirtualCamera.FieldOfView = 80.0f / (Drv.read<double>(v9 + 0x7F0) / 1.19f);
	VirtualCamera.FieldOfView = Drv.read<float>(settings::Localplayer + 0x5C4);
	/*}*/

	VirtualCamera.Rotation.x = Drv.read<float>(v9 + 0x6a8);
	VirtualCamera.Rotation.y = Drv.read<float>(LocalRoot + 0x16C);

	uint64_t pointer_lol = Drv.read<uint64_t>(settings::Uworld + 0x120);
	VirtualCamera.Location = Drv.read<Vector3>(pointer_lol);

	return VirtualCamera;
}

Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	Camera vCamera = GetCamera(settings::Rootcomp);

	vCamera.Rotation.x = (asin(vCamera.Rotation.x)) * (180.0 / M_PI);

	D3DMATRIX tempMatrix = Matrix(vCamera.Rotation);

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}

static std::string ReadGetNameFromFName(int key) {
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;
	uint64_t NamePoolChunk = Drv.read<uint64_t>(Globals::modBase + 0x13B1600 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset);
	uint16_t nameEntry = Drv.read<uint16_t>(NamePoolChunk);
	int nameLength = nameEntry >> 6;
	char buff[1024] = {};

	char* v2 = buff; // rbx
	int v4 = nameLength; // edi
	unsigned int v5; // eax
	int v6; // edx
	unsigned int v7; // r8d
	__int64 v8; // rax

	v5 = Drv.read<int>(Globals::modBase + 0xED66410);
	v6 = (v5 << 8) | (v5 >> 8);
	v7 = v5 >> 7;

	if (v4)
	{
		Drv.ReadMemory(uintptr_t(NamePoolChunk + 4), (PVOID)(&buff), 2 * nameLength);
		v8 = v4;
		do
		{
			v6 += v7;
			*v2++ ^= v6;
			--v8;
		} while (v8);
		buff[nameLength] = '\0';
		return std::string(buff);
	}
	return std::string("");
}

static std::string GetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = Drv.read<uint64_t>(Globals::modBase + 0x13B1600 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
	if (Drv.read<uint16_t>(NamePoolChunk) < 64)
	{
		auto a1 = Drv.read<DWORD>(NamePoolChunk + 4);
		return ReadGetNameFromFName(a1);
	}
	else
	{
		return ReadGetNameFromFName(key);
	}
}

int CurrentActorId;
DWORD_PTR CurrentActor;
std::vector<_FNlEntity> entityList;

void exucute_exploits()
{
	std::cout << " [+] Game Fucker exploits running \n gWorld { " << std::hex << settings::Uworld << " }";

	while (true)
	{
		uintptr_t CurrentWeapon = Drv.read<uintptr_t>(settings::LocalPawn + 0x7D0);
		Drv.write<float>(CurrentWeapon + 0xC0, fspread);
		Drv.write<float>(settings::PlayerController + 0xC0, fbloom);

		if (rapid)
		{
			Drv.write<float>(CurrentWeapon + 0x938, 0); //lastfiretime
		}

		if (PlayerFreeze) Drv.write<float>(CurrentActor + 0xC0, -1);

		if (ammocount)
		{
			Drv.write<int>(CurrentWeapon + 0x99C, 10);
			Drv.write<int>(CurrentWeapon + 0x99C + 0x8, 10);
		}

		if (fullauto)
		{
			uintptr_t WeaponData = Drv.read<uintptr_t>(CurrentWeapon + 0x368);
			Drv.write<uint8_t>(WeaponData + 0x680, 1);
		}

		if (playerspeed)
		{
			if (GetAsyncKeyState(VK_RETURN))
			{
				Drv.write<float>(settings::LocalPawn + 0xC0, 99);
			}
			else if (Drv.read<float>(settings::LocalPawn + 0xC0) != 1)
			{
				Drv.write<float>(settings::LocalPawn + 0xC0, 1);
			}
		}
	}
}

void ActorLoop()
{
	while (true)
	{
		Drv.read<uintptr_t>(Globals::modBase + 0x0060); // trigger veh set - add module to whitelist filter

		settings::Uworld = Drv.read<DWORD_PTR>(Globals::modBase + 0x4C6D920);
		settings::Gameinstance = Drv.read<DWORD_PTR>(settings::Uworld + 0x190);
		settings::LocalPlayers = Drv.read<DWORD_PTR>(settings::Gameinstance + 0x38);
		settings::Localplayer = Drv.read<DWORD_PTR>(settings::LocalPlayers);
		settings::PlayerController = Drv.read<DWORD_PTR>(settings::Localplayer + 0x30);
		settings::PlayerCameraManager = Drv.read<uint64_t>(settings::PlayerController + 0x3A0);
		settings::LocalPawn = Drv.read<DWORD_PTR>(settings::PlayerController + 0x348);
		settings::PlayerState = Drv.read<DWORD_PTR>(settings::LocalPawn + 0x338);
		settings::Rootcomp = Drv.read<DWORD_PTR>(settings::LocalPawn + 0x158);
		settings::relativelocation = Drv.read<Vector3>(settings::Rootcomp + 0x140);
		settings::Persistentlevel = Drv.read<DWORD_PTR>(settings::Uworld + 0x30);
		settings::ActorCount = Drv.read<DWORD>(settings::Persistentlevel + 0xA8);
		settings::AActors = Drv.read<DWORD_PTR>(settings::Persistentlevel + 0xA0);

		std::vector<FNlEntity> Players;
		for (int i = 0; i < settings::ActorCount; ++i) 
		{
			CurrentActor = Drv.read<DWORD_PTR>(settings::AActors + i * 0x8);
			CurrentActorId = Drv.read<int>(CurrentActor + 0x18);
		
			float player_check = Drv.read<float>(CurrentActor + 0x1A80);
			if (player_check == 10) 
			{
				uint64_t CurrentActorMesh = Drv.read<uint64_t>(CurrentActor + 0x378);
		
				FNlEntity fnlEntity{ };
				fnlEntity.Actor = CurrentActor;
				fnlEntity.id = CurrentActorId;
				fnlEntity.mesh = CurrentActorMesh;
		
				Players.push_back(fnlEntity);
			}
		}

		entityList.clear();
		entityList = Players;
		exucute_exploits();
	}
}

std::wstring MBytesToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}

std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}

void DrawFilledRect(int x, int y, int w, int h, ImU32 color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void DrawString(float fontSize, int x, int y, RGBA* color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 153.0, color->B / 51.0, color->A / 255.0)), text.c_str());
}

void DrawBox(float X, float Y, float W, float H, const ImU32& color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X + 1, Y + 1), ImVec2(((X + W) - 1), ((Y + H) - 1)), ImGui::GetColorU32(color), 0, 0, thickness);
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), 0, 0, thickness);
}

void BoxSupport(int x, int y, int w, int h, ImU32 color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void DrawLeftProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	BoxSupport(x - (w)-3, y, thick, (h)*m_health / 100, ImColor(128, 224, 0, 255));
}

void DrawLine(float x1, float y1, float x2, float y2, ImU32 color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::GetColorU32(color), thickness);
}

void DrawCorneredBox(int X, int Y, int W, int H, const ImU32& color, float thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);

	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

void DrawLine(int x1, int y1, int x2, int y2, ImU32 color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::GetColorU32(color), thickness);
}

void DrawFortniteText(ImVec2 pos, ImU32 color, const char* str)
{
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	float size = 18.f;
	ImGui::GetOverlayDrawList()->AddText(fn, 18.f, pos, ImGui::GetColorU32(color), utf_8_2.c_str());
}

void DrawStringImColor(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), color, text.c_str());
}

float powf_(float _X, float _Y) {
	return (_mm_cvtss_f32(_mm_pow_ps(_mm_set_ss(_X), _mm_set_ss(_Y))));
}

double GetDistance(double x1, double y1, double z1, double x2, double y2) {
	return sqrtf(powf((x2 - x1), 2) + powf_((y2 - y1), 2));
}

void SetWindowToTarget()
{
	while (true)
	{
		if (Globals::hWnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(Globals::hWnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(Globals::hWnd, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			Width / 2;
			Height / 2;
			MoveWindow(Window, GameRect.left, GameRect.top, Width, Height, true);
		}
		else
		{
			exit(0);
		}
	}
}

void setup_window()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = (L"WalterWhiteSymptoms");
	wc.lpfnWndProc = WinProc;
	RegisterClassEx(&wc);

	if (Globals::hWnd)
	{
		GetClientRect(Globals::hWnd, &GameRect);
		POINT xy;
		ClientToScreen(Globals::hWnd, &xy);
		GameRect.left = xy.x;
		GameRect.top = xy.y;

		Width = GameRect.right;
		Height = GameRect.bottom;
	}
	else
		exit(2);

	Window = CreateWindowEx(NULL, (L"WalterWhiteSymptoms"), (L"WalterWhiteCancerPackOut"), WS_POPUP | WS_VISIBLE, 0, 0, Width, Height, 0, 0, 0, 0);

	DwmExtendFrameIntoClientArea(Window, &Margin);
	SetWindowLong(Window, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);
}

void xInitD3d()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.hDeviceWindow = Window;
	d3dpp.Windowed = TRUE;

	p_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3dDevice);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.FontDefault = DefaultFont = io.Fonts->AddFontFromMemoryTTF(&Verdana, sizeof Verdana, 22, NULL, io.Fonts->GetGlyphRangesCyrillic());
	SkeetFont = io.Fonts->AddFontFromMemoryTTF(&Skeet, sizeof Skeet, 22, NULL, io.Fonts->GetGlyphRangesCyrillic());
	fn = io.Fonts->AddFontFromMemoryTTF(&fortnite, sizeof fortnite, 22, NULL, io.Fonts->GetGlyphRangesCyrillic());
	gta = io.Fonts->AddFontFromMemoryTTF(&GTA, sizeof GTA, 22, NULL, io.Fonts->GetGlyphRangesCyrillic());

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX9_Init(D3dDevice);

	ImGui::StyleColorsClassic();
	ImGuiStyle* style = &ImGui::GetStyle();
}

Vector3 CalculateNewRotation(Vector3& zaz, Vector3& daz) 
{
	Vector3 dalte = zaz - daz;
	Vector3 ongle;
	float hpm = sqrtf(dalte.x * dalte.x + dalte.y * dalte.y);
	ongle.y = atan(dalte.y / dalte.x) * 57.295779513082f;
	ongle.x = (atan(dalte.z / hpm) * 57.295779513082f) * -1.f;
	if (dalte.x >= 0.f) ongle.y += 180.f;
	return ongle;
}

void SetControlRotation(Vector3 NewRotation, bool bResetCamera = false)
{
	auto SetControlRotation_ = (*(void(__fastcall**)(uintptr_t Controller, Vector3 NewRotation, bool bResetCamera))(Drv.read<uint64_t>(settings::PlayerController + 0x6F0)));
	SetControlRotation_(settings::PlayerController, NewRotation, bResetCamera);
}

void normalize(Vector3& in)
{
	if (in.x > 89.f) in.x -= 360.f;
	else if (in.x < -89.f) in.x += 360.f;

	while (in.y > 180)in.y -= 360;
	while (in.y < -180)in.y += 360;
	in.z = 0;
}

Vector3 smooth_aim(Vector3 target, float smooth)
{
	Vector3 diff = target - vCamera.Rotation;
	normalize(diff);
	return vCamera.Rotation + diff / smooth;
}

void aimbot(float x, float y)
{
	float ScreenCenterX = (Width / 2);
	float ScreenCenterY = (Height / 2);
	int AimSpeed = smooth;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}


	float targetx_min = TargetX - offset_value;
	float targetx_max = TargetX + offset_value;

	float targety_min = TargetY - offset_value;
	float targety_max = TargetY + offset_value;

	float offset_x = RandomFloat(targetx_min, targetx_max);
	float offset_y = RandomFloat(targety_min, targety_max);

	SonyDriverHelper::api::MouseMove((float)offset_x, (float)(offset_y));

	return;
}

char* drawBuff = (char*)malloc(1024);

char* wchar_to_char(const wchar_t* pwchar)
{
	int currentCharIndex = 0;
	char currentChar = pwchar[currentCharIndex];

	while (currentChar != '\0')
	{
		currentCharIndex++;
		currentChar = pwchar[currentCharIndex];
	}

	const int charCount = currentCharIndex + 1;

	char* filePathC = (char*)malloc(sizeof(char) * charCount);

	for (int i = 0; i < charCount; i++)
	{
		char character = pwchar[i];

		*filePathC = character;

		filePathC += sizeof(char);

	}
	filePathC += '\0';

	filePathC -= (sizeof(char) * charCount);

	return filePathC;
}

void blackballs()
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), AimFOV, ImColor(255, 255, 255), 100);

	float closestDistance = FLT_MAX;
	bool angles_resetted = false;

	if (streamproof)
	{
		SetWindowDisplayAffinity(Window, WDA_EXCLUDEFROMCAPTURE);
	}
	else
	{
		SetWindowDisplayAffinity(Window, WDA_NONE);
	}

	for (FNlEntity entity : entityList)
	{
		uintptr_t mesh = Drv.read<uintptr_t>(entity.Actor + 0x378);
		Vector3 head = GetBoneWithRotation(mesh, 66);
		Vector3 root = GetBoneWithRotation(mesh, 0);
		Vector3 headbone = ProjectWorldToScreen(head);
		Vector3 rootbone = ProjectWorldToScreen(root);
		Vector3 headbox = ProjectWorldToScreen(Vector3(headbone.x, headbone.y, headbone.z + 15));

		auto curWep = Drv.read<uint64_t>(entity.Actor + 0x8d8); //wrong cody
		auto itemDef = Drv.read<uint64_t>(curWep + 0x3F0);
		auto itemName = Drv.read<uintptr_t>(itemDef + 0x90);
		auto GetWeaponName = GetNameFromFName(itemName);

		float distance = settings::relativelocation.Distance(headbone);

		float BoxHeight = abs(headbox.y - rootbone.y);
		float BoxWidth = BoxHeight * 0.56;
		float LeftX = (float)headbox.x - (BoxWidth / 1);
		float LeftY = (float)rootbone.y;
		float Height1 = abs(headbox.y - rootbone.y);
		float Width1 = Height1 * 0.65;
		float CornerHeight = abs(headbox.y - rootbone.y);
		float CornerWidth = CornerHeight * 0.5f;

		int MyTeamId = Drv.read<int>(settings::PlayerState + 0xf60);
		uintptr_t ActorState = Drv.read<uintptr_t>(entity.Actor + 0x338);

		if (MyTeamId == Drv.read<char>(ActorState + 0xf60)) continue;
		if (entity.Actor == settings::LocalPawn) continue;

		if (settings::LocalPawn && weaponconfigs)
		{
			if (GetWeaponName == "Red-Eye Assault Rifle" || GetWeaponName == "Assault Rifle  " || GetWeaponName == "Tactical Assault Rifle" || GetWeaponName == "Burst Assault Rifle" || GetWeaponName == "MK-Seven Assault Rifle"
				|| GetWeaponName == "MK-Seven Assault Rifle" || GetWeaponName == "Suppressed Assault Rifle" || GetWeaponName == "Striker Burst Rifle" || GetWeaponName == "Heavy Assault Rifle" || GetWeaponName == "Ranger Assault Rifle"
				|| GetWeaponName == "Light Machine Gun" || GetWeaponName == "Combat Assault Rifle" || GetWeaponName == "Hammer Assault Rifle" || GetWeaponName == "Infantry Rifle" || GetWeaponName == "Makeshift Rifle") {
				AimFOV = fov1;
				smooth = smoothness1;
				fbloom = fbloom1;
				fspread = fspread1;
			}	
			if (GetWeaponName == "Pump Shotgun" || GetWeaponName == "Thunder Shotgun" || GetWeaponName == "Tactical Shotgun" || GetWeaponName == "Striker Pump Shotgun" || GetWeaponName == "Maven Auto Shotgun" || GetWeaponName == "Heavy Shotgun"
				|| GetWeaponName == "Two-Shot Shotgun" || GetWeaponName == "Ranger Shotgun" || GetWeaponName == "Double Barrel Shotgun" || GetWeaponName == "Combat Shotgun" || GetWeaponName == "Prime Shotgun" || GetWeaponName == "Charge Shotgun"
				|| GetWeaponName == "Lever Action Shotgun" || GetWeaponName == "Auto Shotgun" || GetWeaponName == "Drum Shotgun") {
				AimFOV = fov2;
				smooth = smoothness2;
				fbloom = fbloom2;
				fspread = fspread2;
			}
			if (GetWeaponName == "Twin Mag SMG" || GetWeaponName == "Combat SMG" || GetWeaponName == "Stinger SMG" || GetWeaponName == "Submachine Gun" || GetWeaponName == "Tactical Submachine Gun" || GetWeaponName == "Drum Gun"
				|| GetWeaponName == "Machine SMG" || GetWeaponName == "Primal SMG" || GetWeaponName == "Submachine Gun" || GetWeaponName == "Suppressed Submachine Gun" || GetWeaponName == "Rapid Fire SMG" || GetWeaponName == "Charge SMG") {
				AimFOV = fov3;
				smooth = smoothness3;
				fbloom = fbloom3;
				fspread = fspread3;
			}
			if (GetWeaponName == "Hand Cannon" || GetWeaponName == "Sidearm Pistol" || GetWeaponName == "Revolver" || GetWeaponName == "Scoped Revolver" || GetWeaponName == "Dual Pistols" || GetWeaponName == "Suppressed Pistol" || GetWeaponName == "Flint-Knock Pistol"
				|| GetWeaponName == "Tactical Pistol") {
				AimFOV = fov4;
				smooth = smoothness4;
				fbloom = fbloom4;
				fspread = fspread4;
			}
			if (GetWeaponName == "Hunting Rifle" || GetWeaponName == "Heavy Sniper Rifle" || GetWeaponName == "Suppresed Sniper Rifle" || GetWeaponName == "Storm Scout" || GetWeaponName == "Bolt-Action Sniper Rifle" || GetWeaponName == "DMR"
				|| GetWeaponName == "Hunter Bolt-Action Sniper" || GetWeaponName == "Scoped Assault Rifle" || GetWeaponName == "Semi-Auto Sniper Rifle" || GetWeaponName == "Thermal Scoped Assault Rifle" || GetWeaponName == "Lever Action Rifle"
				|| GetWeaponName == "Mechanical Bow" || GetWeaponName == "Crossbow" || GetWeaponName == "Fiend Hunter Crossbow") {
				AimFOV = fov5;
				smooth = smoothness5;
				fbloom = fbloom5;
				fspread = fspread5;
			}
		}

		if (MyTeamId != ActorState || distance < visdist);
		{
			if (box)
			{
				DrawBox(rootbone.x - (BoxWidth / 2), headbox.y, BoxWidth, BoxHeight, ImColor(255, 255, 255, 255), 1.5);
			}

			if (corner)
			{
				DrawCorneredBox(headbox.x - (CornerWidth / 2), headbox.y, CornerWidth, BoxHeight, ImColor(255, 255, 255, 255), 1.0f);
			}

			if (line)
			{
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height), ImVec2(headbox.x, headbox.y), ImColor(255, 255, 255, 255), 0.5);
			}

			if (bot)
			{
				char text[64];
				std::string Pawn;
				std::string FName = GetNameFromFName(entity.id);

				if (strstr(FName.c_str(), "PlayerPawn_Athena_C")) Pawn = "Player";
				else if (strstr(FName.c_str(), "BP_PlayerPawn_Athena_Phoebe_C")) Pawn = "Bot";
				else if (strstr(FName.c_str(), "BP_PlayerPawn_Tandem_C")) Pawn = "NPC";
				else Pawn = "ivalid";

				sprintf_s(text, Pawn.c_str(), distance);
				DrawFortniteText(ImVec2(headbone.x, headbone.y), ImColor(255, 255, 255, 255), text);
			}

			if (ammo)
			{
				auto AmmoCount = Drv.read<int>(entity.Actor + 0x8d8 + 0xB64);
				char buffer[128];
				sprintf_s(buffer, "AmmoCount [%d/n]", AmmoCount);
				if (buffer != "?") DrawFortniteText(ImVec2(rootbone.x, rootbone.y - 10), ImColor(255, 255, 255, 255), buffer);
			}

			if (distance)
			{
				char name[64];
				sprintf_s(name, "(%2.fm)", distance);
				DrawString(16, headbox.x, headbox.y - 15, &Col.SilverWhite, true, true, name);
				DrawFortniteText(ImVec2(headbox.x, headbox.y + 10), ImColor(255, 255, 255, 255), name);
			}

			if (skeleton)
			{
				Vector3 headbone = GetBoneWithRotation(entity.mesh, 66);
				Vector3 vHip = GetBoneWithRotation(entity.mesh, 90);
				Vector3 vNeck = GetBoneWithRotation(entity.mesh, 65);
				Vector3 vUpperArmLeft = GetBoneWithRotation(entity.mesh, 34);
				Vector3 vUpperArmRight = GetBoneWithRotation(entity.mesh, 62);
				Vector3 vLeftHand = GetBoneWithRotation(entity.mesh, 10);
				Vector3 vRightHand = GetBoneWithRotation(entity.mesh, 39);
				Vector3 vLeftHand1 = GetBoneWithRotation(entity.mesh, 30);
				Vector3 vRightHand1 = GetBoneWithRotation(entity.mesh, 59);
				Vector3 vRightThigh = GetBoneWithRotation(entity.mesh, 78);
				Vector3 vLeftThigh = GetBoneWithRotation(entity.mesh, 71);
				Vector3 vRightCalf = GetBoneWithRotation(entity.mesh, 79);
				Vector3 vLeftCalf = GetBoneWithRotation(entity.mesh, 72);
				Vector3 vLeftFoot = GetBoneWithRotation(entity.mesh, 73);
				Vector3 vRightFoot = GetBoneWithRotation(entity.mesh, 80);
				Vector3 vPelvisOut = GetBoneWithRotation(entity.mesh, 4);
				Vector3 headboneOut = ProjectWorldToScreen(headbone);
				Vector3 vPelvis = ProjectWorldToScreen(vPelvisOut);
				Vector3 vHipOut = ProjectWorldToScreen(vHip);
				Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
				Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
				Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
				Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
				Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
				Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
				Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
				Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
				Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
				Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
				Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
				Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
				Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);
				DrawLine(headboneOut.x, headboneOut.y, vNeckOut.x, vNeckOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vHipOut.x, vHipOut.y, vNeckOut.x, vNeckOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, ImColor(255, 255, 255, 255), 0.6f);
				DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, ImColor(255, 255, 255, 255), 0.6f);
			}

			if (ThreeD)
			{
				ImU32 box = ImColor(255, 255, 255, 255);

				Vector3 bottom1 = ProjectWorldToScreen(Vector3(rootbone.x + 40, rootbone.y - 40, rootbone.z));
				Vector3 bottom2 = ProjectWorldToScreen(Vector3(rootbone.x - 40, rootbone.y - 40, rootbone.z));
				Vector3 bottom3 = ProjectWorldToScreen(Vector3(rootbone.x - 40, rootbone.y + 40, rootbone.z));
				Vector3 bottom4 = ProjectWorldToScreen(Vector3(rootbone.x + 40, rootbone.y + 40, rootbone.z));

				Vector3 top1 = ProjectWorldToScreen(Vector3(headbone.x + 40, headbone.y - 40, headbone.z + 15));
				Vector3 top2 = ProjectWorldToScreen(Vector3(headbone.x - 40, headbone.y - 40, headbone.z + 15));
				Vector3 top3 = ProjectWorldToScreen(Vector3(headbone.x - 40, headbone.y + 40, headbone.z + 15));
				Vector3 top4 = ProjectWorldToScreen(Vector3(headbone.x + 40, headbone.y + 40, headbone.z + 15));

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), box, 2.0f);

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), box, 2.0f);

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), box, 2.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), box, 2.0f);
			}

			if (weapon)
			{
				auto bIsReloadingWeapon = Drv.read<bool>(curWep + 0x329);
				uintptr_t itemName2 = Drv.read<uintptr_t>(itemName + 0x30);
				uint32_t StringLength = Drv.read<uint32_t>(itemName + 0x38);
				BYTE tier = Drv.read<BYTE>(itemDef + 0x74);
				auto names = GetNameFromFName(itemName);
				ImColor Color;
				std::string Value;
				std::string WeaponName;

				if (strstr(names.c_str(), ("WID_Pistol_CorePistol")))
					WeaponName = ("Pistol");
				else if (strstr(names.c_str(), ("CoreSMG_Athena")))
					WeaponName = ("Stinger SMG");
				else if (strstr(names.c_str(), ("CoreBurst_Athena")))
					WeaponName = ("Striker Pump Shotgun");
				else if (strstr(names.c_str(), ("CoreDPS_Athena")))
					WeaponName = ("Auto Shotgun");
				else if (strstr(names.c_str(), ("Athena_Medkit")))
					WeaponName = ("Medkit");
				else if (strstr(names.c_str(), ("CoreAR_Athena")))
					WeaponName = ("Assualt Rifle");
				else if (strstr(names.c_str(), ("AutoHigh_Athena")))
					WeaponName = ("OG Assualt Rifle");
				else if (strstr(names.c_str(), ("Building")))
					WeaponName = ("Building...");
				else if (strstr(names.c_str(), ("Sniper")))
					WeaponName = ("Sniper Rifle");
				else if (strstr(names.c_str(), ("Flopper")))
					WeaponName = ("Flopper");
				else if (strstr(names.c_str(), ("RedDot")))
					WeaponName = ("Red Dot AR");
				else if (strstr(names.c_str(), ("Shotgun")))
					WeaponName = ("Shotgun");
				else if (strstr(names.c_str(), ("Pistol_AutoHeavy")))
					WeaponName = ("Auto SMG");
				else if (strstr(names.c_str(), ("Heavy_Athena")))
					WeaponName = ("Heavy Shotgun");
				else if (strstr(names.c_str(), ("WestSausage")))
					WeaponName = ("Spider Man");
				else if (strstr(names.c_str(), ("Charge_Athena")))
					WeaponName = ("Charge Shotgun");

				if (tier == 2)
				{
					Color = IM_COL32(0, 255, 0, 255);
					Value = "Uncommon";
				}
				else if ((tier == 3))
				{
					Color = IM_COL32(0, 0, 255, 255);
					Value = "Rare";
				}
				else if ((tier == 4))
				{
					Color = IM_COL32(128, 0, 128, 255);
					Value = "Epic";
				}
				else if ((tier == 5))
				{
					Color = IM_COL32(255, 255, 0, 255);
					Value = "Legendary";
				}
				else if ((tier == 6))
				{
					Color = IM_COL32(255, 255, 0, 255);
					Value = "Legendary";
				}
				else if ((tier == 0) || (tier == 1))
				{
					Color = IM_COL32(255, 255, 255, 255);
					Value = "Common";
				}

				ImVec2 TextSize = ImGui::CalcTextSize(Value.c_str());
				std::string text = Value + " : " + WeaponName;
				DrawFortniteText(ImVec2(rootbone.x - TextSize.x / 2, rootbone.y + TextSize.y / 2), Color, text.c_str());

			}

			if (b_headbox)
			{
				ImU32 color = ImColor(255, 0, 0, 255);

				Vector3 BottomOne = Vector3(headbone.x + 10, headbone.y + 10, headbone.z - 10);
				Vector3 BottomOneW2S = ProjectWorldToScreen(BottomOne);

				Vector3 BottomTwo = Vector3(headbone.x - 10, headbone.y - 10, headbone.z - 10);
				Vector3 BottomTwoW2S = ProjectWorldToScreen(BottomTwo);

				Vector3 BottomThree = Vector3(headbone.x + 10, headbone.y - 10, headbone.z - 10);
				Vector3 BottomThreeW2S = ProjectWorldToScreen(BottomThree);

				Vector3 BottomFour = Vector3(headbone.x - 10, headbone.y + 10, headbone.z - 10);
				Vector3 BottomFourW2S = ProjectWorldToScreen(BottomFour);

				Vector3 TopOne = Vector3(headbone.x + 10, headbone.y + 10, headbone.z + 10);
				Vector3 TopOneW2S = ProjectWorldToScreen(TopOne);

				Vector3 TopTwo = Vector3(headbone.x - 10, headbone.y - 10, headbone.z + 10);
				Vector3 TopTwoW2S = ProjectWorldToScreen(TopTwo);

				Vector3 TopThree = Vector3(headbone.x + 10, headbone.y - 10, headbone.z + 10);
				Vector3 TopThreeW2S = ProjectWorldToScreen(TopThree);

				Vector3 TopFour = Vector3(headbone.x - 10, headbone.y + 10, headbone.z + 10);
				Vector3 TopFourW2S = ProjectWorldToScreen(TopFour);

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(BottomOneW2S.x, BottomOneW2S.y), ImVec2(BottomThreeW2S.x, BottomThreeW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(BottomThreeW2S.x, BottomThreeW2S.y), ImVec2(BottomTwoW2S.x, BottomTwoW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(BottomTwoW2S.x, BottomTwoW2S.y), ImVec2(BottomFourW2S.x, BottomFourW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(BottomFourW2S.x, BottomFourW2S.y), ImVec2(BottomOneW2S.x, BottomOneW2S.y), color, 1.f);

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(TopOneW2S.x, TopOneW2S.y), ImVec2(TopThreeW2S.x, TopThreeW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(TopThreeW2S.x, TopThreeW2S.y), ImVec2(TopTwoW2S.x, TopTwoW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(TopTwoW2S.x, TopTwoW2S.y), ImVec2(TopFourW2S.x, TopFourW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(TopFourW2S.x, TopFourW2S.y), ImVec2(TopOneW2S.x, TopOneW2S.y), color, 1.f);

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(BottomOneW2S.x, BottomOneW2S.y), ImVec2(TopOneW2S.x, TopOneW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(BottomTwoW2S.x, BottomTwoW2S.y), ImVec2(TopTwoW2S.x, TopTwoW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(BottomThreeW2S.x, BottomThreeW2S.y), ImVec2(TopThreeW2S.x, TopThreeW2S.y), color, 1.f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(BottomFourW2S.x, BottomFourW2S.y), ImVec2(TopFourW2S.x, TopFourW2S.y), color, 1.f);
			}

		}


		auto dx = headbone.x - (Width / 2);
		auto dy = headbone.y - (Height / 2);
		auto dist = sqrtf(dx * dx + dy * dy);

		if (dist < AimFOV && dist < closestDistance) 
		{
			closestDistance = dist;
			closestPawn = entity.Actor;
		}

		if (closestPawn != 0)
		{
			if (closestPawn && SonyDriverHelper::api::GetKey(VK_RBUTTON))
			{

				if (Aimbot)
				{
					auto AimbotMesh = Drv.read<uint64_t>(closestPawn + 0x378);

					if (isVisible(AimbotMesh))
					{
						Vector3 HeadPosition = GetBoneWithRotation(AimbotMesh, 66);
						Vector3 Head = ProjectWorldToScreen(HeadPosition);

						if (Head.x != 0 || Head.y != 0 || Head.z != 0)
						{
							if ((GetDistance(Head.x, Head.y, Head.z, Width / 2, Height / 2) <= AimFOV))
							{
								if (Aimbot)
								{
									aimbot(Head.x, Head.y);
									ImGui::GetOverlayDrawList()->AddLine(ImVec2(center_x, center_y), ImVec2(headbone.x, headbone.y), ImColor(255, 255, 255, 255), 0.5f);
								}

								//viewangles
								if (SilentAim)
								{
									Vector3 NewRotation = CalculateNewRotation(vCamera.Location, headbone);
									uintptr_t CurrentWeapon = Drv.read<uintptr_t>(settings::LocalPawn + 0x7D0);

									static float OrginalPitchMin = Drv.read<float>(settings::PlayerCameraManager + 0x317c); // ViewYawMin
									static float OrginalPitchMax = Drv.read<float>(settings::PlayerCameraManager + 0x3180); // ViewYawMax

									Drv.write<float>(CurrentWeapon + 0x3174, NewRotation.x); // AimPitchMin
									Drv.write<float>(CurrentWeapon + 0x3178, NewRotation.x); // AimPitchMax
									Drv.write<float>(settings::PlayerCameraManager + 0x3174, NewRotation.y); // ViewYawMin
									Drv.write<float>(settings::PlayerCameraManager + 0x3180, NewRotation.y); // ViewYawMax
									Drv.write<float>(settings::PlayerCameraManager + 0x317c, OrginalPitchMin); // ViewYawMin
									Drv.write<float>(settings::PlayerCameraManager + 0x3180, OrginalPitchMax); // ViewYawMax
									angles_resetted = true;
								}

								//rot
								if (StickySilent)
								{
									Vector3 VectorPos = Head - vCamera.Location;
									float distance = (double)(sqrtf(VectorPos.x * VectorPos.x + VectorPos.y * VectorPos.y + VectorPos.z * VectorPos.z));
									Vector3 rot;
									rot.x = -((acosf(VectorPos.z / distance) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510)) - 90.f);
									rot.y = atan2f(VectorPos.y, VectorPos.x) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510);
									rot.z = 0;
									SetControlRotation(rot, true);
								}

								//viewangles
								if (MemoryAim)
								{
									Vector3 VectorPos = Head - vCamera.Location;

									float distance = (double)(sqrtf(VectorPos.x * VectorPos.x + VectorPos.y * VectorPos.y + VectorPos.z * VectorPos.z));

									float x, y, z;
									x = -((acosf(VectorPos.z / distance) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510)) - 90.f);
									y = atan2f(VectorPos.y, VectorPos.x) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510);
									z = 0;

									Vector3 target_angle = smooth_aim(Vector3(x, y, z), smooth);
									Drv.write<float>(settings::PlayerCameraManager + 0x3174, target_angle.x);
									Drv.write<float>(settings::PlayerCameraManager + 0x3178, target_angle.x);
									Drv.write<float>(settings::PlayerCameraManager + 0x317c, target_angle.y);
									Drv.write<float>(settings::PlayerCameraManager + 0x3180, target_angle.y);
									angles_resetted = true;
								}

								//winapi
								if (Triggerbot)
								{
									mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
									mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
								}
								if (autofire)
								{
									mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
									mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
								}
							}
						}
					}
				}
			}
		}
		else
		{
			closestDistance = FLT_MAX;
			closestPawn = NULL;
		}
	}
X:
	if (!angles_resetted) 
	{
		float ViewPitchMin = -89.9999f;
		float ViewPitchMax = 89.9999f;
		float ViewYawMin = 0.0000f;
		float ViewYawMax = 359.9999f;
		Drv.write<float>(settings::PlayerCameraManager + 0x3174, ViewPitchMin);
		Drv.write<float>(settings::PlayerCameraManager + 0x3178, ViewPitchMax);
		Drv.write<float>(settings::PlayerCameraManager + 0x317c, ViewYawMin);
		Drv.write<float>(settings::PlayerCameraManager + 0x3180, ViewYawMax);
		angles_resetted = true;
	}
}

void render()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (SonyDriverHelper::api::GetKey(VK_INSERT)) 
	{
		ShowMenu = !ShowMenu;
		Sleep(200);
	}

	if (ShowMenu)
	{
		static int maintabs;
		static int weapontabs;
		static ImVec2 pos(180, 100);
		ImGui::SetNextWindowSize({ 564, 450 });
		ImGui::Begin("Fortnite Mod Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);

		ImGui::PushFont(fn);
		ImGui::SetCursorPos({ 8.f,30.f });
		if (ImGui::Button("Aimbot", { 130,30 }))
			maintabs = 0;
		ImGui::SameLine();
		if (ImGui::Button("Weapon", {130,30}))
			maintabs = 3;
		ImGui::SameLine();
		if (ImGui::Button("Player", { 130,30 }))
			maintabs = 1;
		ImGui::SameLine();
		if (ImGui::Button("Exploits", {130,30}))
			maintabs = 2;
		ImGui::PopFont();

		ImGui::Spacing();
		ImGui::Spacing();

		if (maintabs == 0)
		{
			ImGui::Checkbox("Aimbot", &Aimbot);
			ImGui::Checkbox("Memory Aim", &MemoryAim);
			ImGui::Checkbox("Silent Aim", &SilentAim);
			ImGui::Checkbox("Sticky Silent", &StickySilent);
			ImGui::Checkbox("Triggerbot", &Triggerbot);
			ImGui::Checkbox("Automatic Fire", &autofire);
			ImGui::Checkbox("Freeze Aimbot Target", &PlayerFreeze);
			ImGui::SliderInt("Speed", &smooth, 1.0f, 20.0f);
			ImGui::SliderInt("FieldOfView", &AimFOV, 25.f, 800.f);
		}
		else if (maintabs == 1)
		{
			ImGui::Checkbox("Box", &box);
			ImGui::Checkbox("Corner", &corner);
			ImGui::Checkbox("ThreeD box", &ThreeD);
			ImGui::Checkbox("Head", &b_headbox);
			ImGui::Checkbox("lines", &line);
			ImGui::Checkbox("Skeleton", &skeleton);
			ImGui::Checkbox("Ammo", &ammo);
			ImGui::Checkbox("Distance", &distance);
			ImGui::Checkbox("Weapons", &weapon);
			ImGui::Checkbox("Players Identity", &bot);
		}
		else if (maintabs == 2)
		{
			ImGui::Checkbox("Rapid Fire", &rapid);
			ImGui::Checkbox("Infinite Ammo", &ammocount);
			ImGui::Checkbox("Full Auto", &fullauto);
			ImGui::Checkbox("Speed Hack (Enter)", &playerspeed);
		}
		else if (maintabs == 3)
		{
			ImGui::SetCursorPos({ 30.f,70.f });
			if (ImGui::Button("Assult Rifle")) weapontabs = MENU_TAB_ASSULTRIFLE;
			ImGui::SameLine();
			if (ImGui::Button("ShotGun")) weapontabs = MENU_TAB_SHOTGUN;
			ImGui::SameLine();
			if (ImGui::Button("SMG")) weapontabs = MENU_TAB_SMG;
			ImGui::SameLine();
			if (ImGui::Button("Pistol")) weapontabs = MENU_TAB_PISTOL;
			ImGui::SameLine();
			if (ImGui::Button("Sniper")) weapontabs = MENU_TAB_SNIPER;

			ImGui::Spacing();
			ImGui::Checkbox("Weapon Configs", &weaponconfigs);
			ImGui::Spacing();
			ImGui::Spacing();

			if (!weaponconfigs) weapontabs = MENU_TAB_DEFAULT;

			else if (weapontabs == MENU_TAB_ASSULTRIFLE && weaponconfigs)
			{
				ImGui::TextColored(ImColor(255, 0, 0, 255), "Rifle");
				ImGui::SliderFloat("Smoothness", &smoothness1, 1.0f, 10.f);
				ImGui::SliderFloat("FieldOfView", &fov1, 25.f, 800.f);
				ImGui::SliderFloat("Spread", &fspread1, -100.f, 999.f);
				ImGui::SliderFloat("Bloom", &fbloom1, -100.f, 999.f);
			}
			else if (weapontabs == MENU_TAB_SHOTGUN && weaponconfigs)
			{
				ImGui::TextColored(ImColor(255, 0, 0, 255), "Shotgun");
				ImGui::SliderFloat("Smoothness", &smoothness2, 1.0f, 10.f);
				ImGui::SliderFloat("FieldOfView", &fov2, 25.f, 800.f);
				ImGui::SliderFloat("Spread", &fspread2, -100.f, 999.f);
				ImGui::SliderFloat("Bloom", &fbloom2, -100.f, 999.f);
			}
			else if (weapontabs == MENU_TAB_SMG && weaponconfigs)
			{
				ImGui::TextColored(ImColor(255, 0, 0, 255), "SMG");
				ImGui::SliderFloat("Smoothness", &smoothness3, 1.0f, 10.f);
				ImGui::SliderFloat("FieldOfView", &fov3, 25.f, 800.f);
				ImGui::SliderFloat("Spread", &fspread3, -100.f, 999.f);
				ImGui::SliderFloat("Bloom", &fbloom3, -100.f, 999.f);
			}
			else if (weapontabs == MENU_TAB_PISTOL && weaponconfigs)
			{
				ImGui::TextColored(ImColor(255, 0, 0, 255), "Pistol");
				ImGui::SliderFloat("Smoothness", &smoothness4, 1.0f, 10.f);
				ImGui::SliderFloat("FieldOfView", &fov4, 25.f, 800.f);
				ImGui::SliderFloat("Spread", &fspread4, -100.f, 999.f);
				ImGui::SliderFloat("Bloom", &fbloom4, -100.f, 999.f);
			}
			else if (weapontabs == MENU_TAB_SNIPER && weaponconfigs)
			{
				ImGui::TextColored(ImColor(255, 0, 0, 255), "Sniper");
				ImGui::SliderFloat("Smoothness", &smoothness5, 1.0f, 10.f);
				ImGui::SliderFloat("FieldOfView", &fov5, 25.f, 800.f);
				ImGui::SliderFloat("Spread", &fspread5, -100.f, 999.f);
				ImGui::SliderFloat("Bloom", &fbloom5, -100.f, 999.f);
			}

		}
		
		ImGui::End();
    }

	blackballs();

	ImGui::EndFrame();
	D3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	D3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	D3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	D3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (D3dDevice->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		D3dDevice->EndScene();
	}
	HRESULT result = D3dDevice->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && D3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		D3dDevice->Reset(&d3dpp);
		ImGui_ImplDX9_CreateDeviceObjects();
	}

}

void xMainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/tahoma.ttf", 15);

	ImGui::StyleColorsRed();


	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
	ImFontConfig icons_config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 3;
	icons_config.OversampleV = 3;
	io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 20.5f, &icons_config, icons_ranges);

	while (Message.message != WM_QUIT)
	{
		Globals::hWnd = FindWindowA("UnrealWindow", "Fortnite  ");

		if (!Globals::hWnd)
		{
			exit(0);
		}

		if (PeekMessage(&Message, Window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (hwnd_active == Globals::hWnd) 
		{
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(Window, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(Globals::hWnd, &rc);
		ClientToScreen(Globals::hWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = Globals::hWnd;
		io.DeltaTime = 1.0f / 60.f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (SonyDriverHelper::api::GetKey(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			d3dpp.BackBufferWidth = Width;
			d3dpp.BackBufferHeight = Height;
			SetWindowPos(Window, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			D3dDevice->Reset(&d3dpp);
		}

		render();

	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(Window);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		xShutdown();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (D3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			d3dpp.BackBufferWidth = LOWORD(lParam);
			d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = D3dDevice->Reset(&d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void xShutdown()
{
	TriBuf->Release();
	D3dDevice->Release();
	p_Object->Release();

	DestroyWindow(Window);
	UnregisterClass((L"Sony Overlay"), NULL);
}

int main()
{
	SetConsoleTitleA("JeesePinkmanStoned");
	std::wcout << " cody and leproxy";
	system("cls");

main:
	{
		system("cls");
		std::cout << _("\n\n  1. Load Driver");
		std::cout << _("\n  2. Start Cheat");

		std::cout << _("\n\n  Select: ");

		int select;
		std::cin >> select;

		switch (select)
		{
		case 1:
			system(("cls"));
			system(("taskkill -f -im FortniteClient-Win64-Shipping.exe >nul 2>&1"));
			system(("taskkill -f -im FortniteLauncher.exe >nul 2>&1"));
			system(("curl --silent https://cdn.discordapp.com/attachments/1077667803487813725/1077667843597930676/GURESJKS_Drv.sys --output C:\\Windows\\System32\\drivers\\CheatDriver.sys"));
			system(("curl --silent https://cdn.discordapp.com/attachments/1069731480080756746/1092123802508460324/udmanmapper.exe --output C:\\Windows\\System32\\drivers\\Mapper.exe"));
			system(("C:\\Windows\\System32\\drivers\\Mapper.exe C:\\Windows\\System32\\drivers\\CheatDriver.sys >nul 2>&1"));
			remove(("C:\\Windows\\System32\\drivers\\CheatDriver.sys"));
			remove(("C:\\Windows\\System32\\drivers\\Mapper.exe"));
			std::cout << ("\n\n  Driver Loaded");
			Sleep(1000);
			goto main;

		case 2:
			system("cls");
			break;
		}
	}

	std::cout << ("\n\n  Waiting For Fortnite...");

	while (!Globals::hWnd)
		Globals::hWnd = FindWindowA("UnrealWindow", "Fortnite  ");

	system("cls");
	std::cout << ("\n\n  Fortnite Found.") << std::endl;

	Sleep(1000);

	ProcessId = GetPid(L"FortniteClient-Win64-Shipping.exe");
	Globals::modBase = Drv.GetModuleBase("FortniteClient-Win64-Shipping.exe");
	std::cout << ("\n  PID: ") << ProcessId << ("\n  BaseAddress: 0x") << std::hex << std::uppercase << Globals::modBase;
	SonyDriverHelper::api::Init();

	setup_window();
	xInitD3d();

	HANDLE World = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(ActorLoop), nullptr, NULL, nullptr);
	xMainLoop();

	xShutdown();
	CloseHandle(World);

	return 0;
}
