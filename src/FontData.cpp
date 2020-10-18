#include "return_macros.h"
#include "FontData.h"
#include "FntParser.h"
#include "TextureData.h"
#include "ITextureLoader.h"
#include "FileSystem.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FontData::FontData(const char *fntBufferName, const char *fntBufferPtr, unsigned long int fntBufferSize, const char *texFilename)
    : isValid_(false),
      fntParser_(nctl::makeUnique<FntParser>(fntBufferName, fntBufferPtr, fntBufferSize)),
      texData_(nctl::makeUnique<TextureData>(texFilename))
{
	isValid_ = texData_->isValid() && checkFntInformation();
}

FontData::FontData(const char *fntBufferName, const char *fntBufferPtr, unsigned long int fntBufferSize,
                   const char *texBufferName, const unsigned char *texBufferPtr, unsigned long int texBufferSize)
    : isValid_(false),
      fntParser_(nctl::makeUnique<FntParser>(fntBufferName, fntBufferPtr, fntBufferSize)),
      texData_(nctl::makeUnique<TextureData>(texBufferName, texBufferPtr, texBufferSize))
{
	isValid_ = texData_->isValid() && checkFntInformation();
}

FontData::FontData(const char *fntFilename)
    : isValid_(false), fntParser_(nctl::makeUnique<FntParser>(fntFilename))
{
	nctl::String dirName = fs::dirName(fntFilename);
	nctl::String texFilename = fs::absoluteJoinPath(dirName, fntParser_->pageTag(0).file);
	texData_ = nctl::makeUnique<TextureData>(texFilename.data());

	isValid_ = texData_->isValid() && checkFntInformation();
}

FontData::FontData(const char *fntFilename, const char *texFilename)
    : isValid_(false),
      fntParser_(nctl::makeUnique<FntParser>(fntFilename)),
      texData_(nctl::makeUnique<TextureData>(texFilename))
{
	isValid_ = texData_->isValid() && checkFntInformation();
}

FontData::~FontData()
{
	// Defined to solve deletion of incomplete type pointer (forward declared class)
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const char *FontData::fntFilename() const
{
	return fntParser_->filename();
}

const char *FontData::texFilename() const
{
	return texData_->filename();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/*! \note The same checks are performed by `Font::checkFntInformation()` using a `Texture` object */
bool FontData::checkFntInformation() const
{
	const FntParser::InfoTag &infoTag = fntParser_->infoTag();
	RETURNF_ASSERT_MSG_X(infoTag.outline == 0, "Font outline is not supported");

	const FntParser::CommonTag &commonTag = fntParser_->commonTag();
	RETURNF_ASSERT_MSG_X(commonTag.pages == 1, "Multiple texture pages are not supported (pages: %d)", commonTag.pages);
	RETURNF_ASSERT_MSG(commonTag.packed == false, "Characters packed into each of the texture channels are not supported");

	const ITextureLoader &texLoader = *texData_->texLoader_;
	RETURNF_ASSERT_MSG_X(commonTag.scaleW == texLoader.width(), "Texture width is different than FNT scale width: %u instead of %u", texLoader.width(), commonTag.scaleW);
	RETURNF_ASSERT_MSG_X(commonTag.scaleH == texLoader.height(), "Texture height is different than FNT scale height: %u instead of %u", texLoader.height(), commonTag.scaleH);

	if (texLoader.bpp() == 1)
	{
		RETURNF_ASSERT_MSG(commonTag.alphaChnl == FntParser::ChannelData::GLYPH ||
		                   commonTag.alphaChnl == FntParser::ChannelData::OUTLINE ||
		                   commonTag.alphaChnl == FntParser::ChannelData::MISSING,
		                   "Texture has one channel only but it does not contain glyph data");
	}
	else if (texLoader.bpp() == 4)
	{
		RETURNF_ASSERT_MSG((commonTag.redChnl == FntParser::ChannelData::MISSING && commonTag.alphaChnl == FntParser::ChannelData::MISSING) ||
		                   commonTag.redChnl == FntParser::ChannelData::GLYPH || commonTag.alphaChnl == FntParser::ChannelData::GLYPH,
		                   "Texture has four channels but neither red nor alpha channel contain glyph data");
	}

	return true;
}

}
