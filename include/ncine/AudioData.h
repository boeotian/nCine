#ifndef CLASS_NCINE_AUDIODATA
#define CLASS_NCINE_AUDIODATA

#include "common_defines.h"
#include <nctl/UniquePtr.h>

namespace ncine {

class IAudioLoader;

/// An audio loader wrapper class
/*! The class offers a way to users to load audio data and check
 * if it is valid before trying to create an audio player
 * \note The object can be reused to create multiple players */
class DLL_PUBLIC AudioData
{
  public:
	AudioData(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	explicit AudioData(const char *filename);
	~AudioData();

	/// Returns true if the audio data is valid
	inline bool isValid() const { return isValid_; }
	/// Returns the name of the buffer or file used to load data from
	const char *filename() const;

  private:
	/// A smart pointer to the audio loader object
	nctl::UniquePtr<IAudioLoader> audioLoader_;
	/// A flag indicating if the loading process has been successful
	bool isValid_;

	/// Deleted copy constructor
	AudioData(const AudioData &) = delete;
	/// Deleted assignment operator
	AudioData &operator=(const AudioData &) = delete;

	friend class AudioBuffer;
	friend class AudioStream;
};

}

#endif
