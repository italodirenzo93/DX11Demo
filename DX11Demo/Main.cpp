#include "pch.h"
#include "SampleScene.h"

using namespace DirectX;

namespace
{
	std::unique_ptr<SampleScene> g_scene;
}

LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
HWND InitWindow(HINSTANCE, int, int, int);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);

	winrt::init_apartment();

	if (!XMVerifyCPUSupport())
	{
		MessageBox(nullptr, L"CPU unsupported.", L"Error", MB_OK);
		return 1;
	}

	g_scene = std::make_unique<SampleScene>();

	int w, h;
	g_scene->GetDefaultSize(w, h);

	// Create application window
	HWND hWnd = InitWindow(hInstance, nCmdShow, w, h);

	if (!hWnd)
	{
		MessageBox(nullptr, L"Unable to create an application window.", L"Error", MB_OK);
		return 1;
	}

	g_scene->Initialize(hWnd, w, h);

	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// do stuff
			g_scene->Tick();
		}
	}

	g_scene.reset();

	winrt::uninit_apartment();

	return static_cast<int>(msg.wParam);
}


LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SampleScene* scene = reinterpret_cast<SampleScene*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_GETMINMAXINFO:
	{
		auto minMaxInfo = (MINMAXINFO*)lParam;
		minMaxInfo->ptMinTrackSize.x = 320;
		minMaxInfo->ptMinTrackSize.y = 240;
	}
		break;

	case WM_SIZE:
		if (scene && wParam != SIZE_MINIMIZED)
		{
			scene->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_PAINT:
		if (g_scene)
		{
			g_scene->Tick();
		}
		else
		{
			PAINTSTRUCT ps = {};
			std::ignore = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(uMsg, wParam, lParam);
		Mouse::ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
		}
		Keyboard::ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond
		// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_MOUSEACTIVATE:
		// When you click to activate the window, we want Mouse to ignore that event.
		return MA_ACTIVATEANDEAT;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND InitWindow(HINSTANCE hInstance, int nCmdShow, int width, int height)
{
	static constexpr wchar_t className[] = L"DX11Demo";

	// Create window class
	{
		WNDCLASSEX wcex = {};

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpfnWndProc = WndProc;
		wcex.lpszClassName = className;
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.hInstance = hInstance;

		if (!RegisterClassEx(&wcex))
		{
			return nullptr;
		}
	}

	// Create window
	RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd = CreateWindow(
		className,
		L"DirectX 11 Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd)
	{
		return nullptr;
	}

	ShowWindow(hWnd, nCmdShow);

	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_scene.get()));

	GetClientRect(hWnd, &rect);

	UnregisterClass(className, hInstance);

	return hWnd;
}

void ExitApplication() noexcept
{
	PostQuitMessage(0);
}
