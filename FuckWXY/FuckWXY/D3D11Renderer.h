#pragma once

#pragma warning (push)
#pragma warning (disable: 4005)
#include <d3d9.h>
#include <d3d11.h>
#include <d3dx9.h>
#include <d3dx10.h>
#include <d3dx11.h>
#include <D3DX11tex.h>
#include <DXGI.h>
#pragma warning (pop)
#include <stdio.h>
#include <Windows.h>
#include "FW1FontWrapper\FW1FontWrapper.h"
#include "Helper.h"
#include "D3D11StateSaver.h"
#include "D3D11Shader.h"
#include <queue>
#include <string>
typedef unsigned char uchar;
#define COLORWHITE D3DCOLOR_ARGB(255,255,255,255)
#define COLORRED D3DCOLOR_ARGB(255,255,0,0)
#define COLORPINK D3DCOLOR_ARGB(255,244,183,189)
#define COLORBLACK D3DCOLOR_ARGB(255,0,0,0)
#define COLORYELLOW D3DCOLOR_ARGB(255,255,255,0)
#define COLORBLUE D3DCOLOR_ARGB(255, 0, 184, 245)
#define COLORGREEN D3DCOLOR_ARGB(255, 46, 139, 87)
#define COLORORENGE D3DCOLOR_ARGB(255, 255, 128, 0)
#define COLORDARKRED D3DCOLOR_ARGB(255, 255,128, 255)

class Color
{
public:
	uchar A, R, G, B;

	Color()
	{
		A = R = G = B = 0;
	}

	Color(uchar A, uchar R, uchar G, uchar B)
	{
		this->A = A;
		this->R = R;
		this->G = G;
		this->B = B;
	}
};

class D3D11Renderer
{
private:
	struct COLOR_VERTEX
	{
		D3DXVECTOR3	Position;
		D3DXCOLOR Color;
	};

	IDXGISwapChain *swapChain;
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;
	ID3D11InputLayout *mInputLayout;
	ID3D11Buffer *mVertexBuffer;
	ID3D11VertexShader *mVS;
	ID3D11PixelShader *mPS;
	ID3D11BlendState *transparency;
	IFW1Factory*	m_pFW1Factory;
	D3D11StateSaver *stateSaver;
	IFW1FontWrapper*		m_pFontWrapper;
	IFW1FontWrapper*		m_pFontVerdana;
	std::wstring msgqueue;
	DWORD msgqueuecolor;
	bool restoreState = false;
	DWORD timestart;
	DWORD msgendtime;
public:
	D3D11Renderer(IDXGISwapChain *SwapChain);
	~D3D11Renderer();
	void FillARGB(int x, int y, int w, int h, DWORD color/*ARGB*/);
	bool Initialize();
	void DrawPoint(int x, int y, DWORD color);
	void DrawBox(int x, int y, int w, int h, DWORD BoxColor, DWORD BorderColor);
	void DrawBorder(int x, int y, int w, int h, int px, DWORD BorderColor);
	void DrawPixel(int x, int y, DWORD color);
	void DrawCircleInternal(int x0, int y0, int radius, int r, int g, int b, int a);
	void DrawCircle(int x, int y, int radius, DWORD color/*ARGB*/);
	void RenderText(int x, int y, DWORD color, float m_fDefaultFontSize, WCHAR *szText);
	void FillRect(float x, float y, float w, float h, DWORD color);
	void DrawLine(int x, int y, int x1, int y1, DWORD color/*ARGB*/);
	void DrawLineInternal(int x0, int y0, int x1, int y1, int r, int g, int b, int a);
	void DrawRectInternal(int x0, int y0, int x1, int y1, int r, int g, int b, int a);
	void DrawHealthBar(float x, float y, float w, float health, float max);
	void DrawHealthBar(float x, float y, float w, float h, float health, float max);
	void DrawTimedLine(int x, int y, int x1, int y1, DWORD color/*ARGB*/,DWORD time);
	void RenderTimedText(int x, int y,float m_fDefaultFontSize);
	void AddTimedText(WCHAR* Text, DWORD time, DWORD color);
	float GetWidth();
	float GetHeight();
	void BeginScene();
	void EndScene();
	void RenderText2(int x, int y, DWORD color, float m_fDefaultFontSize, WCHAR *szText);

};