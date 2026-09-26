#include "c_api.h"

#include <chrono>
#include <filesystem>

#include <UDPClientSocket.hpp>
#include <TCPClientSocket.hpp>
#include <PatternParser.h>
#include <opencv2/core/utils/logger.hpp>
#include <dxcam/dxcam.h>

#include <cuda.h>

#ifdef __LINUX__

#else
#include <NvCodec/NvDecoder/NvDecoder.h>
#include <NvCodec/NvEncoder/NvEncoderCuda.h>
#endif

#ifdef __LINUX__
#include <dlfcn.h>
#else
#include <Windows.h>
#include <mmsystem.h>
#endif

#include "Functionality.hpp"
#include "Microphone.hpp"
#include "Speaker.hpp"

#ifdef __LINUX__
using HMODULE = void*;
#endif

static constexpr uint32_t sampleRate = 48'000;
static constexpr uint32_t frameSize = 480;
static void* resourceLibrary = nullptr;

template<>
struct utility::parsers::Converter<int32_t>
{
	constexpr void convert(std::string_view data, int32_t& result)
	{
		result = std::stoi(data.data());
	}
};

class ScreenCapturerData
{
private:
	std::string windowName;

#ifdef __LINUX__

#else
	CUcontext context;
	std::unique_ptr<NvEncoderCuda> encoder;
	std::unique_ptr<NvDecoder> decoder;
#endif

public:
	std::shared_ptr<DXCam::DXCamera> capturer;

public:
	ScreenCapturerData(uint32_t width, uint32_t height, int32_t qualityPreset, bool showPreview);

	cv::Mat processFrame(cv::Mat& frame);

	~ScreenCapturerData();
};

using GetResourceSignature = const uint8_t* (*)(uint64_t*);

static void loadResourceLibrary();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initialize(Exception* exception)
{
	try
	{
		loadResourceLibrary();

		cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

UdpSocketObject createSocket(const char* ip, uint16_t port, Exception* exception)
{
	try
	{
		return new web::UDPClientSocket(ip, port);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

TcpSocketObject createTcpSocket(const char* ip, uint16_t port, Exception* exception)
{
	try
	{
		return new web::TCPClientSocket(ip, port);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

MicrophoneObject createMicrophone(UdpSocketObject socket, Exception* exception)
{
	try
	{
		return new voice::Microphone(*reinterpret_cast<web::UDPSocket*>(socket), frameSize, sampleRate);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

SpeakerObject createSpeaker(UdpSocketObject socket, Exception* exception)
{
	try
	{
		return new voice::Speaker(*reinterpret_cast<web::UDPSocket*>(socket), frameSize, sampleRate);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sendData(UdpSocketObject socket, const char* data, uint64_t size, Exception* exception)
{
	try
	{
		static_cast<web::UDPSocket*>(socket)->sendData(std::string_view(data, size));
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void receiveData(UdpSocketObject socket, void(*callback)(const char* data, uint64_t size, void* userData), int32_t flags, void* userData, Exception* exception)
{
	try
	{
		static_cast<web::UDPSocket*>(socket)->receiveData
		(
			[callback, userData](const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
			{
				if (size == SOCKET_ERROR)
				{
#ifdef __LINUX__
					throw std::runtime_error(std::format("Can't send data: {}", strerror(errno)));
#else

					throw std::runtime_error(std::format("Can't send data: {}", WSAGetLastError()));
#endif
				}

				callback(data.data(), size, userData);
			},
			flags
		);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void sendTcpData(TcpSocketObject socket, const char* data, uint64_t size, Exception* exception)
{
	try
	{
		static_cast<web::TCPSocket*>(socket)->sendBytes(data, static_cast<int>(size));
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void receiveNotification(TcpSocketObject socket, void(*callback)(const char* data, uint64_t size, void* userData), void* userData, Exception* exception)
{
	try
	{
		std::array<char, web::TCPSocket::notificationSize> notification{};
		uint64_t actualSize = static_cast<web::TCPSocket*>(socket)->receiveBytes(notification.data(), notification.size());

		callback(notification.data(), actualSize, userData);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void overrideMicrophoneDeviceId(MicrophoneObject microphone, uint32_t id, Exception* exception)
{
	try
	{
		static_cast<voice::Microphone*>(microphone)->overrideDeviceId(id);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void muteOrUnmute(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		functionality::muteOrUnmute(*static_cast<voice::Microphone*>(microphone));
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

bool isStreamRunning(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		return static_cast<voice::Microphone*>(microphone)->isStreamRunning();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return false;
}

void restartMicrophone(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		static_cast<voice::Microphone*>(microphone)->restart();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void sendSilence(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		static_cast<voice::Microphone*>(microphone)->sendSilence();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void setMicrophoneVolume(MicrophoneObject microphone, double volume, Exception* exception)
{
	try
	{
		static_cast<voice::Microphone*>(microphone)->setVolume(volume);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

double getMicrophoneVolume(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		return static_cast<voice::Microphone*>(microphone)->getVolume();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0.0;
}

void overrideSpeakerDeviceId(SpeakerObject speaker, uint32_t id, Exception* exception)
{
	try
	{
		static_cast<voice::Speaker*>(speaker)->overrideDeviceId(id);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void restartSpeaker(SpeakerObject speaker, Exception* exception)
{
	try
	{
		static_cast<voice::Speaker*>(speaker)->restart();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void fixSpeakerDelay(SpeakerObject speaker, Exception* exception)
{
	try
	{
		static_cast<voice::Speaker*>(speaker)->fixSpeakerDelay();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

ScreenCapturer startStream(uint32_t width, uint32_t height, int32_t qualityPreset, bool showPreview, Exception* exception)
{
	try
	{
		return new ScreenCapturerData(width, height, qualityPreset, showPreview);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

void processFrame(UdpSocketObject socket, ScreenCapturer capturer, Exception* exception)
{
	try
	{
		ScreenCapturerData& data = *static_cast<ScreenCapturerData*>(capturer);
		cv::Mat frame; 
		
		while (frame.empty())
		{
			frame = data.capturer->grab();
		}

		cv::Mat result = data.processFrame(frame);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void stopStream(ScreenCapturer capturer, Exception* exception)
{
	try
	{
		delete static_cast<ScreenCapturerData*>(capturer);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void setSpeakerVolume(SpeakerObject microphone, double volume, Exception* exception)
{
	try
	{
		static_cast<voice::Speaker*>(microphone)->setVolume(volume);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

double getSpeakerVolume(SpeakerObject microphone, Exception* exception)
{
	try
	{
		return static_cast<voice::Speaker*>(microphone)->getVolume();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0.0;
}

DeviceInformationArray getDeviceInformation(Exception* exception)
{
	try
	{
		std::vector<RtAudio::DeviceInfo>* result = new std::vector<RtAudio::DeviceInfo>();
		std::vector<RtAudio::DeviceInfo> devices = functionality::getAudioDevices();

		for (RtAudio::DeviceInfo& device : devices)
		{
			result->emplace_back(std::move(device));
		}

		return result;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

uint64_t getDeviceInformationSize(DeviceInformationArray deviceInformation, Exception* exception)
{
	try
	{
		return static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation)->size();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

uint32_t getDeviceInformationId(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].ID;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

const char* getDeviceInformationName(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].name.data();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

uint32_t getDeviceInformationInputChannels(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].inputChannels;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

uint32_t getDeviceInformationOutputChannels(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].outputChannels;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

bool getDeviceInformationDefaultInput(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].isDefaultInput;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return false;
}

bool getDeviceInformationDefaultOutput(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].isDefaultOutput;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return false;
}

const char* getVersion(Exception* exception)
{
	try
	{
		return functionality::getDiscordCloneClientLibraryVersion().data();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

void getVersionExtended(int32_t* major, int32_t* minor, int32_t* patch, Exception* exception)
{
	constexpr utility::parsers::PatternParser<int32_t, int32_t, int32_t> parser("{}.{}.{}");

	try
	{
		parser.parse(functionality::getDiscordCloneClientLibraryVersion(), *major, *minor, *patch);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

const char* getExceptionMessage(Exception exception)
{
	if (!exception)
	{
		return nullptr;
	}

	return static_cast<std::runtime_error*>(exception)->what();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void playMicrophoneOffSound(Exception* exception)
{
	try
	{
		if (!resourceLibrary)
		{
			return;
		}

		uint64_t size = 0;

#ifdef __LINUX__
		const uint8_t* data = reinterpret_cast<GetResourceSignature>(dlsym(resourceLibrary, "getMicrophoneOffSound"))(&size);
#else
		const uint8_t* data = reinterpret_cast<GetResourceSignature>(GetProcAddress(static_cast<HMODULE>(resourceLibrary), "getMicrophoneOffSound"))(&size);

		PlaySoundA(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(data)), nullptr, SND_MEMORY | SND_ASYNC);
#endif
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void playMicrophoneOnSound(Exception* exception)
{
	try
	{
		if (!resourceLibrary)
		{
			return;
		}

		uint64_t size = 0;

#ifdef __LINUX__
		const uint8_t* data = reinterpret_cast<GetResourceSignature>(dlsym(resourceLibrary, "getMicrophoneOnSound"))(&size);
#else
		const uint8_t* data = reinterpret_cast<GetResourceSignature>(GetProcAddress(static_cast<HMODULE>(resourceLibrary), "getMicrophoneOnSound"))(&size);

		PlaySoundA(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(data)), nullptr, SND_MEMORY | SND_ASYNC);
#endif
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void playJoinSound(Exception* exception)
{
	try
	{
		if (!resourceLibrary)
		{
			return;
		}

		uint64_t size = 0;

#ifdef __LINUX__
		const uint8_t* data = reinterpret_cast<GetResourceSignature>(dlsym(resourceLibrary, "getJoinSound"))(&size);
#else
		const uint8_t* data = reinterpret_cast<GetResourceSignature>(GetProcAddress(static_cast<HMODULE>(resourceLibrary), "getJoinSound"))(&size);

		PlaySoundA(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(data)), nullptr, SND_MEMORY | SND_ASYNC);
#endif
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void deleteSocket(UdpSocketObject socket)
{
	delete static_cast<web::UDPSocket*>(socket);
}

void deleteTcpSocket(TcpSocketObject socket)
{
	delete static_cast<web::TCPSocket*>(socket);
}

void deleteMicrophone(MicrophoneObject microphone)
{
	delete static_cast<voice::Microphone*>(microphone);
}

void deleteSpeaker(SpeakerObject speaker)
{
	delete static_cast<voice::Speaker*>(speaker);
}

void deleteException(Exception exception)
{
	delete static_cast<std::runtime_error*>(exception);
}

void deleteDeviceInformation(DeviceInformationArray deviceInformation)
{
	delete static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loadResourceLibrary()
{
	if (resourceLibrary)
	{
		return;
	}

	std::filesystem::path currentPath(std::filesystem::current_path());

#ifdef __LINUX__
	currentPath /= "libDiscordCloneClientResources.so";

	resourceLibrary = dlopen(currentPath.string().data(), RTLD_LAZY);
#else
	currentPath /= "DiscordCloneClientResources.dll";

	resourceLibrary = LoadLibraryA(currentPath.string().data());
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ScreenCapturerData::ScreenCapturerData(uint32_t width, uint32_t height, int32_t qualityPreset, bool showPreview) :
	capturer(DXCam::create())
{
#ifdef __LINUX__

#else
	context = nullptr;

	ck(cuInit(0));

	CUdevice device = 0;
	int gpu = 0;

	ck(cuDeviceGet(&device, gpu));

	ck(NVCODEC_CUDA_CTX_CREATE(&context, 0, device));

	encoder = std::make_unique<NvEncoderCuda>(context, width, height, NV_ENC_BUFFER_FORMAT::NV_ENC_BUFFER_FORMAT_IYUV);
	decoder = std::make_unique<NvDecoder>(context, false, cudaVideoCodec_HEVC, true);

	NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
	NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };

	initializeParams.encodeConfig = &encodeConfig;

	const GUID* preset = nullptr;

	switch (qualityPreset)
	{
	case 0:
		preset = &NV_ENC_PRESET_P3_GUID;

		break;

	case 1:
		preset = &NV_ENC_PRESET_P1_GUID;
		break;

	case 2:
		preset = &NV_ENC_PRESET_P2_GUID;
		break;

	case 3:
		preset = &NV_ENC_PRESET_P3_GUID;
		break;

	case 4:
		preset = &NV_ENC_PRESET_P4_GUID;
		break;

	case 5:
		preset = &NV_ENC_PRESET_P5_GUID;
		break;

	case 6:
		preset = &NV_ENC_PRESET_P6_GUID;
		break;

	case 7:
		preset = &NV_ENC_PRESET_P7_GUID;
		break;

	default:
		preset = &NV_ENC_PRESET_P3_GUID;

		break;
	}

	encoder->CreateDefaultEncoderParams(&initializeParams, NV_ENC_CODEC_HEVC_GUID, *preset, NV_ENC_TUNING_INFO::NV_ENC_TUNING_INFO_LOW_LATENCY);
	encoder->CreateEncoder(&initializeParams);
#endif

	if (showPreview)
	{
		windowName = "Stream";

		cv::namedWindow(windowName, cv::WINDOW_NORMAL || cv::WINDOW_OPENGL);
	}
}

cv::Mat ScreenCapturerData::processFrame(cv::Mat& frame)
{
	int32_t width = encoder->GetEncodeWidth();
	int32_t height = encoder->GetEncodeHeight();
	uint8_t* frameData = nullptr;
	std::vector<NvEncOutputFrame> frames;
	cv::Mat result;

	cv::resize(frame, frame, cv::Size(width, height));

	cv::cvtColor(frame, frame, cv::COLOR_BGR2YUV_IYUV);

	const NvEncInputFrame* encoderInputFrame = encoder->GetNextInputFrame();
	NvEncoderCuda::CopyToDeviceFrame
	(
		context, frame.data, 0, (CUdeviceptr)encoderInputFrame->inputPtr,
		static_cast<int>(encoderInputFrame->pitch),
		width,
		height,
		CU_MEMORYTYPE_HOST,
		encoderInputFrame->bufferFormat,
		encoderInputFrame->chromaOffsets,
		encoderInputFrame->numChromaPlanes
	);

	while (frames.empty())
	{
		encoder->EncodeFrame(frames);
	}

	NvEncOutputFrame& lastFrame = frames.back();

	decoder->Decode(lastFrame.frame.data(), lastFrame.frame.size(), CUvideopacketflags::CUVID_PKT_ENDOFPICTURE);

	while (!frameData)
	{
		frameData = decoder->GetFrame();
	}

	cv::Mat decoded(height * 3 / 2, width, CV_8UC1, frameData);
	
	cv::cvtColor(decoded, result, cv::COLOR_YUV2BGR_NV12);

	if (windowName.size())
	{
		cv::imshow(windowName, result);
	}

	return result;
}

ScreenCapturerData::~ScreenCapturerData()
{
	if (windowName.size())
	{
		cv::destroyWindow(windowName);
	}
}
