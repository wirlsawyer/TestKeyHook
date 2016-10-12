#if BCB
#include <windows.h>
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////
//	Key Event Types -- Key-Down, Key-Up, Key-Repeat.
////////////////////////////////////////////////////////////////////////////////////////////////
#define KEYHOOK_KEY_DOWN			0x01 // Key-down event
#define KEYHOOK_KEY_UP				0x02 // Key-up event
#define KEYHOOK_KEY_REPEAT			0x04 // Key-repeat event, the key is 
//////////////////////////////////////////////////////////////////////////////////
//	Combination Key Flags -- States of the Alt, Ctrl, and Shift Keys.
//////////////////////////////////////////////////////////////////////////////////
#define KEYHOOK_ALT_DOWN			0x01 // The "Alt" key is(or must be) pressed
#define KEYHOOK_ALT_UP				0x02 // The "Alt" key must not be pressed
#define KEYHOOK_CTRL_DOWN			0x04 // The "Ctrl" key is(or must be) pressed
#define KEYHOOK_CTRL_UP				0x08 // The "Ctrl" key must not be pressed
#define KEYHOOK_SHIFT_DOWN			0x10 // The "Shift" key is(or must be) pressed
#define KEYHOOK_SHIFT_UP			0x20 // The "Shift" key must not be pressed
/////////////////////////////////////////////////////////////////////////////////////////////////
#define VK_0		0x30
#define VK_1		0x31
#define VK_2		0x32
#define VK_3		0x33
#define VK_4		0x34
#define VK_5		0x35
#define VK_6		0x36
#define VK_7		0x37
#define VK_8		0x38
#define VK_9		0x39
#define VK_A		0x41
#define VK_B		0x42
#define VK_C		0x43
#define VK_D		0x44
#define VK_E		0x45
#define VK_F		0x46
#define VK_G		0x47
#define VK_H		0x48
#define VK_I		0x49
#define VK_J		0x4A
#define VK_K		0x4B
#define VK_L		0x4C
#define VK_M		0x4D
#define VK_N		0x4E
#define VK_O		0x4F
#define VK_P		0x50
#define VK_Q		0x51
#define VK_R		0x52
#define VK_S		0x53
#define VK_T		0x54
#define VK_U		0x55
#define VK_V		0x56
#define VK_W		0x57
#define VK_X		0x58
#define VK_Y		0x59
#define VK_Z		0x5A
/////////////////////////////////////////////////////////////////////////////////////////////////
typedef bool (WINAPI* KeyHookInit)(void);
typedef void (WINAPI* KeyHookDestroy)(void);
typedef bool (WINAPI* KeyHookSetHotKey)(HWND, UINT, INT, BYTE, BYTE);
typedef void (WINAPI* KeyHookClearHotKey)(void);
/////////////////////////////////////////////////////////////////////////////////////////////////
// This message will be receive when our key events are captured 
#define WM_KEYHOOK	(WM_APP + 100)
#define WM_KEYHOOK1 (WM_APP + 101)
#define WM_KEYHOOK2 (WM_APP + 102)
/////////////////////////////////////////////////////////////////////////////////////////////////



class keyhookManger
{

public:	
	//creat
	static keyhookManger* Instance(void);
	~keyhookManger(void);
	//method
	bool Init();
	void Destroy();
	bool SetHotKey(HWND nCallWnd, UINT nMessage, INT nVKCode, BYTE nStatus, BYTE nCombKey=0);
	void ClearHotKey();
private:
	//method
	keyhookManger(void);
	

private:
	//var
	//creat
	static keyhookManger*	m_instance;	
	//dll
	HMODULE  				m_hModule;
	KeyHookInit				m_keyHookInit;
	KeyHookDestroy			m_keyHookDestroy;
	KeyHookSetHotKey        m_keyHookSetHotKey;
	KeyHookClearHotKey		m_keyHookClear;
};