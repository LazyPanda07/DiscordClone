#include "Hotkeys.hpp"

#include <unordered_map>

namespace functionality
{
	class Hotkeys::Implementation
	{
	private:
		std::unordered_map<int, std::function<void(voice::InputVoice&, voice::OutputVoice&)>> hotkeys;
		int currentHotkeyIndex;
		voice::InputVoice& inputVoice;
		voice::OutputVoice& outputVoice;

	public:
		Implementation(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice);

		void registerHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback);

		~Implementation();
	};

	Hotkeys::Hotkeys(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice) :
		implementation(new Implementation(inputVoice, outputVoice))
	{

	}

	void Hotkeys::addHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback)
	{
		
	}

	Hotkeys::~Hotkeys()
	{
		delete implementation;
	}
}

namespace functionality
{
	Hotkeys::Implementation::Implementation(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice) :
		currentHotkeyIndex(0),
		inputVoice(inputVoice),
		outputVoice(outputVoice)
	{
		
	}

	void Hotkeys::Implementation::registerHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback)
	{
		hotkeys.emplace(currentHotkeyIndex++, callback);
	}

	Hotkeys::Implementation::~Implementation()
	{
		
	}
}
