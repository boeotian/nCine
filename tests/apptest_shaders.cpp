#include "apptest_shaders.h"
#include <ncine/Application.h>
#include <ncine/Shader.h>
#include <ncine/ShaderState.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/MeshSprite.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *Texture1File = "texture1_ETC2.ktx";
const char *Texture2File = "texture2_ETC2.ktx";
const char *Texture3File = "texture3_ETC2.ktx";
const char *Texture4File = "texture4_ETC2.ktx";
#else
const char *Texture1File = "texture1.png";
const char *Texture2File = "texture2.png";
const char *Texture3File = "texture3.png";
const char *Texture4File = "texture4.png";
#endif

bool paused = false;

const int NumTexelPoints = 3;
const nc::Vector2f TexelPoints[] = {
	{4.0f, 2.0f}, {124.0f, 2.0f}, {64.0f, 122.0f}
};

char const * const sprite_vs = R"(
uniform mat4 projection;
uniform mat4 modelView;
layout (std140) uniform SpriteBlock
{
	//mat4 modelView;
	vec4 color;
	vec4 texRect;
	vec2 spriteSize;
};
out vec2 vTexCoords;
out vec4 vColor;
void main()
{
	vec2 aPosition = vec2(0.5 - float(gl_VertexID >> 1), -0.5 + float(gl_VertexID % 2));
	vec2 aTexCoords = vec2(1.0 - float(gl_VertexID >> 1), 1.0 - float(gl_VertexID % 2));
	vec4 position = vec4(aPosition.x * spriteSize.x, aPosition.y * spriteSize.y, 0.0, 1.0);
	gl_Position = projection * modelView * position;
	vTexCoords = vec2(aTexCoords.x * texRect.x + texRect.y, aTexCoords.y * texRect.z + texRect.w);
	vColor = color;
}
)";

char const * const sprite_fs = R"(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D uTexture;
in vec2 vTexCoords;
in vec4 vColor;
out vec4 fragColor;
void main()
{
	const vec2 onePixel = vec2(1.0 / 128.0, 1.0 / 128.0);
	vec4 color;

	color.rgb = vec3(0.5);
	float alpha = texture(uTexture, vTexCoords).a;
	color -= texture2D(uTexture, vTexCoords - onePixel) * 5.0;
	color += texture2D(uTexture, vTexCoords + onePixel) * 5.0;
	color.rgb = vec3((color.r + color.g + color.b) / 3.0);

	fragColor = vec4(color.rgb, alpha);
}
)";

char const * const meshsprite_vs = R"(
uniform mat4 projection;
uniform mat4 modelView;
layout (std140) uniform MeshSpriteBlock
{
	//mat4 modelView;
	vec4 color;
	vec4 texRect;
	vec2 spriteSize;
};
in vec2 aPosition;
in vec2 aTexCoords;
out vec2 vTexCoords;
out vec4 vColor;
void main()
{
	vec4 position = vec4(aPosition.x * spriteSize.x, aPosition.y * spriteSize.y, 0.0, 1.0);
	gl_Position = projection * modelView * position;
	vTexCoords = vec2(aTexCoords.x * texRect.x + texRect.y, aTexCoords.y * texRect.z + texRect.w);
	vColor = color;
}
)";

char const * const meshsprite_fs = R"(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D uTexture;
in vec2 vTexCoords;
in vec4 vColor;
out vec4 fragColor;
void main()
{
	vec4 color = texture(uTexture, vTexCoords);
	float lum = dot(vec3(0.30, 0.59, 0.11), color.xyz);
	fragColor = vec4(vec3(lum) * vColor.rgb, color.a);
}
)";

}

// TODO: Define a custom modelview and projection uniform name (and optionally a block) or retrieve it from somewhere
// TODO: Test with batching enabled and with all drawable node types
// TODO: Reload a shader if compilation fails

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	angle_ = 0.0f;

	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	const float width = nc::theApplication().width();
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, textures_[i % NumTextures].get(), width * 0.15f + width * 0.1f * i, 300.0f));
		sprites_.back()->setScale(0.5f);
	}

	nc::Sprite &sprite = *sprites_.back();

	shader_ = nctl::makeUnique<nc::Shader>(nc::Shader::LoadMode::STRING, sprite_vs, sprite_fs);
	FATAL_ASSERT(shader_->isLinked());
	{
		nc::ShaderState &ss = sprite.shaderState();
		ss.setShader(shader_.get());
		ss.setUniformInt(nullptr, "uTexture", 0);

		const nc::Vector2i texSize = sprite.texture()->size();
		const float texScaleX = sprite.texRect().w / float(texSize.x);
		const float texBiasX = sprite.texRect().x / float(texSize.x);
		const float texScaleY = sprite.texRect().h / float(texSize.y);
		const float texBiasY = sprite.texRect().y / float(texSize.y);


		ss.setUniformFloat("SpriteBlock", "color", 0.0f, 1.0f, 1.0f, 1.0f);
		ss.setUniformFloat("SpriteBlock", "texRect", texScaleX, texBiasX, texScaleY, texBiasY);
		ss.setUniformFloat("SpriteBlock", "spriteSize", sprite.absWidth(), sprite.absHeight());
	}


	meshSprite_ = nctl::makeUnique<nc::MeshSprite>(&rootNode, textures_[0].get(), width * 0.5f, nc::theApplication().height() * 0.8f);
	meshSprite_->createVerticesFromTexels(NumTexelPoints, TexelPoints);

	shader2_ = nctl::makeUnique<nc::Shader>(nc::Shader::LoadMode::STRING, meshsprite_vs, meshsprite_fs);
	FATAL_ASSERT(shader2_->isLinked());
	{
		nc::ShaderState &ss = meshSprite_->shaderState();
		ss.setShader(shader2_.get());
		ss.setUniformInt(nullptr, "uTexture", 0);

		const nc::Vector2i texSize = sprite.texture()->size();
		const float texScaleX = sprite.texRect().w / float(texSize.x);
		const float texBiasX = sprite.texRect().x / float(texSize.x);
		const float texScaleY = sprite.texRect().h / float(texSize.y);
		const float texBiasY = sprite.texRect().y / float(texSize.y);


		ss.setUniformFloat("MeshSpriteBlock", "color", 1.0f, 1.0f, 1.0f, 1.0f);
		ss.setUniformFloat("MeshSpriteBlock", "texRect", texScaleX, texBiasX, texScaleY, texBiasY);
		ss.setUniformFloat("MeshSpriteBlock", "spriteSize", sprite.absWidth(), sprite.absHeight());

		// TODO: How to link the semantics to the name? ("aPosition contains vertex positions")
		ss.setAttribute("aPosition", 4 * sizeof(float), 0);
		ss.setAttribute("aTexCoords", 4 * sizeof(float), 2 * sizeof(float));
	}
}

void MyEventHandler::onFrameStart()
{
	const float height = nc::theApplication().height();

	if (paused == false)
	{
		angle_ += 1.0f * nc::theApplication().interval();
		if (angle_ > 360.0f)
			angle_ -= 360.0f;
	}

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_[i]->y = height * 0.3f + fabsf(sinf(angle_ + 5.0f * i)) * (height * (0.25f + 0.02f * i));
		sprites_[i]->setRotation(angle_ * 20.0f);
	}

	meshSprite_->setRotation(angle_ * 20.0f);
	meshSprite_->shaderState().setUniformFloat("MeshSpriteBlock", "color", sinf(angle_), 1.0f - sinf(angle_), 1.0f, 1.0f);
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	paused = true;
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	paused = false;
}
#endif

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		paused = true;
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		paused = false;
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
	else if (event.sym == nc::KeySym::R)
	{
		sprites_.back()->shaderState().setShader(nullptr);
		meshSprite_->shaderState().setShader(nullptr);
	}
}
