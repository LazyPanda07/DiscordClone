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
	Hotkeys::Implementation::Implementation(voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice) :
		currentHotkeyIndex(0),
		inputVoice(inputVoice),
		outputVoice(outputVoice)
	{
		
	}

	void Hotkeys::Implementation::registerHotkey(const std::function<void(voice::InputVoice&, voice::OutputVoice&)>& callback, uint32_t modifiers, uint32_t key)
	{
		
	}

	Hotkeys::Implementation::~Implementation()
	{
		
	}
}
