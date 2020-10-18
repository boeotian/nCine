#include "AudioData.h"
#include "IAudioLoader.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioData::AudioData(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
    : audioLoader_(IAudioLoader::createFromMemory(bufferName, bufferPtr, bufferSize)), isValid_(audioLoader_->hasLoaded())
{
}

AudioData::AudioData(const char *filename)
    : audioLoader_(IAudioLoader::createFromFile(filename)), isValid_(audioLoader_->hasLoaded())
{
}

AudioData::~AudioData()
{
	// Defined to solve deletion of incomplete type pointer (forward declared class)
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const char *AudioData::filename() const
{
	return audioLoader_->filename();
}

}
