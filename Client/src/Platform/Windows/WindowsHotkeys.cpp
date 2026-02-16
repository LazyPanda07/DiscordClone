#include "Hotkeys.hpp"

#include <unordered_map>
#include <future>

#include <Windows.h>

namespace functionality
{
	class Hotkeys::Implementation
	{
	private:
		static constexpr auto windowClassName = TEXT("HotkeyWindow");
		static constexpr auto initMessageId = WM_USER + 1;

	private:
		static LRESULT hotkeyWindowsProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	private:
		void createHotkeyWindow();

		void messageLoop();

	private:
		std::unordered_map<int, std::function<void(voice::InputVoice&, voice::OutputVoice&)>> hotkeys;
		std::future<void> messageLoopFuture;
		HINSTANCE hInstance;
		HWND hotkeyWindowHandle;
		int currentHotkeyIndex;
		voice::InputVoice& inputVoice;
		voice::OutputVoice& outputVoice;

	public:
		Implementation(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice);

		void addHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback);

		~Implementation();
	};

	Hotkeys::Hotkeys(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice) :
		implementation(std::make_unique<Implementation>(inputVoice, outputVoice))
	{
		
	}

	void Hotkeys::addHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback)
	{
		implementation->addHotkey(callback);
	}
}

namespace functionality
{
	LRESULT Hotkeys::Implementation::hotkeyWindowsProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		static Hotkeys::Implementation* implementation = nullptr;

		if (msg == WM_HOTKEY)
		{
			if (auto it = implementation->hotkeys.find(static_cast<int>(wparam)); it != implementation->hotkeys.end())
			{
				it->second(implementation->inputVoice, implementation->outputVoice);
			}

			return 0;
		}
		else if (msg == Hotkeys::Implementation::initMessageId)
		{
			implementation = reinterpret_cast<Hotkeys::Implementation*>(wparam);
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	void Hotkeys::Implementation::createHotkeyWindow()
	{
		WNDCLASS windowClass{};

		windowClass.lpfnWndProc = &Hotkeys::Implementation::hotkeyWindowsProcedure;
		windowClass.hInstance = hInstance;
		windowClass.lpszClassName = Implementation::windowClassName;

		RegisterClass(&windowClass);

		hotkeyWindowHandle = CreateWindowEx
		(
			0,
			windowClass.lpszClassName,
			TEXT(""),
			0,
			0, 0, 0, 0,
			HWND_MESSAGE,
			nullptr,
			hInstance,
			nullptr
		);
	}

	void Hotkeys::Implementation::messageLoop()
	{
		MSG msg;

		while (GetMessage(&msg, nullptr, 0, 0) > 0)
		{
			TranslateMessage(&msg);

			DispatchMessage(&msg);
		}
	}

	Hotkeys::Implementation::Implementation(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice) :
		hInstance(GetModuleHandle(nullptr)),
		currentHotkeyIndex(0),
		inputVoice(inputVoice),
		outputVoice(outputVoice)
	{
		this->createHotkeyWindow();

		RegisterHotKey(hotkeyWindowHandle, 1, MOD_CONTROL | MOD_ALT, VK_SPACE);

		messageLoopFuture = std::async(std::launch::async, &Hotkeys::Implementation::messageLoop, this);

		SendMessage(hotkeyWindowHandle, Hotkeys::Implementation::initMessageId, reinterpret_cast<WPARAM>(this), 0);
	}

	void Hotkeys::Implementation::addHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback)
	{
		hotkeys.emplace(currentHotkeyIndex++, callback);
	}

	Hotkeys::Implementation::~Implementation()
	{
		SendMessage(hotkeyWindowHandle, WM_QUIT, 0, 0);

		for (const auto& [hotkeyId, _] : hotkeys)
		{
			UnregisterHotKey(hotkeyWindowHandle, hotkeyId);
		}

		if (messageLoopFuture.valid())
		{
			messageLoopFuture.wait();
		}

		UnregisterClass(Implementation::windowClassName, hInstance);
	}
}
