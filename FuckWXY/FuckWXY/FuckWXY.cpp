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
	Rotator vec;
	_GameOffsets.GetControlPos(&vec);
	DWORD dwCount = _GameOffsets.GetEntityCount();
	printf("%f %f %f %d\n", vec.Pitch ,vec.Roll, vec.Yaw, dwCount);
}