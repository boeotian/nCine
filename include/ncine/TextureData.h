#ifndef CLASS_NCINE_TEXTUREDATA
#define CLASS_NCINE_TEXTUREDATA

#include "common_defines.h"
#include <nctl/UniquePtr.h>

namespace ncine {

class ITextureLoader;

/// A texture loader wrapper class
/*! The class offers a way to users to load texture data and check
 * if it is valid before trying to create an actual Texture class
 * \note The object can be reused to create multiple textures */
class DLL_PUBLIC TextureData
{
  public:
	TextureData(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	explicit TextureData(const char *filename);
	~TextureData();

	/// Returns true if the texture data is valid
	inline bool isValid() const { return isValid_; }
	/// Returns the name of the buffer or file used to load data from
	const char *filename() const;

  private:
	/// A smart pointer to the texture loader object
	nctl::UniquePtr<ITextureLoader> texLoader_;
	/// A flag indicating if the loading process has been successful
	bool isValid_;

	/// Deleted copy constructor
	TextureData(const TextureData &) = delete;
	/// Deleted assignment operator
	TextureData &operator=(const TextureData &) = delete;

	friend class Texture;
	friend class FontData;
};

}

#endif
