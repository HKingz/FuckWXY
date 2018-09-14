#include "stdafx.h"
#include "Draw.h"

#include "global.h"
#include <list>
#include <D2D1.h>
#include "D3D11Renderer.h"
extern D3D11Renderer *pMyD3D11Reader;
using namespace std;
extern uItem *gMenuBuf;
#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}
ID2D1HwndRenderTarget*	pRender;
extern RECT										MyRect;	
IDWriteFactory*					pWriteFactory;
extern ID2D1SolidColorBrush*		pBrush;
 ID2D1SolidColorBrush*					DrawBrush=NULL;
 ID2D1LinearGradientBrush*			DrawGradientBrush=NULL;


extern IDWriteTextFormat*				pTextFormatMsyh;
IDWriteTextFormat*						pTextFormatKaiTi=NULL;
IDWriteTextFormat*						pTextFormatSimSun = NULL;
IDWriteTextFormat*						pTextFormatHealth = NULL;

IDWriteTextFormat*						pTextFormatItem = NULL;
IDWriteTextFormat*						pTextFormatItemBig = NULL;
IDWriteTextFormat*						pTextFormatItemMid = NULL;
IDWriteTextFormat*						pTextFormatItemTiny = NULL;

IDWriteTextFormat*						pTextFormatESP = NULL;
void DarwRelease()
{
	SAFE_RELEASE(DrawBrush);
	SAFE_RELEASE(pTextFormatKaiTi);
	SAFE_RELEASE(DrawGradientBrush);
}


bool TextFormatInit()//创建字体
{
	if (!pTextFormatESP)
	{
		pWriteFactory->CreateTextFormat(L"SimSun",
			NULL,
			DWRITE_FONT_WEIGHT_MEDIUM,    // 粗细
			DWRITE_FONT_STYLE_NORMAL,      // 斜体
			DWRITE_FONT_STRETCH_NORMAL,    // Stretch
			12.0f,                         // Size    
			L"MyFont",                      // Local
			&pTextFormatESP);
	}

	if (!pTextFormatItem)
	{
		pWriteFactory->CreateTextFormat(L"SimSun",
			NULL,
			DWRITE_FONT_WEIGHT_BLACK,    // 粗细
			DWRITE_FONT_STYLE_NORMAL,      // 斜体
			DWRITE_FONT_STRETCH_NORMAL,    // Stretch
			16.0f,                         // Size    
			L"MyFont",                      // Local
			&pTextFormatItem);
	}
	if (!pTextFormatItemBig)
	{
		pWriteFactory->CreateTextFormat(L"SimSun",
			NULL,
			DWRITE_FONT_WEIGHT_BLACK,    // 粗细
			DWRITE_FONT_STYLE_NORMAL,      // 斜体
			DWRITE_FONT_STRETCH_NORMAL,    // Stretch
			16.0f,                         // Size    
			L"MyFont",                      // Local
			&pTextFormatItemBig);
	}
	if (!pTextFormatItemMid)
	{
		pWriteFactory->CreateTextFormat(L"SimSun",
			NULL,
			DWRITE_FONT_WEIGHT_BLACK,    // 粗细
			DWRITE_FONT_STYLE_NORMAL,      // 斜体
			DWRITE_FONT_STRETCH_NORMAL,    // Stretch
			14.0f,                         // Size    
			L"MyFont",                      // Local
			&pTextFormatItemMid);
	}
	if (!pTextFormatItemTiny)
	{
		pWriteFactory->CreateTextFormat(L"SimSun",
			NULL,
			DWRITE_FONT_WEIGHT_BLACK,    // 粗细
			DWRITE_FONT_STYLE_NORMAL,      // 斜体
			DWRITE_FONT_STRETCH_NORMAL,    // Stretch
			12.0f,                         // Size    
			L"MyFont",                      // Local
			&pTextFormatItemTiny);
	}

	if (!pTextFormatKaiTi)
	{
		pWriteFactory->CreateTextFormat(L"KaiTi",
			NULL,
			::DWRITE_FONT_WEIGHT_BLACK,    // 粗细
			DWRITE_FONT_STYLE_NORMAL,      // 斜体
			DWRITE_FONT_STRETCH_NORMAL,    // Stretch
			19.0f,                         // Size    
			L"MyFont",                      // Local
			&pTextFormatKaiTi);
	}
	if (!pTextFormatSimSun)
	{
		pWriteFactory->CreateTextFormat(L"SimSun",
			NULL,
			DWRITE_FONT_WEIGHT_MEDIUM,    // 粗细
			DWRITE_FONT_STYLE_NORMAL,      // 斜体
			DWRITE_FONT_STRETCH_NORMAL,    // Stretch
			18.0f,                         // Size    
			L"MyFont",                      // Local
			&pTextFormatSimSun);
	}
	if (!pTextFormatHealth)
	{
		pWriteFactory->CreateTextFormat(L"Msyh",
			NULL,
			DWRITE_FONT_WEIGHT_MEDIUM,    // 粗细
			DWRITE_FONT_STYLE_NORMAL,      // 斜体
			DWRITE_FONT_STRETCH_NORMAL,    // Stretch
			12.0f,                         // Size    
			L"MyFont",                      // Local
			&pTextFormatHealth);
	}
	return false;
}
bool SolidColorBrushInit()//创建固色画刷
{
	if (!DrawBrush)
	{
		pRender->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &DrawBrush);
		return true;
	}
	return false;
}

bool LinearGradientBrushInit(int X, int Y, int W, int H, D3DCOLOR Color, D3DCOLOR Color2, float Alpha)//创建变色画刷
{
	SAFE_RELEASE(DrawGradientBrush);
	if (!DrawGradientBrush)
	{
		D2D1_GRADIENT_STOP GRADIENT_STOP[2];
		ID2D1GradientStopCollection* GradientStopCollection = NULL;
		GRADIENT_STOP[0].color = D2D1::ColorF(D2D1::ColorF(Color, Alpha));
		GRADIENT_STOP[0].position = 0.f;
		GRADIENT_STOP[1].color = D2D1::ColorF(D2D1::ColorF(Color2, Alpha));
		GRADIENT_STOP[1].position = 1.f;
		pRender->CreateGradientStopCollection(GRADIENT_STOP, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &GradientStopCollection);
		pRender->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(
				D2D1::Point2F((float)X, (float)Y),
				D2D1::Point2F(float(W), float(H))),
			GradientStopCollection,
			&DrawGradientBrush
		);
		return true;
	}
	return false;
}
void AddTimedText(WCHAR*text, DWORD time,DWORD color)//文字
{
	
	pMyD3D11Reader->AddTimedText(text,time, color);
}
void DrawRenderText(int X, int Y,  WCHAR* text, D3DCOLOR Color, float size)//文字
{

	pMyD3D11Reader->RenderText(X, Y, Color, size, text);

}
void DrawRenderText2(int X, int Y, WCHAR* text, D3DCOLOR Color, float size)//文字
{
	pMyD3D11Reader->RenderText2(X, Y, Color, size, text);
}
void DrawLine(int X, int Y, int XX, int YY, D3DCOLOR Color, float Width , float Alpha)//直线
{
	
	pMyD3D11Reader->DrawLine(X, Y, XX, YY, Color);
}
void DrawTimedLine(int X, int Y, int XX, int YY, D3DCOLOR Color, DWORD time,float Width , float Alpha )
{
	pMyD3D11Reader->DrawTimedLine(X, Y, XX, YY, Color,time);
}
void DrawFillRectangle(float X, float Y, int W, int H ,D3DCOLOR color, float Alpha)//填充矩形
{
	
	pMyD3D11Reader->FillRect(X, Y, W, H, color);
}

void DrawRectangle(float X, float Y, float W, float H, D3DCOLOR Color, float Width, float Alpha )//矩形
{
	pMyD3D11Reader->DrawBox(X, Y, W, H, 0, Color);
}

void DrawFillRoundedRectangle(int X, int Y, int W, int H, float RadianX, float RadianY, D3DCOLOR Color, float Alpha = 1.0f)//填充圆角
{
	pMyD3D11Reader->FillRect(X, Y, W, H, Color);
	//DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
	//pRender->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF((float)X, (float)Y, float(X + W), float(Y + H)),
	//	RadianX, RadianY), DrawBrush);
}
//void DrawFillRoundedRectangleGradient(int X, int Y, int W, int H, float RadianX, float RadianY, D3DCOLOR Color, D3DCOLOR Color2,float Alpha = 1.0f)//填充变色圆角
//{
//	LinearGradientBrushInit(X, Y, W, H, Color, Color2, Alpha);
//	pRender->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF((float)X, (float)Y, float(X + W), float(Y + H)),
//		RadianX, RadianY), DrawGradientBrush);
//}
//void DrawRoundedRectangle(int X, int Y, int W, int H, float RadianX, float RadianY, D3DCOLOR Color, float Width = 1.0f, float Alpha = 1.0f)//圆角
//{
//	DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
//	pRender->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF((float)X, (float)Y, float(W), float(H)), RadianX, RadianY), DrawBrush, Width);
//}
//void DrawRoundedRectangleGradient(int X, int Y, int W, int H, float RadianX, float RadianY, D3DCOLOR Color, D3DCOLOR Color2, float Width = 1.0f, float Alpha = 1.0f)//变色圆角
//{
//	LinearGradientBrushInit(X, Y, W, H, Color, Color2, Alpha);
//	pRender->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF((float)X, (float)Y, float(W), float(H)), RadianX, RadianY), DrawGradientBrush, Width);
//}
//void DrawFillRound(float X, float Y, float Radius, D3DCOLOR Color, float Alpha = 1.0f)//填充圆
//{
//	DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
//	pRender->FillEllipse(D2D1::Ellipse(D2D1::Point2F(X, Y), Radius, Radius), DrawBrush);
//}
//void DrawRound(float X, float Y, float Radius, D3DCOLOR Color, float Width = 1.0f, float Alpha = 1.0f)//圆
//{
//	DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
//	pRender->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(X, Y), Radius, Radius), DrawBrush, Width);
//}
//void DrawEllipse(float X, float Y, float RadiusX, float RadiusY, D3DCOLOR Color, float Width, float Alpha )//椭圆
//{
//	DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
//	pRender->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(X, Y), RadiusX, RadiusY), DrawBrush, Width);
//}
//void DrawFillEllipse(float X, float Y, float RadiusX, float RadiusY, D3DCOLOR Color, float Alpha = 1.0f)//填充椭圆
//{
//	DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
//	pRender->FillEllipse(D2D1::Ellipse(D2D1::Point2F(X, Y), RadiusX, RadiusY), DrawBrush);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////							以下为自定义                   ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawProgressBar(int X, int Y, int W, int H, int Value, int MaxValue, D3DCOLOR Color)//进度条
{
	int progress = (int)((float)Value / ((float)MaxValue / 100));
	int w = (int)((float)W / 100 * progress);
	D3DCOLOR color = D3DCOLOR_RGBA(255, 0, 0, 255);
	DrawFillRoundedRectangle(X, Y - 1, W + 1, H + 2, 15, 15, Color, 0);
	if (w >= 2)DrawFillRoundedRectangle(X + 1, Y, w - 1, H, 15, 15, color);
}

void DrawHalfRectangle(float X, float Y, float W, float H, D3DCOLOR Color, float Width, float Alpha)//断式矩形
{
	float DivW =W/4.5;
	float DivH = H/3.5;

	DrawLine(X, Y,	X + DivW, Y, Color, Width, Alpha);
 	DrawLine(X, Y, X,	Y + DivH, Color, Width, Alpha);

	DrawLine(X + W - DivW, Y, X + W , Y, Color, Width, Alpha);
	DrawLine(X + W, Y, X + W, Y + DivH, Color, Width, Alpha);

	DrawLine(X , Y + H, X + DivW, Y + H, Color, Width, Alpha);
	DrawLine(X , Y + H, X  , Y +H- DivH, Color, Width, Alpha);

	DrawLine(X + W - DivW, Y + H, X + W, Y + H, Color, Width, Alpha);
 	DrawLine(X + W, Y + H, X + W, Y + H - DivH, Color, Width, Alpha);
}
void DrawTimedHalfRectangle(float X, float Y, float W, float H, D3DCOLOR Color, DWORD time, float Width , float Alpha )
{
	float DivW = W / 4.5;
	float DivH = H / 3.5;

	DrawTimedLine(X, Y, X + DivW, Y, Color, Width, Alpha);
	DrawTimedLine(X, Y, X, Y + DivH, Color, Width, Alpha);

	DrawTimedLine(X + W - DivW, Y, X + W, Y, Color, Width, Alpha);
	DrawTimedLine(X + W, Y, X + W, Y + DivH, Color, Width, Alpha);

	DrawTimedLine(X, Y + H, X + DivW, Y + H, Color, Width, Alpha);
	DrawTimedLine(X, Y + H, X, Y + H - DivH, Color, Width, Alpha);

	DrawTimedLine(X + W - DivW, Y + H, X + W, Y + H, Color, Width, Alpha);
	DrawTimedLine(X + W, Y + H, X + W, Y + H - DivH, Color, Width, Alpha);
}
void DrawVerticalHealth(bool isVis, float Health, float MaxHealth, int X, int Y, int W, int H)//高要负数
{
	//if (H >= -20)
	//	H = -21;

	int progress = (int)((float)Health / ((float)MaxHealth / 100));
	int h = (int)((float)H / 100 * progress);

	D3DCOLOR color = D3DCOLOR_RGBA(106, 90, 205, 255);
	if (isVis)
	{
		color = D3DCOLOR_RGBA(255, 0, 0, 255);
		if (progress >= 20) color = D3DCOLOR_RGBA(255, 165, 0, 255);
		if (progress >= 40) color = D3DCOLOR_RGBA(255, 255, 0, 255);
		if (progress >= 60) color = D3DCOLOR_RGBA(173, 255, 47, 255);
		if (progress >= 80) color = D3DCOLOR_RGBA(0, 255, 0, 255);
	}
	DrawFillRoundedRectangle(X, Y - 1, W, H, 3, 1, D3DCOLOR_RGBA(0, 0, 0, 255));//最大血量黑色
	DrawFillRoundedRectangle(X + 1, Y, W, h, 3, 1, color, 0.8);//当前血量

// 	char text[20];
// 	sprintf_s(text, "HP: %d", (INT)Health);
// 	DrawRenderText(X-10, Y, text, color, pTextFormatHealth);
}
void DrawTransverseHealth(bool isVis,float Health, float MaxHealth, int X, int Y, int W, int H)
{

	if (W <= 30)
		W = 31;
	int progress = (int)((float)Health / ((float)MaxHealth / 100));
	int w = (int)((float)W / 100 * progress);
	D3DCOLOR color = D3DCOLOR_RGBA(106, 90, 205,255);
	if (isVis)
	{
			color = D3DCOLOR_RGBA(255, 0, 0, 255);
			if (progress >= 20) color = D3DCOLOR_RGBA(255, 165, 0, 255);
			if (progress >= 40) color = D3DCOLOR_RGBA(255, 255, 0, 255);
			if (progress >= 60) color = D3DCOLOR_RGBA(173, 255, 47, 255);
			if (progress >= 80) color = D3DCOLOR_RGBA(0, 255, 0, 255);
	}

	DrawFillRoundedRectangle(X, Y - 1, W, H, 3, 1, D3DCOLOR_RGBA(0, 0, 0, 255));//最大血量黑色
	DrawFillRoundedRectangle(X + 1, Y, w, H, 3, 1, color);//当前血量
}
 




void MyChack::MyCreateChack(int X, int Y, int W, int H, D3DCOLOR Color,WCHAR* Text)
{
	MyChackX = X;
	MyChackY = Y;
	MyChackW = W;
	MyChackH = H;
	DrawRectangle(X - 1, Y - 1, W + 2, H + 2, D2D1::ColorF::Tomato,1.5);
	DrawRectangle(X, Y, W, H, Color,1.5);
	if (MyChackSelect)
	{
		DrawFillRectangle(X+2, Y+2, W-4, H-4, Color,1.5);
	}
	DrawRenderText(X + 18, Y - 3, Text,COLORBLACK ,14.0);
	DrawRenderText(X+20, Y-5, Text, COLORWHITE,14.0);
}

void MyChack::MyChackClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
		if (ClickMouse&&	
		((MyChackX < MousePoint.x) && ((MyChackX+MyChackW) > MousePoint.x))	&&
		((MyChackY < MousePoint.y) && ((MyChackH+ MyChackY) > MousePoint.y))
		)
	{
			MyChackSelect = !MyChackSelect;


	}
}
POINT MenuPoint;
void MyMenu::MyCreateMenu(POINT MenuPoint ,int W, int H, D3DCOLOR Color, WCHAR* Text)
{
	MyMenuX = MenuPoint.x;
	MyMenuY = MenuPoint.y;
	MyMenuW = W;
	MyMenuH = H;
	if (MyMenuHideShow)
	{
		DrawFillRoundedRectangle(MyMenuX, MyMenuY, W, 25, 7, 7, Color);
		DrawFillRectangle(MyMenuX, MyMenuY + 10, W, 15, Color);
//		DrawFillRectangle(MyMenuX, MyMenuY + 10, 111, H+3, Color);
		DrawLine(MyMenuX + 1, MyMenuY + 25, MyMenuX + 1, MyMenuY + H + 12, Color);//左竖
		DrawLine(MyMenuX + W - 1, MyMenuY + 25, MyMenuX + W - 1, MyMenuY + H + 12, Color);//右竖
		DrawLine(MyMenuX, MyMenuY + H + 13, MyMenuX + W, MyMenuY + H + 13, Color);
		//DrawFillRectangle(MyMenuX + 1, MyMenuY + 13, W - 2, H, D2D1::ColorF::Black, 0.6f);
		DrawRenderText(MyMenuX + 13, MyMenuY, Text, COLORBLACK,14.0);
		DrawRenderText(MyMenuX + 15, MyMenuY - 2, Text, COLORWHITE,14.0);

	}
}
extern	POINT MenuPoint;
			POINT DragPoint;
void MyMenu::MyClickPoint(POINT MousePoint)
{
	SHORT ClickMouse = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
	if (ClickMouse)
	{
		if (!DragMenu&&((MyMenuX < MousePoint.x) && ((MyMenuX + MyMenuW) > MousePoint.x)) &&
			((MyMenuY < MousePoint.y) && ((25 + MyMenuY) > MousePoint.y))
			)
	{

		if (DragPoint.y == 0 | DragPoint.x == 0)
				{
					DragPoint.y = MousePoint.y - MenuPoint.y;
					DragPoint.x = MousePoint.x - MenuPoint.x;
				}
				DragMenu = true;		
		}

		if (DragMenu == true)
		{
			MenuPoint.y = MousePoint.y - DragPoint.y;
			MenuPoint.x = MousePoint.x - DragPoint.x;
		}
	}
	else
	{
		DragMenu = false;
		DragPoint = POINT{};
	}

	
}
void MyRadio::MyCreateRadio(int X, int Y, int W, int H, D3DCOLOR Color, WCHAR* Text) 
{
	MyRadioX = X;
	MyRadioY = Y;
	MyRadioW = W;
	MyRadioH = H;
	//DrawRectangle(X - 1, Y - 1, W + 2, H + 2, Color,0.7);
	DrawFillRectangle(X, Y, W, H, Color);

	if (!MyRadioSelect)
	{
		DrawFillRectangle(X, Y, W, H, D2D1::ColorF::Black, 0.6f);
	}
	DrawRenderText(X+4, Y + (H/5)+2, Text,COLORBLACK, 14.0);
	DrawRenderText(X+6, Y +(H/5), Text, COLORWHITE, 14.0);
}
void MyLootESPButton::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color)
{

	MyButtonX = X + 6;
	MyButtonY = Y + (H / 2);
	MyButtonW = W;
	MyButtonH = H;
	DrawFillRectangle(MyButtonX, MyButtonY, W, H, Color);
	DrawLine (MyButtonX+11 , MyButtonY+1, MyButtonX+3, MyButtonY+8, D2D1::ColorF::White,2.5);
	DrawLine(MyButtonX+3 , MyButtonY + 8, MyButtonX+11, MyButtonY + 14, D2D1::ColorF::White, 2.5);
	DrawFillRectangle(MyButtonX+50, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 53, MyButtonY + 1, MyButtonX + 62, MyButtonY + 8, D2D1::ColorF::White, 3);
	DrawLine(MyButtonX + 62, MyButtonY + 8, MyButtonX + 53, MyButtonY + 14, D2D1::ColorF::White, 3);


	WCHAR output[10];
	swprintf(output, L"%d", LootESPRange);
	DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, output, COLORBLACK, 14.0);
	DrawRenderText(MyButtonX + 24, Y + (H / 5), output, COLORWHITE, 14.0);

	//if (MyButtonSelect)
	//{
	//	DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, "横", D2D1::ColorF::Black, pTextFormatSimSun);
	//	DrawRenderText(MyButtonX + 24, Y + (H / 5), "横", D2D1::ColorF::White, pTextFormatSimSun);
	//}
	//if (!MyButtonSelect)
	//{
	//	DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, "竖", D2D1::ColorF::Black, pTextFormatSimSun);
	//	DrawRenderText(MyButtonX + 24, Y + (H / 5), "竖", D2D1::ColorF::White, pTextFormatSimSun);
	//}

}
void ::MyLootESPButton::MyButtonClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
	if (ClickMouse &&
			((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
			((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
			)
		{
			LootESPRange--;
			char output[10];
			sprintf(output, "%d", LootESPRange);
			//WritePrivateProfileString(VMPDSA("ESP"), VMPDSA("range"), output, inipath);

		}
	if (ClickMouse &&
		((MyButtonX+50 < MousePoint.x) && ((MyButtonX+50 + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		LootESPRange++;
		char output[10];
		sprintf(output, "%d", LootESPRange);
	//	WritePrivateProfileString(VMPDSA("ESP"), VMPDSA("range"), output, inipath);

	}
}
void MyAimBotHeight::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color)
{
	MyButtonX = X + 6;
	MyButtonY = Y + (H / 2);
	MyButtonW = W;
	MyButtonH = H;
	DrawFillRectangle(MyButtonX, MyButtonY, W, H, Color);
	DrawLine(MyButtonX  + MyButtonW / 2, MyButtonY + 1, MyButtonX + 2, MyButtonY + 12, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX  + MyButtonW / 2, MyButtonY + 1,  MyButtonX + MyButtonW-2 , MyButtonY + 12, D2D1::ColorF::White, 2.5);
	DrawFillRectangle(MyButtonX , MyButtonY+50, W, H, Color);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY +	64, MyButtonX + 2, MyButtonY + 53, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 64, MyButtonX + MyButtonW - 2, MyButtonY + 53, D2D1::ColorF::White, 2.5);
	
}
void MyAimBotHeight::MyButtonClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyHeightInt += 1;

	}
	if (ClickMouse &&
		((MyButtonX  < MousePoint.x) && ((MyButtonX  + MyButtonW) > MousePoint.x)) &&
		((MyButtonY + 50 < MousePoint.y) && ((MyButtonH + MyButtonY + 50) > MousePoint.y))
		)
	{
		MyHeightInt -= 1;
	
	}
}
void MyAimBotKey::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color)
{
	MyButtonX = X + 6;
	MyButtonY = Y + (H / 2);
	MyButtonW = W;
	MyButtonH = H;
	DrawFillRectangle(MyButtonX, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 1, MyButtonX + 2, MyButtonY + 12, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 1, MyButtonX + MyButtonW - 2, MyButtonY + 12, D2D1::ColorF::White, 2.5);
	DrawFillRectangle(MyButtonX, MyButtonY + 50, W, H, Color);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 64, MyButtonX + 2, MyButtonY + 53, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 64, MyButtonX + MyButtonW - 2, MyButtonY + 53, D2D1::ColorF::White, 2.5);
	WCHAR A[100];
	//char output[50];
	//sprintf(output, "KeyCode %d", MyHeightInt);
	//if (MyHeightInt == 1)
	//{
	//	DrawRenderText(MyButtonX - 73, MyButtonY + 19, output, COLORWHITE,14.0);
	//	return;
	//}
	//if (MyHeightInt == 2)
	//{
	//	DrawRenderText(MyButtonX - 73, MyButtonY + 19, output, COLORWHITE, 14.0);
	//	return;
	//}
	//if (MyHeightInt == 4)
	//{
	//	DrawRenderText(MyButtonX - 73, MyButtonY + 19, output, COLORWHITE, 14.0);
	//	return;
	//}
	//sprintf_s(A, "KeyCode:  %d", MyHeightInt);
	DrawRenderText(MyButtonX - 73, MyButtonY + 19, A, COLORWHITE, 14.0);
}
void MyAimBotKey::MyButtonClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		if (MyHeightInt == 0xfe)
		{
			return;
		}
		MyHeightInt += 1;

	}
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY + 50 < MousePoint.y) && ((MyButtonH + MyButtonY + 50) > MousePoint.y))
		)
	{
		if (MyHeightInt == 1)
		{
			return;
		}
		MyHeightInt -= 1;

	}
}

void MyAimBotSpeed::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color)
{
 }
void MyAimBotSpeed::MyButtonClick(POINT MousePoint)
{
 
}
void MyChangeButton::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color,WCHAR*Text)
{
	MyButtonX = X + 6;
	MyButtonY = Y + (H / 2);
	MyButtonW = W;
	MyButtonH = H;
	DrawFillRectangle(MyButtonX, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 1, MyButtonX + 2, MyButtonY + 12, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 1, MyButtonX + MyButtonW - 2, MyButtonY + 12, D2D1::ColorF::White, 2.5);
	DrawFillRectangle(MyButtonX, MyButtonY + 50, W, H, Color);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 64, MyButtonX + 2, MyButtonY + 53, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + MyButtonW / 2, MyButtonY + 64, MyButtonX + MyButtonW - 2, MyButtonY + 53, D2D1::ColorF::White, 2.5);
	WCHAR A[100];
	swprintf_s(A, L"%d", MyHeightInt);
	DrawRenderText(MyButtonX - 83, MyButtonY + 19, Text, COLORWHITE, 14.0);
	DrawRenderText(MyButtonX + 13, MyButtonY + 20, A, COLORWHITE, 14.0);
}


void MyAimStyleSelect::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color)
{


	MyButtonX = X + 6;
	MyButtonY = Y + (H / 2);
	MyButtonW = W;
	MyButtonH = H;
	DrawFillRectangle(MyButtonX, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 11, MyButtonY + 1, MyButtonX + 3, MyButtonY + 8, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + 3, MyButtonY + 8, MyButtonX + 11, MyButtonY + 14, D2D1::ColorF::White, 2.5);
	DrawFillRectangle(MyButtonX + 150, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 153, MyButtonY + 1, MyButtonX + 162, MyButtonY + 8, D2D1::ColorF::White, 3);
	DrawLine(MyButtonX + 162, MyButtonY + 8, MyButtonX + 153, MyButtonY + 14, D2D1::ColorF::White, 3);

	if (MyButtonSelect)
	{
		//DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, gMenuBuf[Text准星附近优先].ItemName, COLORBLACK, 14.0);
		//DrawRenderText(MyButtonX + 24, Y + (H / 5), gMenuBuf[Text准星附近优先].ItemName, COLORWHITE, 14.0);
	}
	else
	{
		//DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, gMenuBuf[Text离自己最近优先].ItemName, COLORBLACK, 14.0);
		//DrawRenderText(MyButtonX + 24, Y + (H / 5), gMenuBuf[Text离自己最近优先].ItemName, COLORWHITE, 14.0);
	}
}
void MyAimStyleSelect::MyButtonClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyButtonSelect = !MyButtonSelect;

	}
	if (ClickMouse &&
		((MyButtonX + 150 < MousePoint.x) && ((MyButtonX + 150 + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyButtonSelect = !MyButtonSelect;

	}
}


void MyHackWeaponStyleSelect::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color)
{


	MyButtonX = X + 6;
	MyButtonY = Y + (H / 2);
	MyButtonW = W;
	MyButtonH = H;
	DrawFillRectangle(MyButtonX, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 11, MyButtonY + 1, MyButtonX + 3, MyButtonY + 8, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + 3, MyButtonY + 8, MyButtonX + 11, MyButtonY + 14, D2D1::ColorF::White, 2.5);
	DrawFillRectangle(MyButtonX + 150, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 153, MyButtonY + 1, MyButtonX + 162, MyButtonY + 8, D2D1::ColorF::White, 3);
	DrawLine(MyButtonX + 162, MyButtonY + 8, MyButtonX + 153, MyButtonY + 14, D2D1::ColorF::White, 3);

	switch (MyButtonSelect)
	{
	case 0:
	{
		//DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, gMenuBuf[Text16].ItemName, COLORBLACK, 14.0);
		//DrawRenderText(MyButtonX + 24, Y + (H / 5), gMenuBuf[Text16].ItemName, COLORWHITE, 14.0);
		break;
	}
	case 1:
	{
		//DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, gMenuBuf[Text17].ItemName, COLORBLACK, 14.0);
		//DrawRenderText(MyButtonX + 24, Y + (H / 5), gMenuBuf[Text17].ItemName, COLORWHITE, 14.0);
		break;
	}
	case 2:
	{
		//DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, gMenuBuf[Text18].ItemName, COLORBLACK, 14.0);
		//DrawRenderText(MyButtonX + 24, Y + (H / 5), gMenuBuf[Text18].ItemName, COLORWHITE, 14.0);
	}
	}

}
void MyHackWeaponStyleSelect::MyButtonClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyButtonSelect ++;
		if (MyButtonSelect > 2) MyButtonSelect = 0;

	}
	if (ClickMouse &&
		((MyButtonX + 150 < MousePoint.x) && ((MyButtonX + 150 + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyButtonSelect --;
		if (MyButtonSelect < 0) MyButtonSelect = 2;

	}
}
void MyHealthStyleBtn::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color)
{


	MyButtonX = X + 6;
	MyButtonY = Y + (H / 2);
	MyButtonW = W;
	MyButtonH = H;
	DrawFillRectangle(MyButtonX, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 11, MyButtonY + 1, MyButtonX + 3, MyButtonY + 8, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + 3, MyButtonY + 8, MyButtonX + 11, MyButtonY + 14, D2D1::ColorF::White, 2.5);
	DrawFillRectangle(MyButtonX + 50, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 53, MyButtonY + 1, MyButtonX + 62, MyButtonY + 8, D2D1::ColorF::White, 3);
	DrawLine(MyButtonX + 62, MyButtonY + 8, MyButtonX + 53, MyButtonY + 14, D2D1::ColorF::White, 3);

	if (MyButtonSelect)
	{
		DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, L"--", COLORBLACK, 14.0);
		DrawRenderText(MyButtonX + 24, Y + (H / 5), L"--", COLORWHITE, 14.0);
	}
	else
	{
		DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, L"|", COLORBLACK, 14.0);
		DrawRenderText(MyButtonX + 24, Y + (H / 5), L"|", COLORWHITE, 14.0);
	}
}
void MyHealthStyleBtn::MyButtonClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyButtonSelect = !MyButtonSelect;

	}
	if (ClickMouse &&
		((MyButtonX + 50 < MousePoint.x) && ((MyButtonX + 50 + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyButtonSelect = !MyButtonSelect;
		
	}
}
extern uItem *gMenuBuf;
void MyAimStyleBtn::MyCreateButton(int X, int Y, int W, int H, D3DCOLOR Color)
{


	MyButtonX = X + 6;
	MyButtonY = Y + (H / 2);
	MyButtonW = W;
	MyButtonH = H;
	DrawFillRectangle(MyButtonX, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 11, MyButtonY + 1, MyButtonX + 3, MyButtonY + 8, D2D1::ColorF::White, 2.5);
	DrawLine(MyButtonX + 3, MyButtonY + 8, MyButtonX + 11, MyButtonY + 14, D2D1::ColorF::White, 2.5);
	DrawFillRectangle(MyButtonX + 50, MyButtonY, W, H, Color);
	DrawLine(MyButtonX + 53, MyButtonY + 1, MyButtonX + 62, MyButtonY + 8, D2D1::ColorF::White, 3);
	DrawLine(MyButtonX + 62, MyButtonY + 8, MyButtonX + 53, MyButtonY + 14, D2D1::ColorF::White, 3);

	if (MyButtonSelect == 0)
	{
		//DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, gMenuBuf[Text头].ItemName, COLORBLACK, 14.0);
		//DrawRenderText(MyButtonX + 24, Y + (H / 5), gMenuBuf[Text头].ItemName, COLORWHITE, 14.0);
	}
	else if(MyButtonSelect == 1)
	{
		//DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, gMenuBuf[Text胸].ItemName, COLORBLACK, 14.0);
		//DrawRenderText(MyButtonX + 24, Y + (H / 5), gMenuBuf[Text胸].ItemName, COLORWHITE, 14.0);
	}
	else 
	{
		//DrawRenderText(MyButtonX + 22, Y + (H / 5) + 2, gMenuBuf[Text颈].ItemName, COLORBLACK, 14.0);
		//DrawRenderText(MyButtonX + 24, Y + (H / 5), gMenuBuf[Text颈].ItemName, COLORWHITE, 14.0);
	}
}
void MyAimStyleBtn::MyButtonClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyButtonSelect --;
		if (MyButtonSelect < 0) MyButtonSelect = 2;

	}
	if (ClickMouse &&
		((MyButtonX + 50 < MousePoint.x) && ((MyButtonX + 50 + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyButtonSelect ++;
		if (MyButtonSelect >2) MyButtonSelect = 0;
	}
}
void MyChangeButton::MyButtonClick(POINT MousePoint)
{
	bool ClickMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY < MousePoint.y) && ((MyButtonH + MyButtonY) > MousePoint.y))
		)
	{
		MyHeightInt += 1;

	}
	if (ClickMouse &&
		((MyButtonX < MousePoint.x) && ((MyButtonX + MyButtonW) > MousePoint.x)) &&
		((MyButtonY + 50 < MousePoint.y) && ((MyButtonH + MyButtonY + 50) > MousePoint.y))
		)
	{
		MyHeightInt -= 1;

	}
}
D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
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
D3DMATRIX ToMatrixWithScale(FQuat& rot, Vector3 &scale, Vector3 &translation)
{
	D3DMATRIX m;
	m._41 = translation.X;
	m._42 = translation.Y;
	m._43 = translation.Z;

	float x2 = rot.x + rot.x;
	float y2 = rot.y + rot.y;
	float z2 = rot.z + rot.z;

	float xx2 = rot.x * x2;
	float yy2 = rot.y * y2;
	float zz2 = rot.z * z2;
	m._11 = (1.0f - (yy2 + zz2)) * scale.X;
	m._22 = (1.0f - (xx2 + zz2)) * scale.Y;
	m._33 = (1.0f - (xx2 + yy2)) * scale.Z;

	float yz2 = rot.y * z2;
	float wx2 = rot.w * x2;
	m._32 = (yz2 - wx2) * scale.Z;
	m._23 = (yz2 + wx2) * scale.Y;

	float xy2 = rot.x * y2;
	float wz2 = rot.w * z2;
	m._21 = (xy2 - wz2) * scale.Y;
	m._12 = (xy2 + wz2) * scale.X;

	float xz2 = rot.x * z2;
	float wy2 = rot.w * y2;
	m._31 = (xz2 + wy2) * scale.Z;
	m._13 = (xz2 - wy2) * scale.X;

	m._14 = 0.0f;
	m._24 = 0.0f;
	m._34 = 0.0f;
	m._44 = 1.0f;

	return m;
}


Vector3 _GetBoneWithRotation(FTransform &bone, FTransform &ComponentToWorld)
{


	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(ToMatrixWithScale(bone.rot, bone.scale, bone.translation), ToMatrixWithScale(ComponentToWorld.rot, ComponentToWorld.scale, ComponentToWorld.translation));
	Vector3 vec;
	vec.X = Matrix._41;
	vec.Y = Matrix._42;
	vec.Z = Matrix._43;
	return vec;
}





//list<int> upper_part = { Bones::neck_01, Bones::Head, Bones::forehead };
list<int> right_arm = { Bones::neck_01, Bones::upperarm_r, Bones::lowerarm_r, Bones::hand_r };
list<int> left_arm = { Bones::neck_01, Bones::upperarm_l, Bones::lowerarm_l, Bones::hand_l };
list<int> spine = { Bones::neck_01, Bones::spine_01, Bones::spine_02, Bones::pelvis };

list<int> lower_right = { Bones::pelvis, Bones::thigh_r, Bones::calf_r, Bones::foot_r };
list<int> lower_left = { Bones::pelvis, Bones::thigh_l, Bones::calf_l, Bones::foot_l };
extern GameOffsets		_GameOffsets;
//std::vector<list<int>> skeleton = { upper_part, right_arm, left_arm, spine, lower_right, lower_left };
std::vector<list<int>> skeleton = {right_arm, left_arm, spine, lower_right, lower_left };

