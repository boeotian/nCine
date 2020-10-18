#include "TextureData.h"
#include "ITextureLoader.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureData::TextureData(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
    : texLoader_(ITextureLoader::createFromMemory(bufferName, bufferPtr, bufferSize)), isValid_(texLoader_->hasLoaded())
{
}

TextureData::TextureData(const char *filename)
    : texLoader_(ITextureLoader::createFromFile(filename)), isValid_(texLoader_->hasLoaded())
{
}

TextureData::~TextureData()
{
	// Defined to solve deletion of incomplete type pointer (forward declared class)
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const char *TextureData::filename() const
{
	return texLoader_->filename();
}

}
