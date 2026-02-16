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
		Implementation* implementation;

	public:
		Hotkeys(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice);
		
		void registerHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback);

		~Hotkeys();
	};
}
