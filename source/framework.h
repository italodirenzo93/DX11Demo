#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

// For winrt::com_ptr<T>
#include <winrt/base.h>

#ifdef WINDOWS_UWP
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Graphics.Display.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>
#endif

// DirectX headers
#include <d3d11_4.h>
#pragma comment (lib, "d3d11.lib")

#include <dxgi1_6.h>
#pragma comment (lib, "dxgi.lib")

#include <d2d1_1.h>
#pragma comment (lib, "d2d1.lib")

#include <dwrite_3.h>
#pragma comment (lib, "dwrite.lib")

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <DirectXColors.h>
#include <DirectXMath.h>

// STL headers
#include <iostream>
#include <exception>
#include <memory>
#include <string>
#include <vector>

// DirectXTK helpers
#include <BufferHelpers.h>
#include <CommonStates.h>
#include <Effects.h>
#include <GamePad.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <SimpleMath.h>
#include <VertexTypes.h>

namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) noexcept : result(hr) {}

		const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
#ifdef _DEBUG
			char str[64] = {};
			sprintf_s(str, "**ERROR** Fatal Error with HRESULT of %08X\n", static_cast<unsigned int>(hr));
			OutputDebugStringA(str);
			__debugbreak();
#endif
			throw com_exception(hr);
		}
	}
}

