#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include "DXUtil.h"

class DXApp
{
public:
	DXApp(HINSTANCE hInstance);
	virtual ~DXApp(void);

	// Main application loop
	int Run();

	// Framework Methods
	virtual bool Init();
	virtual void Update(float dt) = 0;
	virtual void Render(float dt) = 0;

protected:
	// Wind32 Attributes
	HWND		m_hWnd;
	HINSTANCE	m_hInstance;
	UINT		m_ClientWidth;
	UINT		m_ClientHeight;
	std::string	m_AppTitle;
	DWORD		m_WndStyle;

	// DirectX Attributes
	ID3D11Device			*m_pDevice;
	ID3D11DeviceContext		*m_pImmediateContext;
	IDXGISwapChain			*m_pSwapChain;
	ID3D11RenderTargetView	*m_pRenderTargetView;
	D3D_DRIVER_TYPE			m_DriverType;
	D3D_FEATURE_LEVEL		m_Featurelevel;
	D3D11_VIEWPORT			m_Viewport;

protected:
	// Init Win32 Window
	bool InitWindow();

	// Init Direct 3D
	bool InitDirect3D();
};