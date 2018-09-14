#pragma once
#include "windows.h"
#include "stdio.h"
#include "winuser.h"
#include "resource.h"
#include "shlwapi.h"
#include "Log.h"
#include "HackProcess.h"
#include "VMProtectSDK.h"
#define VMPDSA(a) VMProtectDecryptStringA(a)
#pragma comment(lib,"shlwapi.lib")

extern CLog Log;

extern CHackProcess HackProcess;

void InitGameBase();
void RenderGameHack();

#define DEF_UWorld 0x2ead6d0 
#define DEF_UGameInstance 0x140   //1a80291	mov rax, qword ptr [rdi+00000140h]
#define DEF_ULocalPlayer 0x38   //19c9330	mov rax, qword ptr [rdi+38h]
#define DEF_PlayerController 0x30 
#define DEF_ControlRotation 0x368
#define DEF_ControlPos 0x70
#define DEF_ULevel 0x30 
#define DEF_EntityCount 0xa8
#define DEF_ViewPortClient 0x58
#pragma pack(push, 1)

struct Vector3
{
	float X;
	float Y;
	float Z;
	Vector3 &operator + (Vector3 &vec)
	{
		float x = X + vec.X;
		float y = Y + vec.Y;
		float z = Z + vec.Z;
		return Vector3{ x, y, z };
	}
	Vector3 &operator - (Vector3 &vec)
	{
		float x = X - vec.X;
		float y = Y - vec.Y;
		float z = Z - vec.Z;
		return Vector3{ x, y, z };
	}
};
struct Rotator
{
	float Pitch;
	float Yaw;
	float Roll;
	Rotator &operator + (Rotator &vec)
	{
		float x = Pitch + vec.Pitch;
		float y = Yaw + vec.Yaw;
		float z = Roll + vec.Roll;
		return Rotator{ x, y, z };
	}
	Rotator &operator - (Rotator &vec)
	{
		float x = Pitch - vec.Pitch;
		float y = Yaw - vec.Yaw;
		float z = Roll - vec.Roll;
		return Rotator{ x, y, z };
	}
};
#pragma pack()

class GameOffsets
{
public:
	DWORD64 BaseAddress;
	DWORD64 dwUEngine;


public:
	DWORD64 dwUWorld = 0;
	DWORD64 dwGameInstance = 0;
	DWORD64 dwULocalPlayer = 0;
	DWORD64 dwPWorld = 0;
	DWORD64 dwULevel = 0;
	DWORD64 dwLocalPlayer = 0;
	DWORD64 dwViewPortClient = 0;
	DWORD64 dwPlayerController = 0;
	DWORD64 dwPlayerCameraManager = 0;
	DWORD64 dwCameraCache = 0;
	DWORD64 dwEntityList = 0;
	
public:
	DWORD64 GetUWorld()
	{
		DWORD64 rt = 0;
		if (!HackProcess.Read(BaseAddress + DEF_UWorld, &rt, sizeof(rt)))
			return 0;
		else
			return rt;
	}
	DWORD64 GetGameInstanceFromUEngine()
	{
		DWORD64 BaseAddress = GetUWorld();
		if (!BaseAddress) return 0;
		DWORD64 rt = 0;
		if (!HackProcess.Read(BaseAddress + DEF_UGameInstance, &rt, sizeof(rt)))
			return 0;
		return rt;

	}
	DWORD64 GetULocalPlayer()
	{

		DWORD64 addr = GetGameInstanceFromUEngine();

		if (!addr) return 0;
		DWORD64 rt = 0;
		
		if (!HackProcess.Read(addr + DEF_ULocalPlayer, &rt, sizeof(rt)))
			return 0;
	
		return rt;

	}
	DWORD64 GetLocalPlayer()
	{

		DWORD64 addr = GetULocalPlayer();
		//	Log.printf("GetLocalPlayer : %I64x\r\n", addr);
		if (!addr) return 0;
		DWORD64 rt = 0;

		if (!HackProcess.Read(addr, &rt, sizeof(rt)))
			return 0;
		return rt;

	}
	DWORD64 GetPlayerController()
	{
		DWORD64 param = GetLocalPlayer();
		//	Log.printf("GetPlayerController : %I64x\r\n", param);
		if (!param) return 0;
		DWORD64 rt = 0;
		
			if (!HackProcess.Read(param + DEF_PlayerController, &rt, sizeof(rt)))
				return 0;
	
		return rt;

	}
	bool GetControlRotation(Rotator * vec)
	{
		DWORD64 PlayerController = GetPlayerController();
		if (!PlayerController) return false;
	
			if (!HackProcess.Read(PlayerController + DEF_ControlRotation, vec, sizeof(Rotator)))
				return false;
	
		return true;

	}
	bool GetControlPos(Rotator * vec)
	{
		DWORD64 PlayerController = GetLocalPlayer();
		if (!PlayerController) return false;

		if (!HackProcess.Read(PlayerController + DEF_ControlPos, vec, sizeof(Rotator)))
			return false;

		return true;

	}

	DWORD64 GetULevel()
	{
		DWORD64 param = DEF_UWorld;//GetPWorld();
		if (!param) return 0;
		DWORD64 addr;
	
			addr = param + DEF_ULevel;
		
		DWORD64 rt = 0;
		if (!HackProcess.Read(addr, &rt, sizeof(rt)))
			return 0;
		return rt;

	}

	DWORD64 GetViewPortClient()
	{
		DWORD64 param = GetLocalPlayer();
			Log.printf("GetViewPortClient : %I64x\r\n", param);
		if (!param) return 0;
		DWORD64 addr = param + DEF_ViewPortClient;
		DWORD64 rt = 0;
		if (!HackProcess.Read(addr, &rt, sizeof(rt)))
			return 0;
		return rt;

	}

	DWORD64 GetPWorld()
	{
		DWORD64 param = GetViewPortClient();
		//DWORD64 param = GetUEnginedView();
		//Log.printf("GetPWorld : %I64x\r\n", param);
		if (!param) return 0;
		DWORD64 addr = param + 0x80;
		DWORD64 rt = 0;
		if (!HackProcess.Read(addr, &rt, sizeof(rt)))
			return 0;
		return rt;

	}

	DWORD GetEntityCount()
	{
		DWORD64 param = GetULevel();
		DWORD rt = 0;
		DWORD64 addr = param + DEF_EntityCount;

		if (!HackProcess.Read(addr, &rt, sizeof(rt)))
			return 0;
		return rt;
	}
};


