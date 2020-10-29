#ifndef CLASS_NCINE_AUDIOBUFFER
#define CLASS_NCINE_AUDIOBUFFER

#include "Object.h"

namespace ncine {

class AudioData;
class IAudioLoader;

/// A class representing an OpenAL buffer
/*! It inherits from `Object` because a buffer can be
 *  shared by more than one `AudioBufferPlayer` object. */
class DLL_PUBLIC AudioBuffer : public Object
{
  public:
	/// A constructor creating a buffer from memory
	AudioBuffer(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	/// A constructor creating a buffer from a file
	explicit AudioBuffer(const char *filename);
	/// Constructor creating a buffer from an audio data class
	explicit AudioBuffer(AudioData &audioData);
	~AudioBuffer() override;

	/// Returns the OpenAL buffer id
	inline unsigned int bufferId() const { return bufferId_; }
	/// Returns the number of audio channels
	inline int numChannels() const { return numChannels_; }
	/// Returns the samples frequency
	inline int frequency() const { return frequency_; }
	/// Returns the size of the buffer in bytes
	inline unsigned long bufferSize() const { return bufferSize_; }

	inline static ObjectType sType() { return ObjectType::AUDIOBUFFER; }

  private:
	/// The OpenAL buffer id
	unsigned int bufferId_;
	/// Number of channels
	int numChannels_;
	/// Samples frequency
	int frequency_;
	/// Buffer size in bytes
	unsigned long bufferSize_;

	/// Loads audio samples based on information from the audio loader and reader
	void load(IAudioLoader &audioLoader);

	/// Deleted copy constructor
	AudioBuffer(const AudioBuffer &) = delete;
	/// Deleted assignment operator
	AudioBuffer &operator=(const AudioBuffer &) = delete;
};

}

#endif
