// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the KEYHOOK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// KEYHOOK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef KEYHOOK_EXPORTS
#define KEYHOOK_API __declspec(dllexport)
#else
#define KEYHOOK_API __declspec(dllimport)
#endif

#ifndef KEYBOARDHOOKER_H
#define KEYBOARDHOOKER_H

extern "C" bool WINAPI InitKeyboardHook(void);
extern "C" void WINAPI UninstallKeyHook(void);
extern "C" bool WINAPI SetHotKey(HWND CallWnd, UINT Message, INT VKCode, BYTE Status, BYTE CombKey=0);
extern "C" void WINAPI ClearHotKey(void);



LRESULT CALLBACK MainProc(int nCode, WPARAM wParam, LPARAM lParam);
BYTE GetKeyEventType(LPARAM wParam, DWORD vkCode);
void CombKeyInfo(BYTE iKeyEvent);

/*
UINT nMessage;    // The notification message that will be sent when appropriate key events are captured, must not be zero.
HWND hCallWnd;    // Handle to the window to which the notification messages will be sent, must not be null.

*/
#endif
