#ifndef CLASS_NCINE_TEXTURE
#define CLASS_NCINE_TEXTURE

#include "Object.h"
#include "Rect.h"

namespace ncine {

class ITextureLoader;
class GLTexture;

/// Texture class
class DLL_PUBLIC Texture : public Object
{
  public:
	/// Texture formats
	enum class Format
	{
		R8,
		RG8,
		RGB8,
		RGBA8
	};

	/// Texture filtering modes
	enum class Filtering
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};

	/// Texture wrap modes
	enum class Wrap
	{
		CLAMP_TO_EDGE,
		MIRRORED_REPEAT,
		REPEAT
	};

	/// Creates an OpenGL texture name
	Texture();

	/// Constructs an empty texture, of a specified format, MIP levels, and size, to be filled later
	Texture(const char *name, Format format, int mipMapCount, int width, int height);
	/// Constructs an empty texture, of a specified format, MIP levels, and size using a vector, to be filled later
	Texture(const char *name, Format format, int mipMapCount, Vector2i size);
	/// Constructs an empty texture, of a specified format and size, to be filled later
	Texture(const char *name, Format format, int width, int height);
	/// Constructs an empty texture, of a specified format and size using a vector, to be filled later
	Texture(const char *name, Format format, Vector2i size);

	Texture(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	explicit Texture(const char *filename);

	~Texture() override;

	bool loadFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	bool loadFromFile(const char *filename);

	/// Loads all texture texels in raw format from a memory buffer
	bool loadFromTexels(const unsigned char *bufferPtr);
	/// Loads texels in raw format from a memory buffer to a texture sub-region in the first mip level
	bool loadFromTexels(const unsigned char *bufferPtr, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	/// Loads texels in raw format from a memory buffer to a texture sub-region with a rectangle in the first mip level
	bool loadFromTexels(const unsigned char *bufferPtr, Recti region);
	/// Loads texels in raw format from a memory buffer to a specific texture mip level and sub-region
	bool loadFromTexels(const unsigned char *bufferPtr, unsigned int level, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	/// Loads texels in raw format from a memory buffer to a specific texture mip level and sub-region with a rectangle
	bool loadFromTexels(const unsigned char *bufferPtr, unsigned int level, Recti region);

	/// Returns texture width
	inline int width() const { return width_; }
	/// Returns texture height
	inline int height() const { return height_; }
	/// Returns texture MIP map levels
	inline int mipMapLevels() const { return mipMapLevels_; }
	/// Returns texture size
	inline Vector2i size() const { return Vector2i(width_, height_); }
	/// Returns texture rectangle
	inline Recti rect() const { return Recti(0, 0, width_, height_); }

	/// Returns true if the texture holds compressed data
	inline bool isCompressed() const { return isCompressed_; }
	/// Returns the number of color channels
	inline unsigned int numChannels() const { return numChannels_; }
	/// Returns the amount of video memory needed to load the texture
	inline unsigned long dataSize() const { return dataSize_; }

	/// Returns the texture filtering for minification
	inline Filtering minFiltering() const { return minFiltering_; }
	/// Returns the texture filtering for magnification
	inline Filtering magFiltering() const { return magFiltering_; }
	/// Returns texture wrap for both `s` and `t` coordinates
	inline Wrap wrap() const { return wrapMode_; }
	/// Sets the texture filtering for minification
	void setMinFiltering(Filtering filter);
	/// Sets the texture filtering for magnification
	void setMagFiltering(Filtering filter);
	/// Sets texture wrap for both `s` and `t` coordinates
	void setWrap(Wrap wrapMode);

	/// Returns the user data opaque pointer for ImGui's ImTextureID
	void *imguiTexId();

	inline static ObjectType sType() { return ObjectType::TEXTURE; }

	// TODO:: Save as Png and WebP?

  private:
	nctl::UniquePtr<GLTexture> glTexture_;
	int width_;
	int height_;
	int mipMapLevels_;
	bool isCompressed_;
	unsigned int numChannels_;
	unsigned long dataSize_;

	Filtering minFiltering_;
	Filtering magFiltering_;
	Wrap wrapMode_;

	/// Deleted copy constructor
	Texture(const Texture &) = delete;
	/// Deleted assignment operator
	Texture &operator=(const Texture &) = delete;

	/// Initialize an empty texture by creating storage for it
	void initialize(const ITextureLoader &texLoader);
	/// Loads the data in a previously initialized texture
	void load(const ITextureLoader &texLoader);

	/// Sets the OpenGL object label for the texture
	void setGLTextureLabel(const char *filename);

	friend class Material;
};

}

#endif
