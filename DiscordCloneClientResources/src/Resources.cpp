#include "Resources.hpp"

#include "Generated/microphone_on.generated.hpp"
#include "Generated/microphone_off.generated.hpp"

const uint8_t* getMicrophoneOffSound(uint64_t* size)
{
	*size = sizeof(microphoneOffWav);

	return microphoneOffWav;
}

const uint8_t* getMicrophoneOnSound(uint64_t* size)
{
	*size = sizeof(microphoneOnWav);

	return microphoneOnWav;
}
