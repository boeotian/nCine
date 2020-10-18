#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "common_macros.h"
#include "AudioBuffer.h"
#include "AudioData.h"
#include "IAudioLoader.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
    : Object(ObjectType::AUDIOBUFFER, bufferName),
      numChannels_(0), frequency_(0), bufferSize_(0)
{
	ZoneScoped;
	ZoneText(bufferName, strnlen(bufferName, nctl::String::MaxCStringLength));

	alGetError();
	alGenBuffers(1, &bufferId_);
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: %x", error);

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromMemory(bufferName, bufferPtr, bufferSize);
	load(*audioLoader.get());
}

AudioBuffer::AudioBuffer(const char *filename)
    : Object(ObjectType::AUDIOBUFFER, filename),
      numChannels_(0), frequency_(0), bufferSize_(0)
{
	ZoneScoped;
	ZoneText(filename, strnlen(filename, nctl::String::MaxCStringLength));

	alGetError();
	alGenBuffers(1, &bufferId_);
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: %x", error);

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromFile(filename);
	load(*audioLoader.get());
}

AudioBuffer::AudioBuffer(AudioData &audioData)
    : Object(ObjectType::AUDIOBUFFER, audioData.filename()),
      numChannels_(0), frequency_(0), bufferSize_(0)
{
	FATAL_ASSERT(audioData.isValid());

	ZoneScoped;
	ZoneText(audioData.filename(), strnlen(audioData.filename(), nctl::String::MaxCStringLength));

	alGetError();
	alGenBuffers(1, &bufferId_);
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: %x", error);

	load(*audioData.audioLoader_.get());
}

AudioBuffer::~AudioBuffer()
{
	alDeleteBuffers(1, &bufferId_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AudioBuffer::load(IAudioLoader &audioLoader)
{
	frequency_ = audioLoader.frequency();
	numChannels_ = audioLoader.numChannels();

	FATAL_ASSERT_MSG_X(numChannels_ == 1 || numChannels_ == 2, "Unsupported number of channels: %d", numChannels_);
	const ALenum format = (numChannels_ == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	// Buffer size calculated as samples * channels * 16bit
	bufferSize_ = audioLoader.bufferSize();
	nctl::UniquePtr<char[]> buffer = nctl::makeUnique<char[]>(bufferSize_);

	nctl::UniquePtr<IAudioReader> audioReader = audioLoader.createReader();
	audioReader->read(buffer.get(), bufferSize_);

	alGetError();
	// On iOS `alBufferDataStatic()` could be used instead
	alBufferData(bufferId_, format, buffer.get(), bufferSize_, frequency_);
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alBufferData failed: %x", error);
}

}
