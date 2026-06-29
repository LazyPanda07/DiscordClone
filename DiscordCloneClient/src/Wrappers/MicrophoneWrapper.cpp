#include "Wrappers/MicrophoneWrapper.hpp"

#include "Utils.hpp"

namespace wrappers
{
	MicrophoneWrapper::MicrophoneWrapper(SocketWrapper& socket) :
		implementation(utils::callApiFunction(&createMicrophone, socket.implementation))
	{
		
	}

	void MicrophoneWrapper::overrideDeviceId(uint32_t id)
	{
		utils::callApiFunction(&overrideMicrophoneDeviceId, implementation, id);
		utils::callApiFunction(&restartMicrophone, implementation);
	}

	void MicrophoneWrapper::muteOrUnmute()
	{
		utils::callApiFunction(&::muteOrUnmute, implementation);
	}

	bool MicrophoneWrapper::isStreamRunning() const
	{
		return utils::callApiFunction(&::isStreamRunning, implementation);
	}

	void MicrophoneWrapper::sendSilence()
	{
		utils::callApiFunction(&::sendSilence, implementation);
	}

	void MicrophoneWrapper::setVolume(double volume)
	{
		utils::callApiFunction(&setMicrophoneVolume, implementation, volume);
	}

	double MicrophoneWrapper::getVolume() const
	{
		return utils::callApiFunction(&getMicrophoneVolume, implementation);
	}

	MicrophoneWrapper::~MicrophoneWrapper()
	{
		deleteMicrophone(implementation);

		implementation = nullptr;
	}
}
