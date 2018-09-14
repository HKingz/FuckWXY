// FuckWXY.cpp : 定义 DLL 应用程序的导出函数。
//
#pragma once
#include "global.h"
GameOffsets	_GameOffsets;
void InitGameBase()
{
	_GameOffsets.BaseAddress = (DWORD64)GetModuleHandle("WuXiaXClient-Win64-Shipping.exe");
	Log.printf(VMPDSA("GameBase %I64x\n"), _GameOffsets.BaseAddress);
}

void RenderGameHack()
{
	Rotator Rotation;
	_GameOffsets.GetControlRotation(&Rotation);
	Vector3 myroot;
	_GameOffsets.GetControlLocation(&myroot);
	Log.printf("%f %f %f\n", Rotation.Pitch, Rotation.Roll, Rotation.Yaw);
	Log.printf("%f %f %f\n", myroot.X, myroot.Y, myroot.Z);
	DWORD dwCount = _GameOffsets.GetEntityCount();
	DWORD64 dwHeader = _GameOffsets.GetEntityList();
	
	for (int i = 0; i < dwCount; i++)
	{
		
	
		DWORD64 CurrentActor = 0;
		if (HackProcess.Read(dwHeader + (i * 0x8), &CurrentActor, sizeof(CurrentActor)))
		{
			//Log.printf("CurrentActor %I64x \n", CurrentActor);
			int IDnumber = 0;
			if (HackProcess.Read(CurrentActor + 0x18, &IDnumber, sizeof(int)))
			{
				//Log.printf("IDNumber %d\n", IDnumber);
				if (IDnumber > 1 && IDnumber < 1000000)
				{
					
					char strobjname[200] = { "0" };
					_GameOffsets.GetObjectName(strobjname, IDnumber);
					//Log.printf("%s\n", strobjname);
					if (!strcmp(strobjname, "BP_WuXiaX_Character_C") || !strcmp(strobjname, "BP_FakePlayer_C"))
					{
						Log.printf("find player\n");
						//printf("%I64X\n", CurrentActor);
						//MessageBoxA(0, 0, 0, 0);
 						Vector3 vec;
						_GameOffsets.GetRelativeLocation(CurrentActor, &vec);
						Log.printf("%f %f %f\n", vec.X, vec.Y, vec.Z);
						Vector3 root = vec;
			
						float x, y;
						worldToScreen3(90, root,Rotation, myroot, &x, &y);
						Log.printf("X %f Y %f\n", x, y);
						
					}
// 					HPSets hp;
// 					_GameOffsets.GetActorHP(CurrentActor, &hp);
// 					Log.printf("hp %f\n", hp.Health);
				}
			}
		}
	}
}