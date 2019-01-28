#include "DXApp.h"

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		// If quit message
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Else do default windows shit
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

// Init variables
DXApp::DXApp(HINSTANCE hInstance) {
	m_hInstance = hInstance;
	m_hWnd = NULL;
	m_ClientWidth = 800;
	m_ClientHeight = 600;
	m_AppTitle = "Win32 Setup";
	m_WndStyle = WS_OVERLAPPEDWINDOW;

	m_pDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pSwapChain = nullptr;
}

DXApp::~DXApp(void) {
	// Clean up
	if (m_pImmediateContext) m_pImmediateContext->ClearState();
	Memory::SafeRelease(m_pRenderTargetView);
	Memory::SafeRelease(m_pSwapChain);
	Memory::SafeRelease(m_pImmediateContext);
	Memory::SafeRelease(m_pDevice);
}

int DXApp::Run() {
	MSG msg = {0};

	// Main loop
	while (WM_QUIT != msg.message) {
		// Check and dispatch messages to CALLBACK
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//Update
			Update(0.0f);

			//Render
			Render(0.0f);
		}
	}

	// Return Exit Code
	return static_cast<int>(msg.wParam);
}

// Init Window
bool DXApp::Init() {
	// Initialize windows "window"
	if (!InitWindow())
		return false;

	// Init D3D Window
	if (!InitDirect3D())
		return false;

	return true;
}

bool DXApp::InitWindow() {
	// WNDCLASSEX
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	// Define Windows Shit
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = m_hInstance;
	wcex.lpfnWndProc = MainWndProc;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "DXAPPWNDCLASS";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Register Windows with OS
	if (!RegisterClassEx(&wcex)) {
		OutputDebugString("\n\n\nFAILED TO CREATE WND CLASS\n\n\n");
		return false;
	}

	// Adjust screen size
	RECT r = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&r, m_WndStyle, FALSE);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	// Define screen coords to be center
	UINT x = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	UINT y = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

	// Create window
	m_hWnd = CreateWindow("DXAPPWNDCLASS", m_AppTitle.c_str(), m_WndStyle,
		x, y, width, height, NULL, NULL, m_hInstance, NULL);

	if (!m_hWnd) {
		OutputDebugString("\n\n\nFAILED TO CREATE WINDOW\n\n\n");
		return false;
	}

	// Show window
	ShowWindow(m_hWnd, SW_SHOW);

	return true;
}

bool DXApp::InitDirect3D() {
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	// Driver order
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	// D3D Types
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	// Swap Chain(FRONT AND BACK BUFFER SWAPING)
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 32Bit Color
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;				// 60 FPS
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// Output to Window
	swapDesc.OutputWindow = m_hWnd;								// Output window
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;				// Discard swap effects(for now)
	swapDesc.Windowed = true;
	swapDesc.SampleDesc.Count = 1;								// Multi sampling
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//alt-enter fullscreen

	// Create device and swap chain
	HRESULT result;
	for (UINT i = 0; i < numDriverTypes; i++) {
		result = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[i], NULL, createDeviceFlags,
			featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice,
			&m_Featurelevel, &m_pImmediateContext);
		if (SUCCEEDED(result)) {
			m_DriverType = driverTypes[i];
			break;
		}
	}

	if(FAILED(result)) {
		OutputDebugString("\n\n\nFAILED TO CREATE DEVICE AND SWAP CHAIN\n\n\n");
		return false;
	}

	// Create Render Target View
	ID3D11Texture2D* m_pBackBufferTex = 0;
	result = m_pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&m_pBackBufferTex));

	if (FAILED(result)) {
		OutputDebugString("\n\n\nFAILED TO GET BUFFER\n\n\n");
		return false;
	}

	result = m_pDevice->CreateRenderTargetView(m_pBackBufferTex, nullptr, &m_pRenderTargetView);
	if (FAILED(result)) {
		OutputDebugString("\n\n\nFAILED TO CREATE TARGET VIEW\n\n\n");
		return false;
	}

	// Bind Render Target View
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

	// Viewport creation
	m_Viewport.Width = static_cast<float>(m_ClientWidth);
	m_Viewport.Height = static_cast<float>(m_ClientHeight);
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Bind Viewport
	m_pImmediateContext->RSSetViewports(1, &m_Viewport);

	return true;
}