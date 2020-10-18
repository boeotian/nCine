#ifndef CLASS_NCINE_FONTDATA
#define CLASS_NCINE_FONTDATA

#include "common_defines.h"
#include <nctl/UniquePtr.h>

namespace ncine {

class FntParser;
class TextureData;
class ITextureLoader;

/// A Fnt parser wrapper class
/*! The class offers a way to users to parse Fnt data, to load texture data
 * and check if they are valid before trying to create an actual Font class
 * \note The object can be reused to create multiple fonts */
class DLL_PUBLIC FontData
{
  public:
	FontData(const char *fntBufferName, const char *fntBufferPtr, unsigned long int fntBufferSize, const char *texFilename);
	FontData(const char *fntBufferName, const char *fntBufferPtr, unsigned long int fntBufferSize,
	         const char *texBufferName, const unsigned char *texBufferPtr, unsigned long int texBufferSize);

	explicit FontData(const char *fntFilename);
	FontData(const char *fntFilename, const char *texFilename);

	~FontData();

	/// Returns true if the Fnt data is valid
	inline bool isValid() const { return isValid_; }
	/// Returns the name of the buffer or file used to load Fnt data from
	const char *fntFilename() const;
	/// Returns the name of the buffer or file used to load Texture data from
	const char *texFilename() const;

  private:
	/// A flag indicating if the loading process has been successful
	bool isValid_;

	/// A smart pointer to the Fnt parser object
	nctl::UniquePtr<FntParser> fntParser_;
	/// A smart pointer to the TextureData object
	nctl::UniquePtr<TextureData> texData_;

	/// Checks if the parsed FNT information is valid when used with the texture
	bool checkFntInformation() const;

	/// Deleted copy constructor
	FontData(const FontData &) = delete;
	/// Deleted assignment operator
	FontData &operator=(const FontData &) = delete;

	friend class Font;
};

}

#endif
