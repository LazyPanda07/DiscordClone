#include "Wrappers/SpeakerWrapper.hpp"

#include "Utils.hpp"

namespace wrappers
{
	SpeakerWrapper::SpeakerWrapper(SocketWrapper& socket) :
		implementation(utils::callApiFunction(&createSpeaker, socket.implementation))
	{

	}

	void SpeakerWrapper::overrideDeviceId(uint32_t id)
	{
		utils::callApiFunction(&overrideSpeakerDeviceId, implementation, id);
		utils::callApiFunction(&restartSpeaker, implementation);
	}

	void SpeakerWrapper::setVolume(double volume)
	{
		utils::callApiFunction(&setSpeakerVolume, implementation, volume);
	}

	double SpeakerWrapper::getVolume() const
	{
		return utils::callApiFunction(&getSpeakerVolume, implementation);
	}

	SpeakerWrapper::~SpeakerWrapper()
	{
		deleteSpeaker(implementation);

		implementation = nullptr;
	}
}
