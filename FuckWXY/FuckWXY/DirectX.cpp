#include "global.h"
#include  <D3dx9math.h>
#include "d3d11.h"
#include "render.h"
#include "D3D11Renderer.h"
#include "Draw.h"
#include "Menu.h"
#define M_PI 3.14159265358979323846
float GameScreenX, GameScreenY;
extern POINT MenuPoint;
#pragma comment(lib, "d3d11.lib")
typedef void(__stdcall *D3D11ClearRenderTargetViewHook) (ID3D11DeviceContext* pContext, ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4]);
D3D11ClearRenderTargetViewHook phookD3D11ClearRenderTargetView = NULL; LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
D3D11PresentHook phookD3D11Present = NULL;


DWORD_PTR* pSwapChainVtable = NULL;
DWORD_PTR* pDeviceContextVTable = NULL;
DWORD_PTR* pDeviceVTable = NULL;
DWORD_PTR* pContextVTable = NULL;
ID3D11Device *g_pDevice = NULL;
ID3D11DeviceContext *g_pContext = NULL;
ID3D11RenderTargetView* RenderTargetView = NULL;
IDXGISwapChain* g_pSwapChain;
ID3D11Texture2D *pBackBuffer;
ID3D11Texture2D* RenderTargetTexture;
bool firstTime = true;
D3D11Renderer *pMyD3D11Reader;
Render * Dxrender;

D3DXMATRIX Matrix(Rotator &rot, Vector3 origin = Vector3{ 0, 0, 0 })
{


	float radPitch = (float)(rot.Pitch * M_PI / 180.f);
	float radYaw = (float)(rot.Yaw * M_PI / 180.f);
	float radRoll = (float)(rot.Roll * M_PI / 180.f);



	float SP = (float)sin(radPitch);
	float CP = (float)cos(radPitch);
	float SY = (float)sin(radYaw);
	float CY = (float)cos(radYaw);
	float SR = (float)sin(radRoll);
	float CR = (float)cos(radRoll);

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

	matrix.m[3][0] = origin.X;
	matrix.m[3][1] = origin.Y;
	matrix.m[3][2] = origin.Z;
	matrix.m[3][3] = 1.f;
	//matrix.m[3][0] = 0;
	//matrix.m[3][1] = 0;
	//matrix.m[3][2] = 0;
	//matrix.m[3][3] = 1.f;
	return matrix;
}


void worldToScreen3(float fov, Vector3 &WorldLocation, Rotator &Rotation, Vector3 &playerposition, float*sx, float*sy)
{


	D3DXVECTOR3 origin;
	D3DMATRIX tempMatrix = Matrix(Rotation); // Matrix

	D3DXVECTOR3 vAxisX, vAxisY, vAxisZ;

	vAxisX = D3DXVECTOR3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = D3DXVECTOR3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = D3DXVECTOR3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
	D3DXVECTOR3 WorldLoc(WorldLocation.X, WorldLocation.Y, WorldLocation.Z);
	D3DXVECTOR3 playerpos(playerposition.X, playerposition.Y, playerposition.Z);
	D3DXVECTOR3 vDelta = WorldLoc - playerpos;
	D3DXVECTOR3 vTransformed;
	//	= D3DXVECTOR3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	vTransformed.x = D3DXVec3Dot(&vDelta, &vAxisY);
	vTransformed.y = D3DXVec3Dot(&vDelta, &vAxisZ);
	vTransformed.z = D3DXVec3Dot(&vDelta, &vAxisX);

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	//float ScreenCenterX = MyRect.right/2; //1280 / 2;
	//float ScreenCenterY = MyRect.bottom/2;//768 / 2;
	//float ScreenCenterX = 1280 / 2;
	//float ScreenCenterY = 768 / 2;
	float ScreenCenterX = GameScreenX / 2;
	float ScreenCenterY = GameScreenY / 2;
	//printf("%d  %d\n", MyRect.right, MyRect.bottom);
	*sx = ScreenCenterX + vTransformed.x * (ScreenCenterX / (float)tan(fov * (float)M_PI / 360)) / vTransformed.z;
	*sy = ScreenCenterY - vTransformed.y * (ScreenCenterX / (float)tan(fov * (float)M_PI / 360)) / vTransformed.z;

}


void SuspendMainThread()
{
	SuspendThread(GetCurrentProcess());
}

void ResumeMainThread()
{
	ResumeThread(GetCurrentProcess());
}


void SetGameScreen()
{
	if (pMyD3D11Reader)
	{
		GameScreenX = pMyD3D11Reader->GetWidth();
		GameScreenY = pMyD3D11Reader->GetHeight();
		Log.printf("GameScreenX %02f GameScreenY %02f\n", GameScreenX, GameScreenY);
	}
}

int Distance(Vector3 *me, Vector3 *enemy)
{
	float A = (me->X - enemy->X);
	float B = (me->Y - enemy->Y);
	float C = (me->Z - enemy->Z);
	return (int)(sqrtf(A*A + B*B)) / 100;
}

void DxDraw()
{
	if (firstTime&&g_pSwapChain)
	{

		if (SUCCEEDED(g_pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&g_pDevice)))
		{
			g_pSwapChain->GetDevice(__uuidof(g_pDevice), (void**)&g_pDevice);
			g_pDevice->GetImmediateContext(&g_pContext);

		}


		if (SUCCEEDED(g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RenderTargetTexture)))
		{
			g_pDevice->CreateRenderTargetView(RenderTargetTexture, NULL, &RenderTargetView);
			RenderTargetTexture->Release();
		}

		Dxrender = new Render(g_pDevice);

		pMyD3D11Reader = new D3D11Renderer(g_pSwapChain);
		pMyD3D11Reader->Initialize();


		//Log.printf("GameScreenX %2f %2f Chain %I64x\n", GameScreenX, GameScreenY, g_pSwapChain);
		firstTime = false;

	
	}
	if (!firstTime&&g_pSwapChain&&pMyD3D11Reader)
	{

		g_pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
		//Log.printf("%d\n", GetCurrentThreadId());
		if (GameScreenX <= 1.0)
		{
			SetGameScreen();
		}
		

		Dxrender->Begin();
		MenuCreate(MenuPoint);
		RenderGameHack();
		//DrawRenderText(0, 0, (L"ÒÑ¹ýÆÚ"), COLORWHITE, 100.0);

		Dxrender->End();


	}
}
HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{



	g_pSwapChain = pSwapChain;
	DxDraw();
	//if (pMyD3D11Reader && !firstTime)
	//{
	//	DrawRectangle(800, 600, 20, 20, COLORRED);
	//	DrawRectangle(1754, 922, 20, 20, COLORRED);
	//}
// 	DWORD oldp;
 	HRESULT hr = phookD3D11Present(pSwapChain, SyncInterval, Flags);
// 	VirtualProtect((LPVOID)&(pSwapChainVtable[8]), 8, PAGE_READWRITE, &oldp);
// 	pSwapChainVtable[8] = (DWORD_PTR)phookD3D11Present;
// 	VirtualProtect((LPVOID)&(pSwapChainVtable[8]), 8, oldp, &oldp);
	return hr;
}


DWORD InitializeHook()
{

	ID3D11Device *pDevice = NULL;
	ID3D11DeviceContext *pContext = NULL;
	IDXGISwapChain* pSwapChain;

	HMODULE hDXGIDLL = 0;
	hDXGIDLL = GetModuleHandle("dxgi.dll");
	Sleep(100);

	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("Dx", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

	D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtainedLevel;
	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

	// LibOVR 0.4.3 requires that the width and height for the backbuffer is set even if
	// you use windowed mode, despite being optional according to the D3D11 documentation.
	scd.BufferDesc.Width = 1;
	scd.BufferDesc.Height = 1;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;

	UINT createFlags = 0;

	IDXGISwapChain* d3dSwapChain = 0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		requestedLevels,
		sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		&obtainedLevel,
		&pContext)))
	{
		//MessageBox(hWnd, "Failed to create directX device and swapchain!", "Error", MB_ICONERROR);
		return NULL;
}


	pSwapChainVtable = (DWORD_PTR*)pSwapChain;
	//Log.printf("swapchian %p\n", pSwapChainVtable);
	pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];
	// 
	pContextVTable = (DWORD_PTR*)pContext;
	pContextVTable = (DWORD_PTR*)pContextVTable[0];

	pDeviceVTable = (DWORD_PTR*)pDevice;
	pDeviceVTable = (DWORD_PTR*)pDeviceVTable[0];

	SuspendMainThread();
	DWORD oldp;
	phookD3D11Present = (D3D11PresentHook)pSwapChainVtable[8];
	VirtualProtect((LPVOID)&(pSwapChainVtable[8]), 8, PAGE_READWRITE, &oldp);
	pSwapChainVtable[8] = (DWORD_PTR)hookD3D11Present;
	VirtualProtect((LPVOID)&(pSwapChainVtable[8]), 8, oldp, &oldp);
	Log.printf(VMPDSA("InitializeHook \r\n"));


	ResumeMainThread();


	//APIHOOK();

	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();
	DestroyWindow(hWnd);

return NULL;
}
