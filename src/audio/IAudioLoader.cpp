#include "IAudioLoader.h"
#include "MemoryFile.h"
#include "AudioLoaderWav.h"
#ifdef WITH_VORBIS
	#include "AudioLoaderOgg.h"
#endif
#include "FileSystem.h"

#ifdef __ANDROID__
	#include "AssetFile.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IAudioLoader::IAudioLoader(nctl::UniquePtr<IFile> fileHandle)
    : hasLoaded_(false), fileHandle_(nctl::move(fileHandle)), bytesPerSample_(0),
      numChannels_(0), frequency_(0), numSamples_(0L), duration_(0.0f)
{
	// Warning: Cannot call a virtual `init()` here, in the base constructor

	// When reusing `AudioData` each reader needs a new IFile so constructor args are saved
#ifdef __ANDROID__
	if (fileHandle_->type() == IFile::FileType::ASSET)
		constructionInfo_.name.format("%s%s", AssetFile::Prefix, fileHandle_->filename());
	else
#else
		constructionInfo_.name = fileHandle_->filename();
#endif
	constructionInfo_.bufferPtr = nullptr;
	if (fileHandle_->type() == IFile::FileType::MEMORY)
	{
		const MemoryFile &memoryFile = static_cast<const MemoryFile &>(*fileHandle_.get());
		constructionInfo_.bufferPtr = memoryFile.bufferPtr();
		constructionInfo_.bufferSize = memoryFile.size();
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const char *IAudioLoader::filename() const
{
	// An `InvalidAudioLoader` has no associated file handle and no name
	return constructionInfo_.name.data();
}

nctl::UniquePtr<IAudioLoader> IAudioLoader::createFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	LOGI_X("Loading memory file: \"%s\" (0x%lx, %lu bytes)", bufferName, bufferPtr, bufferSize);
	return createLoader(nctl::move(IFile::createFromMemory(bufferName, bufferPtr, bufferSize)), bufferName);
}

nctl::UniquePtr<IAudioLoader> IAudioLoader::createFromFile(const char *filename)
{
	LOGI_X("Loading file: \"%s\"", filename);
	// Creating a handle from IFile static method to detect assets file
	return createLoader(nctl::move(IFile::createFileHandle(filename)), filename);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

nctl::UniquePtr<IAudioLoader> IAudioLoader::createLoader(nctl::UniquePtr<IFile> fileHandle, const char *filename)
{
	fileHandle->setExitOnFailToOpen(false);

	if (fs::hasExtension(filename, "wav"))
		return nctl::makeUnique<AudioLoaderWav>(nctl::move(fileHandle));
#ifdef WITH_VORBIS
	else if (fs::hasExtension(filename, "ogg"))
		return nctl::makeUnique<AudioLoaderOgg>(nctl::move(fileHandle));
#endif
	else
	{
		LOGF_X("Extension unknown: \"%s\"", fs::extension(filename));
		fileHandle.reset(nullptr);
		return nctl::makeUnique<InvalidAudioLoader>(nctl::move(fileHandle));
	}
}

}
