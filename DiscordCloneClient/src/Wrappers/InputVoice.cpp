#include "Wrappers/InputVoice.hpp"

#include "Utils.hpp"

namespace wrapper
{
	InputVoice::InputVoice(SocketWrapper& socket) :
		implementation(utils::callApiFunction(&createInputVoiceStream, socket.implementation))
	{
		
	}

	void InputVoice::overrideDeviceId(uint32_t id)
	{
		utils::callApiFunction(&overrideInputDeviceId, implementation, id);
		utils::callApiFunction(&restartInput, implementation);
	}

	void InputVoice::muteOrUnmute()
	{
		utils::callApiFunction(&::muteOrUnmute, implementation);
	}

	void InputVoice::setVolume(double volume)
	{
		utils::callApiFunction(&setInputVolume, implementation, volume);
	}

	double InputVoice::getVolume() const
	{
		return utils::callApiFunction(&getInputVolume, implementation);
	}

	InputVoice::~InputVoice()
	{
		deleteInputVoiceStream(implementation);

		implementation = nullptr;
	}
}
