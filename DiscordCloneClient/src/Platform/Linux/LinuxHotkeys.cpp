#include "Hotkeys.hpp"

#include <unordered_map>

namespace functionality
{
	class Hotkeys::Implementation
	{
	private:
		std::unordered_map<int, std::function<void()>> hotkeys;
		int currentHotkeyIndex;

	public:
		Implementation();

		void registerHotkey(const std::function<void()>& callback, uint32_t modifiers, uint32_t key);

		~Implementation();
	};

	Hotkeys::Hotkeys() :
		implementation(new Implementation())
	{

	}

	void Hotkeys::registerHotkey(const std::function<void()>& callback, uint32_t modifiers, uint32_t key)
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
	Hotkeys::Implementation::Implementation() :
		currentHotkeyIndex(0)
	{
		
	}

	void Hotkeys::Implementation::registerHotkey(const std::function<void()>& callback, uint32_t modifiers, uint32_t key)
	{
		
	}

	Hotkeys::Implementation::~Implementation()
	{
		
	}
}
