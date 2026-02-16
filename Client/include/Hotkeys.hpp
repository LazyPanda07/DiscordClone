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
		std::unique_ptr<Implementation> implementation;

	public:
		Hotkeys(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice);
		
		void addHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback);

		~Hotkeys() = default;
	};
}
