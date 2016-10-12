#include "stdafx.h"
#include "keyhookManger.h"

#define Release(x) { if (x) delete x; x = NULL;}
keyhookManger* keyhookManger::m_instance = NULL;




keyhookManger* keyhookManger::Instance()
{
	if (m_instance == NULL){
		m_instance = new keyhookManger(); 
	}
	return m_instance;
}


keyhookManger::keyhookManger()
{
	m_hModule			= NULL;
	m_keyHookInit		= NULL;
	m_keyHookDestroy	= NULL;
	m_keyHookSetHotKey	= NULL;
	m_keyHookClear		= NULL;
}

keyhookManger::~keyhookManger()
{

	Release(m_hModule);
	Release(m_instance);
}


bool keyhookManger::Init()
{
	m_hModule = LoadLibrary(L".\\keyhook.dll");

	if ( m_hModule != NULL )
	{
		m_keyHookInit		= (KeyHookInit)GetProcAddress( m_hModule, "InitKeyboardHook" );
		m_keyHookDestroy	= (KeyHookDestroy)GetProcAddress( m_hModule, "UninstallKeyHook" );
		m_keyHookSetHotKey	= (KeyHookSetHotKey)GetProcAddress( m_hModule, "SetHotKey" );
		m_keyHookClear      = (KeyHookClearHotKey)GetProcAddress( m_hModule, "ClearHotKey" );

		if (m_keyHookInit == NULL || m_keyHookDestroy == NULL || m_keyHookSetHotKey == NULL || m_keyHookClear == NULL)
		{
			FreeLibrary(m_hModule);
			return false;
		}

		if (m_keyHookInit() == false)
		{
			OutputDebugString(L"keyhookManger::creat keyhook fault!");
		}

	}else{
		OutputDebugString(L"keyhookManger::load keyhookDLL fault!");
		return false;
	}

	return true;
}

void keyhookManger::Destroy()
{
	m_keyHookDestroy();
}


bool keyhookManger::SetHotKey(HWND nCallWnd, UINT nMessage, INT nVKCode, BYTE nStatus, BYTE nCombKey)
{
	return m_keyHookSetHotKey(nCallWnd, nMessage, nVKCode, nStatus, nCombKey);
}

void keyhookManger::ClearHotKey()
{
	m_keyHookClear();
}