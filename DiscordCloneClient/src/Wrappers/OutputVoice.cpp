#include "Wrappers/OutputVoice.hpp"

#include "Utils.hpp"

namespace wrapper
{
	OutputVoice::OutputVoice(SocketWrapper& socket) :
		implementation(utils::callApiFunction(&createOutputVoiceStream, socket.implementation))
	{

	}

	void OutputVoice::overrideDeviceId(uint32_t id)
	{
		utils::callApiFunction(&overrideOutputDeviceId, implementation, id);
		utils::callApiFunction(&restartOutput, implementation);
	}

	void OutputVoice::setVolume(double volume)
	{
		utils::callApiFunction(&setInputVolume, implementation, volume);
	}

	double OutputVoice::getVolume() const
	{
		return utils::callApiFunction(&getInputVolume, implementation);
	}

	OutputVoice::~OutputVoice()
	{
		deleteOutputVoiceStream(implementation);

		implementation = nullptr;
	}
}
