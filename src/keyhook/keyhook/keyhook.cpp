// keyhook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "keyhook.h"
#include <vector>
#include <Windows.h>
#pragma comment (lib, "User32.lib")
/////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_PROCESS_COUNT	256	 // Maximum number of unique processes can use the hook simultaneously
/*
struct KEYHOOKTYPE
{
	HWND  hCallWnd;// Handle to the window to which the notification messages will be sent, must not be null.
	UINT  nMessage; // The notification message that will be sent when appropriate key events are captured, must not be zero.
	int   nVKCode;	//keycode
	BYTE  nStatus;   //down, up, repeat 
	BYTE  nCombKey;	//alt, carl, shift
};
*/
/////////////////////////////////////////////////////////////////////////////////////////////////
//	Data to be Shared Among Processes
/////////////////////////////////////////////////////////////////////////////////////////////////
#pragma data_seg(".adshared")

BYTE			g_iCombKeys = 0;  // Combo key flags
//KEYHOOKTYPE	g_aProcesses[MAX_PROCESS_COUNT];
INT			g_nProcessCount = 0; // process count

HWND  hCallWnd[MAX_PROCESS_COUNT] = {NULL}; // Handle to the window to which the notification messages will be sent, must not be null.
UINT  nMessage[MAX_PROCESS_COUNT] = {0}; // The notification message that will be sent when appropriate key events are captured, must not be zero.
int   nVKCode[MAX_PROCESS_COUNT] = {-1};	//keycode
BYTE  nStatus[MAX_PROCESS_COUNT] = {0};   //down, up, repeat 
BYTE  nCombKey[MAX_PROCESS_COUNT] = {0};	//alt, carl, shift

#pragma data_seg()
#pragma comment(linker,"/SECTION:.adshared,RWS")
/////////////////////////////////////////////////////////////////////////////////////////////////
//	 Non-Shared Data
/////////////////////////////////////////////////////////////////////////////////////////////////
HHOOK		g_hHook = NULL;
//HMODULE		g_hModule = NULL; 
HINSTANCE	g_hinstance = NULL; 
int			keyDown[256] = {0};
/////////////////////////////////////////////////////////////////////////////////////////////////
//	Key Event Types -- Key-Down, Key-Up, Key-Repeat.
////////////////////////////////////////////////////////////////////////////////////////////////
#define KEY_UNKNOW			0x00 // by sawyer
#define KEY_DOWN			0x01 // Key-down event
#define KEY_UP				0x02 // Key-up event
#define KEY_REPEAT			0x04 // Key-repeat event, the key is 

//////////////////////////////////////////////////////////////////////////////////
//	Combination Key Flags -- States of the Alt, Ctrl, and Shift Keys.
//////////////////////////////////////////////////////////////////////////////////
#define ALT_DOWN			0x01 // The "Alt" key is(or must be) pressed
#define ALT_UP				0x02 // The "Alt" key must not be pressed
#define CTRL_DOWN			0x04 // The "Ctrl" key is(or must be) pressed
#define CTRL_UP				0x08 // The "Ctrl" key must not be pressed
#define SHIFT_DOWN			0x10 // The "Shift" key is(or must be) pressed
#define SHIFT_UP			0x20 // The "Shift" key must not be pressed
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _MANAGED
#pragma managed(push, off)
#endif
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// this will be passed into ::SetWindowsHookEx
		//g_hModule = hModule; 
		g_hinstance =  (HINSTANCE) hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		// uninstall the hook, in case the application forgot to do so
		UninstallKeyHook();
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


extern "C" bool WINAPI InitKeyboardHook(void)
{

	if(g_hHook == NULL){
		//g_hHook = SetWindowsHookEx(WH_KEYBOARD, MainProc, g_hModule, 0);
		g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, MainProc, g_hinstance, 0);

		for (int i=0; i<256; i++)
		{
			keyDown[i] = 0;
		}
		//
		OutputDebugString(L"keyhook.dll::install dll!\n");
	}

	if(g_hHook == NULL){
		OutputDebugString(L"keyhook.dll::dll has fault!\n");
		return false;
	}

	OutputDebugString(L"keyhook.dll::dll has success!\n");
	return true;
}

extern "C" void WINAPI UninstallKeyHook(void)
{
	

	if(g_hHook!=NULL)
	{
		UnhookWindowsHookEx(g_hHook);
		g_hHook=NULL;
	}
}

extern "C" bool WINAPI SetHotKey(HWND CallWnd, UINT Message, INT VKCode, BYTE Status, BYTE CombKey)
{
	if (g_nProcessCount>=MAX_PROCESS_COUNT){
		return false;
	}


	hCallWnd[g_nProcessCount] = CallWnd; 
	nMessage[g_nProcessCount] = Message;
	nVKCode[g_nProcessCount]  = VKCode;
	nCombKey[g_nProcessCount] = CombKey;
	nStatus[g_nProcessCount]  = Status;

	g_nProcessCount++;

	/*
	WCHAR wszBuf[512];
	memset(wszBuf, 0 , sizeof(wszBuf));
	wsprintf(wszBuf, L"VKCode=%d CombKey=%d\n", VKCode, CombKey);
	OutputDebugString(wszBuf);
	*/
	return true;
}

extern "C" void WINAPI ClearHotKey(void)
{
	g_nProcessCount = 0; 
	//OutputDebugString(L"ClearHotKey\n");
}

LRESULT CALLBACK MainProc(int nCode, WPARAM wParam, LPARAM lParam)
{



	if(nCode < 0)
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);

	if(nCode == HC_ACTION)
	{
		
		
		KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT*)lParam;		
	
		BYTE KEYEVENT = GetKeyEventType(wParam, key->vkCode);
		CombKeyInfo(KEYEVENT);

		
		//wchar_t wszBuf[256];
		//swprintf_s(wszBuf, L"combo=%d\n", g_iCombKeys);
		//OutputDebugStringW(wszBuf);



		for(INT i=0; i<g_nProcessCount; i++)
		{	
				

			if (key->vkCode == nVKCode[i] && KEYEVENT == nStatus[i] && g_iCombKeys == nCombKey[i])	
			{	
				//swprintf_s(wszBuf, L"count=%d, VKCode=%d, CombKey=%d KEYEVENT=%d", i, key->vkCode, g_iCombKeys, KEYEVENT);
				//OutputDebugStringW(wszBuf);
				//OutputDebugString(L"\n[key match]\n");

				if ((int)g_iCombKeys == (int)nCombKey[i])
				{
					::PostMessage(hCallWnd[i], nMessage[i], (WPARAM)NULL, (LPARAM)NULL); 	
					break;
				}

			}
		}
			
	}
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}


BYTE GetKeyEventType(LPARAM wParam, DWORD vkCode)
{
	keyDown[vkCode] = GetAsyncKeyState(vkCode)?1:0;
	switch (wParam)
	{
	case WM_KEYDOWN:		
		
		if (keyDown[vkCode] !=0 ){
			//OutputDebugString(L"KEY_REPEAT\n");
			return KEY_REPEAT;
		}

		//OutputDebugString(L"KEY_DOWN\n");
		return KEY_DOWN;
		break;
	case WM_KEYUP:
		//OutputDebugString(L"KEY_UP\n");
		return KEY_UP;
		break;

	}

	return KEY_UNKNOW;
}


void CombKeyInfo(BYTE iKeyEvent)
{
     // update combo keys on both key-down and key-up
     if (iKeyEvent == KEY_DOWN || iKeyEvent == KEY_UP)
	 {  

		if (GetKeyState(VK_SHIFT)<0) {	
			 g_iCombKeys |= SHIFT_DOWN;
		}else{
			 g_iCombKeys &= ~SHIFT_DOWN;
		}

		if (GetKeyState(VK_CONTROL)<0) {	
			g_iCombKeys |= CTRL_DOWN;
		}else{
			g_iCombKeys &= ~CTRL_DOWN;
		}

		if (GetKeyState(VK_MENU)<0) {	
			g_iCombKeys |= ALT_DOWN;
		}else{
			g_iCombKeys &= ~ALT_DOWN;
		}
     }
}
