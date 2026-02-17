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
		static constexpr auto registerHotkeyId = WM_USER + 1;
		static constexpr auto initMessageId = registerHotkeyId + 1;

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
		DWORD hotkeysThreadId;
		voice::InputVoice& inputVoice;
		voice::OutputVoice& outputVoice;

	public:
		Implementation(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice);

		void registerHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback, uint32_t modifiers, uint32_t key);

		~Implementation();
	};

	Hotkeys::Hotkeys(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice) :
		implementation(new Implementation(inputVoice, outputVoice))
	{

	}

	void Hotkeys::registerHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback, uint32_t modifiers, uint32_t key)
	{
		implementation->registerHotkey(callback, modifiers, key);
	}

	Hotkeys::~Hotkeys()
	{
		delete implementation;
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

			return 0;
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
		MSG message;

		hotkeysThreadId = GetCurrentThreadId();

		this->createHotkeyWindow();

		SendMessage(hotkeyWindowHandle, Hotkeys::Implementation::initMessageId, reinterpret_cast<WPARAM>(this), 0);

		while (GetMessage(&message, nullptr, 0, 0) > 0)
		{
			if (message.message == Hotkeys::Implementation::registerHotkeyId)
			{
				std::function<void(voice::InputVoice&, voice::OutputVoice&)>* callback = reinterpret_cast<std::function<void(voice::InputVoice&, voice::OutputVoice&)>*>(message.wParam);
				std::tuple<uint32_t, uint32_t>* hotkey = reinterpret_cast<std::tuple<uint32_t, uint32_t>*>(message.lParam);
				int id = currentHotkeyIndex++;

				RegisterHotKey(hotkeyWindowHandle, id, std::get<0>(*hotkey), std::get<1>(*hotkey));

				hotkeys.emplace(id, *callback);

				delete callback;
				delete hotkey;

				continue;
			}

			TranslateMessage(&message);

			DispatchMessage(&message);
		}
	}

	Hotkeys::Implementation::Implementation(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice) :
		hInstance(GetModuleHandle(nullptr)),
		hotkeyWindowHandle(nullptr),
		currentHotkeyIndex(0),
		hotkeysThreadId(0),
		inputVoice(inputVoice),
		outputVoice(outputVoice)
	{
		messageLoopFuture = std::async(std::launch::async, &Hotkeys::Implementation::messageLoop, this);
	}

	void Hotkeys::Implementation::registerHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback, uint32_t modifiers, uint32_t key)
	{
		std::thread
		(
			[=, this]()
			{
				std::this_thread::sleep_for(std::chrono::seconds(5));

				PostThreadMessage
				(
					hotkeysThreadId,
					Hotkeys::Implementation::registerHotkeyId,
					reinterpret_cast<WPARAM>(new std::function<void(voice::InputVoice&, voice::OutputVoice&)>(callback)),
					reinterpret_cast<LPARAM>(new std::tuple<uint32_t, uint32_t>(modifiers, key))
				);
			}
		).detach();
	}

	Hotkeys::Implementation::~Implementation()
	{
		PostThreadMessage(hotkeysThreadId, WM_QUIT, 0, 0);

		if (messageLoopFuture.valid())
		{
			messageLoopFuture.wait();
		}
	}
}
