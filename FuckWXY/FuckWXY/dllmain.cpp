// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "global.h"
#include "global.h"
DWORD64  g_dwJmp;
DWORD64 g_len;
bool g_isHaveBuf = false;
unsigned char code[12];
char g_buf[5000] = { 0 };
char g_strProtobuf[5000] = { "0" };
extern "C" void hookSend();
HANDLE g_ths;
HWND g_MyForm;
bool g_isSend = false;
CHackProcess HackProcess;
CLog Log;
DWORD WINAPI ShowProtobufThread(LPVOID lp)
{
	while (1)
	{
		if (g_isHaveBuf == true)
		{
			printf("len %d\n", g_len);
			g_isHaveBuf = false;
			for (int i = 0; i < g_len; i++)
			{
				printf("%02x", *(BYTE*)((char*)g_buf+i));
			}
			printf("\n");
			
		}
	}

	return 1;
}



extern "C"
{
	bool ShowProtobuf(DWORD64 len,char*buf)
	{
		//MyShow(len, buf);
		//printf("%d\n", len);
		if (len > 0)
		{
			
			g_len = len;
			memcpy(g_buf, buf, len);
			g_isHaveBuf = true;
			if (g_isSend)
			{
				g_isSend = false;
				return true;
			}
			
		}
		
		return false;
	}
	
}

DWORD WINAPI HookThread(LPVOID lp)
{
	DWORD64 hModule = (DWORD64)GetModuleHandleA("wuxiaxclient-win64-shipping.exe");
	DWORD64 dwHook = hModule + 0x3573c1;
	g_dwJmp = hModule + 0x3573cf;
	DWORD oldflag;
	if (VirtualProtect((PVOID)dwHook, 12, PAGE_EXECUTE_READWRITE, &oldflag))
	{
		code[0] = 0x48;
		code[1] = 0xba;
		code[10] = 0x52;
		code[11] = 0xC3;
		DWORD64 dwFunAddr = (DWORD64)hookSend;
		RtlMoveMemory(code + 2, &dwFunAddr, 8);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)dwHook, code, 12, NULL);
		VirtualProtect((PVOID)dwHook, 12, oldflag, &oldflag);
		printf("hook sucess\n");
	}
	return 1;
}

unsigned int str_hex(unsigned char *str, unsigned char *hex)
{
	unsigned char ctmp, ctmp1, half;
	unsigned int num = 0;
	do {
		do {
			half = 0;
			ctmp = *str;
			if (!ctmp) break;
			str++;
		} while ((ctmp == 0x20) || (ctmp == 0x2c) || (ctmp == '\t'));
		if (!ctmp) break;
		if (ctmp >= 'a') ctmp = ctmp - 'a' + 10;
		else if (ctmp >= 'A') ctmp = ctmp - 'A' + 10;
		else ctmp = ctmp - '0';
		ctmp = ctmp << 4;
		half = 1;
		ctmp1 = *str;
		if (!ctmp1) break;
		str++;
		if ((ctmp1 == 0x20) || (ctmp1 == 0x2c) || (ctmp1 == '\t'))
		{
			ctmp = ctmp >> 4;
			ctmp1 = 0;
		}
		else if (ctmp1 >= 'a') ctmp1 = ctmp1 - 'a' + 10;
		else if (ctmp1 >= 'A') ctmp1 = ctmp1 - 'A' + 10;
		else ctmp1 = ctmp1 - '0';
		ctmp += ctmp1;
		*hex = ctmp;
		hex++;
		num++;
	} while (1);
	if (half)
	{
		ctmp = ctmp >> 4;
		*hex = ctmp;
		num++;
	}
	return(num);


}
DWORD WINAPI SetTimeThread(LPVOID lp)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	st.wMinute = st.wMinute + 2;
	SetLocalTime(&st);
	Sleep(5000);
	GetLocalTime(&st);
	st.wMinute = st.wMinute - 2;
	SetLocalTime(&st);
	return 1;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	char strtmp[5000] = { 0 };
	switch (message)
	{
	case WM_INITDIALOG:
		RegisterHotKey(hDlg, 1987, NULL, VK_HOME);//注册快捷键  
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			char strtmp[5000];
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT1), strtmp, 5000);
			g_len = str_hex((unsigned char*)strtmp, (unsigned char*)g_strProtobuf);
			g_isSend = true;
			CreateThread(0, 0, SetTimeThread, 0, 0, 0);
	
			return TRUE;
		case IDCANCEL:
			DestroyWindow(hDlg);
			return TRUE;
		}
	case WM_HOTKEY:
	{
		if (LOWORD(wParam) == 1987)
		{
			if (IsWindowVisible(hDlg))
				ShowWindow(hDlg, SW_HIDE);
			else
			{
				ShowWindow(hDlg, SW_NORMAL);
				SetForegroundWindow(hDlg);
			}
		}
	}
	return TRUE;
	}
	return FALSE;
}

DWORD WINAPI ShowForm(LPVOID lp)
{
	g_MyForm = CreateDialog((HINSTANCE)g_ths, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
	//ShowWindow(g_MyForm, SW_SHOW);
	MSG msg;
	PeekMessage(&msg, g_MyForm, 0U, 0U, PM_NOREMOVE);
	while (msg.message != WM_QUIT)
	{

		if (PeekMessage(&msg, g_MyForm, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Sleep(10);
		}
	}
	
	return 1;
}

typedef void(_cdecl* pfnStartThread)(void*);
VOID NTAPI ThreadAPC(
	__in ULONG_PTR Parameter
)
{

	pfnStartThread func = (pfnStartThread)Parameter;
	func(0);
	ExitThread(0);
}

void CreateHidThread(pfnStartThread func)
{
	HMODULE vcrt = GetModuleHandle("msvcrt.dll");
	DWORD64 vcrtaddr = (DWORD64)GetProcAddress(vcrt, "ftime64_s");
	HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(vcrtaddr + 0x180), 0, CREATE_SUSPENDED, 0);
	if (hThread)
	{
		QueueUserAPC(ThreadAPC, hThread, (ULONG_PTR)func);
		ResumeThread(hThread);
		CloseHandle(hThread);
	}

}


void TestFun()
{
	while (1)
	{
		Sleep(1000);
		RenderGameHack();
	}
}
void InternalInit(void*)
{
	char LogPath[MAX_PATH] = { 0 };
	GetModuleFileNameA((HMODULE)g_ths,LogPath, MAX_PATH);
	PathRemoveFileSpec(LogPath);
	strcat_s(LogPath, VMPDSA("\\Log.txt"));
	Log.SetLogName(LogPath);
	Log.Start();
	Log.printf("ths %p\n", g_ths);
	InitGameBase();
	//InitializeHook();
	TestFun();
	
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (AllocConsole())
		{
			freopen("CONOUT$", "w", stdout);
			printf("Im In!\n");
		}
		g_ths = hModule;
		CreateHidThread(InternalInit);
		//CreateThread(0, 0, HookThread, 0, 0, 0);
		//CreateThread(0, 0, ShowProtobufThread, 0, 0, 0);
		//CreateThread(0, 0, ShowForm, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

