#include "stdafx.h"
#include "Menu.h"
#include "Draw.h"

#include <D2D1.h>
#include "D3D11Renderer.h"
struct MyRectangle
{
	int Left;
	int Top;
	int Width;
	int Height;
};

 MyRectangle		rectangle;
extern IDWriteTextFormat*						pTextFormatSimSun;
extern GameOffsets									_GameOffsets;
MyChack Buttn_ESP;
MyChack Buttn_Health;
MyChack Buttn_LootESP;
MyChack Buttn_AimBot;
MyChack Buttn_CarESP;
MyChack Buttn_Chuanqiang;
MyChack Buttn_HackWeapon;
MyChack Buttn_DragItem;
MyChack Buttn_ShowWeapon;
MyChack Button_MagicHormone;

MyChack Button_Shift;

MyHealthStyleBtn		HealthStyle;
MyAimStyleBtn		AimStyle;
MyAimStyleSelect					AimStyleSelect;
MyHackWeaponStyleSelect HackWeaponStyle;

MyMenu MyMenuA;
MyRadio EspRadio;
MyRadio AimRadio;
MyRadio MiscRadio;
MyRadio SetRadio;

MyLootESPButton		LootStyle;
MyAimBotKey			AimBotKey;




POINT GetMousePoint()
{

		POINT MousePoint = POINT{};
		GetCursorPos(&MousePoint);
		MousePoint.x = MousePoint.x - rectangle.Left;
		MousePoint.y = MousePoint.y - rectangle.Top;
		return MousePoint;

}
DWORD changeArgb(DWORD color) {
	//int color = 0x1A0078ff;  
	int alpha = (color & 0xff000000) >> 24;
	int red = (color & 0x00ff0000) >> 16;
	int green = (color & 0x0000ff00) >> 8;
	int blue = (color & 0x000000ff);
	return D3DCOLOR_ARGB(alpha, red, green, blue);
}

BOOL dragTeam = false;
RECT borderSize;
RECT LastRect;
extern uItem *gMenuBuf;
Vector3 airdragvec;
extern DWORD64 lastaimtarget;
float g_timevalue = 10.0;
void worldToScreen3(float fov, Vector3 &WorldLocation, Rotator &Rotation, Vector3 &playerposition, float*sx, float*sy);
void  GetGameWindowRect()
{
	//::WriteProcessMemory(GetCurrentProcess(), (LPVOID)(_GameOffsets.BaseAddress + DEF_DEBUG_ADDR), "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10, 0);
	while (1)
	{
		
		if ((GetAsyncKeyState(VK_INSERT) && 0x8000))
		{
			MyMenuA.MyMenuHideShow = !MyMenuA.MyMenuHideShow;
		}
		if ((GetAsyncKeyState(VK_F1) && 0x8000))
		{

			Buttn_ESP.MyChackSelect = !Buttn_ESP.MyChackSelect;
		}
		if ((GetAsyncKeyState(VK_F2) && 0x8000))
		{
			Buttn_LootESP.MyChackSelect = !Buttn_LootESP.MyChackSelect;
		}
// 		if ((GetAsyncKeyState(VK_F3) && 0x8000))
// 		{
// 			Buttn_Health.MyChackSelect = !Buttn_Health.MyChackSelect;
// 		}
// 		if ((GetAsyncKeyState(VK_F4) && 0x8000))
// 		{
// 			Buttn_AimBot.MyChackSelect = !Buttn_AimBot.MyChackSelect;
// 		}
// 		if ((GetAsyncKeyState(VK_F5) && 0x8000))
// 		{
// 			Buttn_CarESP.MyChackSelect = !Buttn_CarESP.MyChackSelect;
// 		}
// 		if ((GetAsyncKeyState(VK_F6) && 0x8000))
// 		{
// 			HackWeaponStyle.MyButtonSelect++;
// 			if (HackWeaponStyle.MyButtonSelect > 2) HackWeaponStyle.MyButtonSelect = 0;
// 			//Buttn_HackWeapon.MyChackSelect = !Buttn_HackWeapon.MyChackSelect;
// 		}
// 		if ((GetAsyncKeyState(VK_F7) && 0x8000))
//  		{
// 			AimStyle.MyButtonSelect++;
// 			if (AimStyle.MyButtonSelect > 2)
// 				AimStyle.MyButtonSelect = 0;
//  		}
	
		//if ((GetAsyncKeyState(VK_F10) && 0x8000))
		//{
		//	SDK::HookMovementProcessEvent();
		//	//SDK::HookActorProcessEvent();
		////}
		//////	Buttn_DragItem.MyChackSelect = !Buttn_DragItem.MyChackSelect;
		//	MessageBeep(1);
		//////	
		//}
		//if ((GetAsyncKeyState(VK_F9) && 0x8000))
		//{

		//	//SDK::PrintLastTakeHitInfo();
		//	dragTeam = !dragTeam;

		////	//MessageBeep(1);
		////	SDK::HookEngineProcessEvent();
		//}

		//	DWORD64 apawn = _GameOffsets.GetAcknowledgedPawn();
		//	SDK::ChangeGravity(apawn);
		////	SDK::ChangeHealTime(apawn);
		//	
		//	//float tmp = *(float*)(apawn + 0x0098);
		//	//*(float*)(apawn + 0x0098) = g_timevalue;
		//	//g_timevalue = tmp;

		////	Vector3 vec;
		////	DWORD64 apawn = _GameOffsets.GetAcknowledgedPawn();
		////	SDK::GetLastHitBy(apawn, &vec);
		////	//SDK::HookHud();
		//}
		//if ((GetAsyncKeyState(VK_F10) && 0x8000))
		//{
		//	DWORD64 apawn = _GameOffsets.GetAcknowledgedPawn();
		//	SDK::SetItems(apawn);
		//}
		//if ((GetAsyncKeyState(VK_F11) && 0x8000))
		//{
		//	auto& gobjects = Classes::UObject::GetGlobalObjects();
		//	auto obj = gobjects.GetByIndex(26225);
		//	Log.printf("servernotifyhit %p\n", obj);
		//}
		//	if (lastaimtarget)
		//		SDK::ChangeInteraction(lastaimtarget);
		////	Vector3 vec;
		////	_GameOffsets.GetLocation(&vec);
		//	//SDK::TeleportMe(vec);

		//}
		//if ((GetAsyncKeyState(VK_F12) && 0x8000))
		//{
		//	::WriteProcessMemory(GetCurrentProcess(), (LPVOID)(g_dwGameBase + DEF_DEBUG_ADDR), "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10, 0);
		//	MessageBeep(1);
		//	//	DWORD64 apawn = _GameOffsets.GetAcknowledgedPawn();
		////	SDK::ChangeInteraction(apawn);
		//}
		if (MyMenuA.MyMenuHideShow)
		{

			MyMenuA.MyClickPoint(GetMousePoint());

			LootStyle.MyButtonClick(GetMousePoint());

			//AimBotKey.MyButtonClick(GetMousePoint());

			Buttn_ESP.MyChackClick(GetMousePoint());

			Buttn_CarESP.MyChackClick(GetMousePoint());

			Buttn_Health.MyChackClick(GetMousePoint());
			Buttn_LootESP.MyChackClick(GetMousePoint());
			Buttn_AimBot.MyChackClick(GetMousePoint());
			HealthStyle.MyButtonClick(GetMousePoint());
			AimStyle.MyButtonClick(GetMousePoint());
			AimStyleSelect.MyButtonClick(GetMousePoint());
			HackWeaponStyle.MyButtonClick(GetMousePoint());
			//Buttn_HackWeapon.MyChackClick(GetMousePoint());
			Buttn_ShowWeapon.MyChackClick(GetMousePoint());
			//Buttn_DragItem.MyChackClick(GetMousePoint());
		}
		RECT targetSize;
		GetClientRect(GameHwnd, &targetSize);
		if (LastRect.bottom != targetSize.bottom
			|| LastRect.left != targetSize.left
			|| LastRect.right != targetSize.right
			|| LastRect.top != targetSize.top)
		{
			Sleep(1000);
			LastRect = targetSize;
			//SetGameScreen();

		}
		Sleep(100);
	}
}

void MenuCreate(POINT MenuPoint)
{

	if (MyMenuA.MyMenuHideShow)
	{
		EspRadio.MyRadioSelect = true;
		AimRadio.MyRadioSelect = false;
		
		MyMenuA.MyCreateMenu(MenuPoint, 260, 500, COLORRED, L"WXY");


		Buttn_ESP.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 50, 15, 15, COLORORENGE, L"人物透视");

		Buttn_LootESP.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 80, 15, 15, COLORORENGE, L"物品透视");

		//Buttn_Health.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 110, 15, 15, COLORORENGE, gMenuBuf[TextHealth].ItemName);

		//Buttn_AimBot.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 140, 15, 15, COLORORENGE, gMenuBuf[TextAimBot].ItemName);

		//AimStyleSelect.MyCreateButton(MenuPoint.x + 18, MenuPoint.y + 160, 15, 15, COLORORENGE);

		//Buttn_CarESP.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 200, 15, 15, COLORORENGE, gMenuBuf[TextCarESP].ItemName);

		//HackWeaponStyle.MyCreateButton(MenuPoint.x + 25, MenuPoint.y + 230, 15, 15, COLORORENGE, gMenuBuf[Text变态自瞄].ItemName);
		//Buttn_HackWeapon.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 230, 15, 15, COLORORENGE, gMenuBuf[Text变态自瞄].ItemName);
		//HackWeaponStyle.MyCreateButton(MenuPoint.x + 18, MenuPoint.y + 220, 15, 15, COLORORENGE);

		//Buttn_Chuanqiang.MyChackSelect = true;
		//Buttn_Chuanqiang.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 260, 15, 15, COLORORENGE, gMenuBuf[Text人物穿墙].ItemName);

		//Buttn_ShowWeapon.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 290, 15, 15, COLORORENGE, gMenuBuf[Text行动加速].ItemName);

		//Button_MagicHormone.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 320, 15, 15, COLORORENGE, gMenuBuf[Text神秘激素].ItemName);


		//Button_Shift.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 350, 15, 15, COLORORENGE, "按shift自动锁头");
		//Buttn_DragItem.MyCreateChack(MenuPoint.x + 25, MenuPoint.y + 290, 15, 15, COLORORENGE, gMenuBuf[Text拉空投].ItemName);
 
	//	LootStyle.MyCreateButton(MenuPoint.x + 170, MenuPoint.y + 72, 15, 15, COLORORENGE);
		 
		//HealthStyle.MyCreateButton(MenuPoint.x + 170, MenuPoint.y + 102, 15, 15, COLORORENGE);

		//AimStyle.MyCreateButton(MenuPoint.x + 170, MenuPoint.y + 132, 15, 15, COLORORENGE);


		//AimBotKey.MyCreateButton(MenuPoint.x + 210, MenuPoint.y + 280, 33, 15, COLORORENGE);






	}

}