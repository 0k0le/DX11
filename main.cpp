#include "DXApp.h"

// Create class that derives from DXApp
class MainApp : public DXApp
{
public:
	// Contruct / Deconstruct
	MainApp(HINSTANCE hInstance);
	~MainApp();

	// Overwrite These Function
	virtual bool Init() override;
	virtual void Update(float dt) override;
	virtual void Render(float dt) override;
};

MainApp::MainApp(HINSTANCE hInstance) : DXApp(hInstance) {

}

MainApp::~MainApp() {

}

bool MainApp::Init() {
	// Call Standard Init
	return DXApp::Init();
}

// Update
void MainApp::Update(float dt) {

}

// Render
void MainApp::Render(float dt) {
	if (!m_pImmediateContext)
		return;

	// Render an image
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView,
		DirectX::Colors::CornflowerBlue);

	// Display image
	if (m_pSwapChain->Present(0, 0) != S_OK) {
		PostQuitMessage(1);
	}
}

// Main
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd) {
	// Create Class
	MainApp tApp(hInstance);

	// Init screen
	if (!tApp.Init()) return 1;

	// Run Display
	return tApp.Run();
}