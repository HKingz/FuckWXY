#include "D3D11Renderer.h"
#include "gdiplus.h"
#include <string>
#include <sstream>
#include <iostream>
#include "Draw.h"
#include <queue>
#include "Log.h"
using namespace std;
#pragma comment(lib, "d3d11.lib")
#define D3DCOLOR_ABGR(a,b,g,r) \
	((DWORD)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))
D3D11Renderer::D3D11Renderer(IDXGISwapChain *SwapChain)
{
	timestart = 0;
	this->device = NULL;
	this->deviceContext = NULL;
	this->mVS = NULL;
	this->mPS = NULL;
	this->transparency = NULL;
	this->mInputLayout = NULL;
	this->mVertexBuffer = NULL;
	this->m_pFW1Factory = NULL;
	this->swapChain = SwapChain;
	this->m_pFontWrapper = NULL;
	this->m_pFontVerdana = NULL;
	this->stateSaver = new D3D11StateSaver();
	
}

D3D11Renderer::~D3D11Renderer()
{
	SAFE_DELETE(this->stateSaver);
	SAFE_RELEASE(this->mVS);
	SAFE_RELEASE(this->mPS);
	SAFE_RELEASE(this->transparency);
	SAFE_RELEASE(this->mInputLayout);
	SAFE_RELEASE(this->mVertexBuffer);
	SAFE_RELEASE(this->swapChain);
	SAFE_RELEASE(this->device);
	SAFE_RELEASE(this->deviceContext);
}
extern CLog Log;
bool D3D11Renderer::Initialize()
{
	HRESULT hr;

	if (!this->swapChain)
		return false;

	this->swapChain->GetDevice(__uuidof(this->device), (void**)&this->device);
	if (!this->device)
		return false;

	this->device->GetImmediateContext(&this->deviceContext);
	if (!this->deviceContext)
		return false;

	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1Factory)))
		return false;

	if (FAILED(m_pFW1Factory->CreateFontWrapper(this->device, L"Microsoft Yahei", &m_pFontWrapper)))
	{

		return false;
		
	}
	if (FAILED(m_pFW1Factory->CreateFontWrapper(this->device, L"Verdana", &m_pFontVerdana)))
	{

		return false;

	}
	

	this->m_pFW1Factory->Release();

	typedef HRESULT(__stdcall* D3DCompile_t)(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO *pDefines, ID3DInclude *pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob **ppCode, ID3DBlob *ppErrorMsgs);
	D3DCompile_t myD3DCompile = (D3DCompile_t)GetProcAddress(GetD3DCompiler(), "D3DCompile");
	if (!myD3DCompile)
		return false;

	ID3D10Blob *VS, *PS;
	hr = myD3DCompile(D3D11FillShader, sizeof(D3D11FillShader), NULL, NULL, NULL, "VS", "vs_4_0", 0, 0, &VS, NULL);
	if (FAILED(hr))
		return false;

	hr = this->device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &this->mVS);
	if (FAILED(hr))
	{
		SAFE_RELEASE(VS);
		return false;
	}
				
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->device->CreateInputLayout(layout, ARRAYSIZE(layout), VS->GetBufferPointer(), VS->GetBufferSize(), &this->mInputLayout);
	SAFE_RELEASE(VS);
	if (FAILED(hr))
		return false;
		
	myD3DCompile(D3D11FillShader, sizeof(D3D11FillShader), NULL, NULL, NULL, "PS", "ps_4_0", 0, 0, &PS, NULL);
	if (FAILED(hr))
		return false;

	hr = this->device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &this->mPS);
	if (FAILED(hr))
	{
		SAFE_RELEASE(PS);
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = 4 * sizeof(COLOR_VERTEX);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	hr = this->device->CreateBuffer(&bufferDesc, NULL, &this->mVertexBuffer);
	if(FAILED(hr))
		return false;

	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(blendStateDescription));

	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	hr = this->device->CreateBlendState(&blendStateDescription, &this->transparency);
	if (FAILED(hr))
		return false;
	return true;
}

void D3D11Renderer::FillRect(float x, float y, float w, float h, DWORD color)
{
	if (this->deviceContext == NULL)
		return;

	//int a = color.A & 0xff;
	//int r = color.R & 0xff;
	//int g = color.G & 0xff;
	//int b = color.B & 0xff;

	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	this->deviceContext->RSGetViewports(&viewportNumber, &vp);

	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;

	float xx0 = 2.0f * (x0 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y0 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;

	COLOR_VERTEX* v = NULL;
	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(this->mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	v[0].Position.x = (float)x0;
	v[0].Position.y = (float)y0;
	v[0].Position.z = 0;
	v[0].Color = color;

	v[1].Position.x = (float)x1;
	v[1].Position.y = (float)y1;
	v[1].Position.z = 0;
	v[1].Color = color;

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;
	v[0].Position.z = 0;
	v[0].Color = color;

	v[1].Position.x = xx1;
	v[1].Position.y = yy0;
	v[1].Position.z = 0;
	v[1].Color = color;

	v[2].Position.x = xx0;
	v[2].Position.y = yy1;
	v[2].Position.z = 0;
	v[2].Color = color;

	v[3].Position.x = xx1;
	v[3].Position.y = yy1;
	v[3].Position.z = 0;
	v[3].Color = color;


	this->deviceContext->Unmap(this->mVertexBuffer, NULL);

	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;

	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);
	this->deviceContext->Draw(4, 0);
}

void D3D11Renderer::DrawLine(int x, int y, int x1, int y1, DWORD color/*ARGB*/)
{
	int a = (color >> 24) & 0xff;
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = (color) & 0xff;
	this->DrawLineInternal(x, y, x1, y1, r, g, b, a);
}


void D3D11Renderer::DrawHealthBar(float x, float y, float w, float health, float max)
{
	this->DrawHealthBar(x, y, w, 2, health, max);
}

void D3D11Renderer::DrawHealthBar(float x, float y, float w, float h, float health, float max)
{

	//if (!max)
	//	return;

	//if (w < 5)
	//	w= 5;

	//if (health < 0)
	//	health = 0;

	//float ratio = health / max;

	//Color col = Color(255, (uchar)(255 - 255 * ratio), (uchar)(255 * ratio), 0);

	//float step = (w / max);
	//float draw = (step * health);

	//FillRect(x, y, w, h, Color(255, 0, 0, 0));
	//FillRect(x, y, draw, h, col);
}

static std::string asciiEncode(const std::wstring & w)
{
	std::ostringstream  s;
	char *              c;
	int					r;

	c = new char[MB_CUR_MAX + 1];
	for (size_t i = 0; i < w.length(); i++) {
		r = wctomb(c, w[i]);
		c[r] = '\0';
		if (r <= 1 && c[0] > 0) {
			s << c;
		}
	}
	return s.str();
}

static std::wstring asciiDecode(const std::string & s)
{
	std::wostringstream    w;
	wchar_t                c;

	for (size_t i = 0; i < s.length(); i++) {
		mbtowc(&c, &s[i], 1);
		w << c;
	}
	return w.str();
}
void D3D11Renderer::AddTimedText(WCHAR* Text, DWORD time,DWORD color)
{
	msgendtime = GetTickCount() + time;
	int a = (color >> 24) & 0xff;
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = (color) & 0xff;

	msgqueuecolor = D3DCOLOR_ABGR(a, b, g, r);

	msgqueue = Text;
}
void D3D11Renderer::RenderTimedText(int x, int y, float m_fDefaultFontSize)
{

	if (this->deviceContext == NULL)
		return;

	DWORD nowtime = GetTickCount();
	if (nowtime < msgendtime)
	{

		m_pFontVerdana->DrawString(deviceContext, msgqueue.c_str(), m_fDefaultFontSize, x, y, msgqueuecolor, FW1_RESTORESTATE);
		
	}

	
}
void D3D11Renderer::RenderText2(int x, int y, DWORD color, float m_fDefaultFontSize, WCHAR *szText)
{

	if (this->deviceContext == NULL)
		return;


	int a = (color >> 24) & 0xff;
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = (color) & 0xff;

	DWORD dwABGR = D3DCOLOR_ABGR(a, b, g, r);
	//DWORD dwBlack = D3DCOLOR_ABGR(255, 0, 0, 0);

	


	//pFontWrapper->DrawString(g_pContext, L"LeftSpace - First D3D11 Hook x64 - 13.07.2017", 14, 16.0f, 16.0f, 0xffff1414, FW1_RESTORESTATE);
	// 	m_pFontWrapper->DrawString(deviceContext, WStr, m_fDefaultFontSize, x + 1, y, dwABGR, FW1_RESTORESTATE);
	//  	m_pFontWrapper->DrawString(deviceContext, WStr, m_fDefaultFontSize, x - 1, y, dwABGR, FW1_RESTORESTATE);
	// 	m_pFontWrapper->DrawString(deviceContext, WStr, m_fDefaultFontSize, x, y + 1, dwABGR, FW1_RESTORESTATE);
	//  	m_pFontWrapper->DrawString(deviceContext, WStr, m_fDefaultFontSize, x, y - 1, dwABGR, FW1_RESTORESTATE);
	m_pFontVerdana->DrawString(deviceContext, szText, m_fDefaultFontSize, x, y, dwABGR, FW1_RESTORESTATE);
}

void D3D11Renderer::RenderText(int x, int y, DWORD color, float m_fDefaultFontSize,WCHAR *szText)
{

	if (this->deviceContext == NULL)
		return;

	
	int a = (color >> 24) & 0xff;
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = (color) & 0xff;

	DWORD dwABGR = D3DCOLOR_ABGR(a, b, g, r);
	//DWORD dwBlack = D3DCOLOR_ABGR(255, 0, 0, 0);




	//pFontWrapper->DrawString(g_pContext, L"LeftSpace - First D3D11 Hook x64 - 13.07.2017", 14, 16.0f, 16.0f, 0xffff1414, FW1_RESTORESTATE);
// 	m_pFontWrapper->DrawString(deviceContext, WStr, m_fDefaultFontSize, x + 1, y, dwABGR, FW1_RESTORESTATE);
//  	m_pFontWrapper->DrawString(deviceContext, WStr, m_fDefaultFontSize, x - 1, y, dwABGR, FW1_RESTORESTATE);
// 	m_pFontWrapper->DrawString(deviceContext, WStr, m_fDefaultFontSize, x, y + 1, dwABGR, FW1_RESTORESTATE);
//  	m_pFontWrapper->DrawString(deviceContext, WStr, m_fDefaultFontSize, x, y - 1, dwABGR, FW1_RESTORESTATE);
 	m_pFontWrapper->DrawString(deviceContext, szText, m_fDefaultFontSize, x, y, dwABGR, FW1_RESTORESTATE);
}

void D3D11Renderer::FillARGB(int x, int y, int w, int h, DWORD color/*ARGB*/)
{
	int a = (color >> 24) & 0xff;
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = (color) & 0xff;
	this->DrawRectInternal(x, y, x + w, y + h, r, g, b, a);
}

void D3D11Renderer::DrawTimedLine(int x, int y, int x1, int y1, DWORD color/*ARGB*/,DWORD time)
{

	if (timestart!=0) timestart = GetTickCount();
	if (GetTickCount() - timestart < time)
	{
		DrawLine(x, y, x1, y1, color);
	}
	else
	{
		timestart = 0xFFFFFFFF;
	}
}
void D3D11Renderer::DrawLineInternal(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	if (this->deviceContext == NULL)
		return;

	if (device == NULL || deviceContext == NULL)
		return;

	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	this->deviceContext->RSGetViewports(&viewportNumber, &vp);

	float xx0 = 2.0f * (x0 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y0 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;

	COLOR_VERTEX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;
	v[0].Position.z = 0;
	v[0].Color.r = ((FLOAT)r / 255.0f);
	v[0].Color.g = ((FLOAT)g / 255.0f);
	v[0].Color.b = ((FLOAT)b / 255.0f);
	v[0].Color.a = ((FLOAT)a / 255.0f);

	v[1].Position.x = xx1;
	v[1].Position.y = yy1;
	v[1].Position.z = 0;
	v[1].Color.r = ((FLOAT)r / 255.0f);
	v[1].Color.g = ((FLOAT)g / 255.0f);
	v[1].Color.b = ((FLOAT)b / 255.0f);
	v[1].Color.a = ((FLOAT)a / 255.0f);

	this->deviceContext->Unmap(mVertexBuffer, NULL);



	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;


	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);
	this->deviceContext->Draw(2, 0);
}

void D3D11Renderer::DrawRectInternal(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	if (this->deviceContext == NULL)
		return;

	if (device == NULL || deviceContext == NULL)
		return;

	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	this->deviceContext->RSGetViewports(&viewportNumber, &vp);

	float xx0 = 2.0f * (x0 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y0 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;

	COLOR_VERTEX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;
	v[0].Position.z = 0;
	v[0].Color.r = ((FLOAT)r / 255.0f);
	v[0].Color.g = ((FLOAT)g / 255.0f);
	v[0].Color.b = ((FLOAT)b / 255.0f);
	v[0].Color.a = ((FLOAT)a / 255.0f);

	v[1].Position.x = xx1;
	v[1].Position.y = yy0;
	v[1].Position.z = 0;
	v[1].Color.r = ((FLOAT)r / 255.0f);
	v[1].Color.g = ((FLOAT)g / 255.0f);
	v[1].Color.b = ((FLOAT)b / 255.0f);
	v[1].Color.a = ((FLOAT)a / 255.0f);

	v[2].Position.x = xx0;
	v[2].Position.y = yy1;
	v[2].Position.z = 0;
	v[2].Color.r = ((FLOAT)r / 255.0f);
	v[2].Color.g = ((FLOAT)g / 255.0f);
	v[2].Color.b = ((FLOAT)b / 255.0f);
	v[2].Color.a = ((FLOAT)a / 255.0f);

	v[3].Position.x = xx1;
	v[3].Position.y = yy1;
	v[3].Position.z = 0;
	v[3].Color.r = ((FLOAT)r / 255.0f);
	v[3].Color.g = ((FLOAT)g / 255.0f);
	v[3].Color.b = ((FLOAT)b / 255.0f);
	v[3].Color.a = ((FLOAT)a / 255.0f);

	this->deviceContext->Unmap(mVertexBuffer, NULL);




	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;


	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);
	this->deviceContext->Draw(4, 0);


}

void D3D11Renderer::DrawCircleInternal(int x0, int y0, int radius, int r, int g, int b, int a)
{
	if (this->deviceContext == false)
		return;

	if (device == NULL || deviceContext == NULL)
		return;

	const int NUMPOINTS = 30;
	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	this->deviceContext->RSGetViewports(&viewportNumber, &vp);

	COLOR_VERTEX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	float WedgeAngle = (float)((2 * D3DX_PI) / NUMPOINTS);

	for (int i = 0; i <= NUMPOINTS; i++)
	{
		float Theta = (float)(i * WedgeAngle);
		float x = (float)(x0 + radius * cos(Theta));
		float y = (float)(y0 - radius * sin(Theta));

		v[i].Position.x = 2.0f * (x - 0.5f) / vp.Width - 1.0f;
		v[i].Position.y = 1.0f - 2.0f * (y - 0.5f) / vp.Height;
		v[i].Position.z = 0.0f;
		v[i].Color.r = ((FLOAT)r / 255.0f);
		v[i].Color.g = ((FLOAT)g / 255.0f);
		v[i].Color.b = ((FLOAT)b / 255.0f);
		v[i].Color.a = ((FLOAT)a / 255.0f);
	}

	this->deviceContext->Unmap(mVertexBuffer, NULL);




	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;


	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);
	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);
	this->deviceContext->Draw(4, 0);


}

void D3D11Renderer::DrawCircle(int x, int y, int radius, DWORD color/*ARGB*/)
{
	int a = (color >> 24) & 0xff;
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = (color) & 0xff;
	this->DrawCircleInternal(x, y, radius, r, g, b, a);
}

void D3D11Renderer::DrawBox(int x, int y, int w, int h, DWORD BoxColor, DWORD BorderColor)
{
	if (BoxColor)
		FillARGB(x, y, w, h, BoxColor);
	DrawBorder(x, y, w, h, 1, BorderColor);
}


void D3D11Renderer::DrawBorder(int x, int y, int w, int h, int px, DWORD BorderColor)
{
	FillARGB(x, (y + h - px), w, px, BorderColor);
	FillARGB(x, y, px, h, BorderColor);
	FillARGB(x, y, w, px, BorderColor);
	FillARGB((x + w - px), y, px, h, BorderColor);
}

void D3D11Renderer::DrawPoint(int x, int y, DWORD color)
{
	FillARGB(x - 1, y - 1, 2, 2, color);
}

void D3D11Renderer::DrawPixel(int x, int y, DWORD color)
{
	FillARGB(x, y, 1, 1, color);
}

float D3D11Renderer::GetWidth()
{
	D3D11_VIEWPORT vp;
	UINT nvp = 1;
	this->deviceContext->RSGetViewports(&nvp, &vp);
	return vp.Width;
}

float D3D11Renderer::GetHeight()
{
	D3D11_VIEWPORT vp;
	UINT nvp = 1;
	this->deviceContext->RSGetViewports(&nvp, &vp);
	return vp.Height;
}

void D3D11Renderer::BeginScene()
{
	this->restoreState = false;
	if (SUCCEEDED(this->stateSaver->saveCurrentState(this->deviceContext,this->device)))
		this->restoreState = true;

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	this->deviceContext->OMSetBlendState(this->transparency, blendFactor, 0xffffffff);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

}

char* UnicodeToUtf8(const wchar_t* unicode)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	return szUtf8;
}

void D3D11Renderer::EndScene()
{
	//RenderText(100, 100, COLORBLUE, 14, "11111");
	//DrawCircle(200, 200, 100, COLORWHITE);
	//外挂功能写这里吧~
	if (this->restoreState)
		this->stateSaver->restoreSavedState();
}