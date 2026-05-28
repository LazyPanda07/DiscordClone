#pragma once

#include <memory>
#include <functional>

namespace functionality
{
	class Hotkeys
	{
	private:
		class Implementation;

	private:
		Implementation* implementation;

	public:
		Hotkeys();
		
		void registerHotkey(const std::function<void()>& callback, uint32_t modifiers, uint32_t key);

		~Hotkeys();
	};
}
