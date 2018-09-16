// FuckWXY.cpp : 定义 DLL 应用程序的导出函数。
//
#pragma once
#include "global.h"
#include "Draw.h"
#include "D3D11Renderer.h"
#include "string.h"

GameOffsets	_GameOffsets;
void InitGameBase()
{
	_GameOffsets.BaseAddress = (DWORD64)GetModuleHandle("WuXiaXClient-Win64-Shipping.exe");
	Log.printf(VMPDSA("GameBase %I64x\n"), _GameOffsets.BaseAddress);
}



void ShowPeople(float x, float y, int dc)
{
	char tmp[255] = { "0" };
	sprintf(tmp, "距离:%d", dc);
	WCHAR dctext[500] = { 0 };
	long    lLen = MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), dctext, sizeof(dctext));
	dctext[lLen] = '\0';
	DrawRenderText(x, y - 20, dctext, COLORWHITE, 14.0);
	DrawRenderText(x, y, L"人", COLORWHITE, 14.0);
}


void Showhorse(float x, float y, int dc)
{
	char tmp[255] = { "0" };
	sprintf(tmp, "距离:%d", dc);
	WCHAR dctext[500] = { 0 };
	long    lLen = MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), dctext, sizeof(dctext));
	dctext[lLen] = '\0';
	DrawRenderText(x, y - 20, dctext, COLORWHITE, 14.0);
	DrawRenderText(x, y, L"马", COLORGREEN, 14.0);
}
void Showitem(float x, float y, int dc,char *itemname)
{
	char tmp[255] = { "0" };
	sprintf(tmp, "距离:%d", dc);
	WCHAR dctext[500] = { 0 };
	long    lLen = MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), dctext, sizeof(dctext));
	dctext[lLen] = '\0';
	WCHAR wItemName[500] = { 0 };
	lLen = MultiByteToWideChar(CP_ACP, 0, itemname, strlen(itemname), wItemName, sizeof(wItemName));
	wItemName[lLen] = '\0';
	DrawRenderText(x, y - 20, dctext, COLORWHITE, 14.0);
	DrawRenderText(x, y, wItemName, COLORBLUE, 14.0);
}

void RenderGameHack()
{
	Rotator Rotation;
	_GameOffsets.GetControlRotation(&Rotation);
	Vector3 myroot;
	_GameOffsets.GetControlLocation(&myroot);

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
						//Log.printf("find player\n");
						//printf("%I64X\n", CurrentActor);
						//MessageBoxA(0, 0, 0, 0);
 						Vector3 vec;
						_GameOffsets.GetRelativeLocation(CurrentActor, &vec);
						//Log.printf("%f %f %f\n", vec.X, vec.Y, vec.Z);
						Vector3 root = vec;
						//Log.printf("Rotation %f %f %f\n", Rotation.Pitch, Rotation.Roll, Rotation.Yaw);
						//Log.printf("myroot %f %f %f\n", myroot.X, myroot.Y, myroot.Z);
						float x, y;
						//Rotation.Roll = 120;
						worldToScreen3(90.0, root,Rotation, myroot, &x, &y);

						int dc = Distance(&root, &myroot);
						ShowPeople(x, y, dc);
					//	Log.printf("X %f Y %f\n", x, y);
						
					}
					if (!strcmp(strobjname, "BP_WuXiaHorse_C"))
					{
						Vector3 vec;
						_GameOffsets.GetRelativeLocation(CurrentActor, &vec);
						Vector3 root = vec;
						float x, y;
						worldToScreen3(90.0, root, Rotation, myroot, &x, &y);

						int dc = Distance(&root, &myroot);
		
						Showhorse(x, y, dc);
					}
					if (!strcmp(strobjname, "PickItem"))
					{
						DWORD dwItemID = _GameOffsets.GetItemId(CurrentActor);
						std::string strItemName;
;						if (dwItemID)
						{
							switch (dwItemID)
							{
							case 110003:
								strItemName = "纱布";
								break;
							case 110004:
								strItemName = "解毒药";
								break;
							case 110005:
								strItemName = "金疮药";
								break;
							case 110006:
								strItemName = "续命丸";
								break;
							case 110007:
								strItemName = "大补丸";
								break;
							case 110008:
								strItemName = "华佗药箱";
					
								break;

							default:
								break;
							}
						}

Vector3 vec;
_GameOffsets.GetRelativeLocation(CurrentActor, &vec);
Vector3 root = vec;
float x, y;
worldToScreen3(90.0, root, Rotation, myroot, &x, &y);

int dc = Distance(&root, &myroot);
if (strItemName.length()!=0)
{
	Showitem(x, y, dc, (char*)strItemName.c_str());
}




					}
				}
			}
		}
	}
}