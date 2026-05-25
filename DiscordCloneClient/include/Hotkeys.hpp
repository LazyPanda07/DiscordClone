#pragma once

#include <memory>
#include <functional>

#include <InputVoice.hpp>
#include <OutputVoice.hpp>

namespace functionality
{
	class Hotkeys
	{
	private:
		class Implementation;

	private:
		Implementation* implementation;

	public:
		Hotkeys(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice);
		
		void registerHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback, uint32_t modifiers, uint32_t key);

		~Hotkeys();
	};
}
