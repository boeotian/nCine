#include <ncine/imgui.h>

#include <nctl/algorithms.h> // for clamping values
#include "apptest_loading.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/AudioBuffer.h>
#include <ncine/AudioBufferPlayer.h>
#include <ncine/AudioStreamPlayer.h>
#include <ncine/TextNode.h>
#include <ncine/IFile.h>
#include <ncine/Colorf.h>
#include "apptest_datapath.h"

#define DEFAULT_CONSTRUCTORS (0)
#define LOADING_FAILURES (1)

namespace {

char const * const TextureFiles[MyEventHandler::NumTextures] = { "texture1.png", "texture2.png", "texture3.png", "texture4.png" };
nctl::StaticArray<nctl::UniquePtr<uint8_t[]>, MyEventHandler::NumTextures> textureBuffers;
nctl::StaticArray<unsigned long int, MyEventHandler::NumTextures> textureBufferSizes;

char const * const SoundFiles[MyEventHandler::NumSounds] = { "explode.wav", "waterdrop.wav", "coins.wav", "c64.ogg", "chiptune_loop.ogg" };
nctl::StaticArray<nctl::UniquePtr<uint8_t[]>, MyEventHandler::NumSounds> soundBuffers;
nctl::StaticArray<unsigned long int, MyEventHandler::NumSounds> soundBufferSizes;

char const * const FontFiles[MyEventHandler::NumFonts] = { "DroidSans32_256.fnt", "NotoSans-Regular32_256.fnt", "NotoSerif-Regular32_256.fnt", "OpenSans-Regular32_256.fnt", "Roboto-Regular32_256.fnt" };
char const * const FontTexFiles[MyEventHandler::NumFonts] = { "DroidSans32_256.png", "NotoSans-Regular32_256.png", "NotoSerif-Regular32_256.png", "OpenSans-Regular32_256.png", "Roboto-Regular32_256.png" };
nctl::StaticArray<nctl::UniquePtr<uint8_t[]>, MyEventHandler::NumFonts> fontBuffers;
nctl::StaticArray<unsigned long int, MyEventHandler::NumFonts> fontBufferSizes;

const unsigned long int randomBufferLength = 1024;
uint8_t randomBuffer[randomBufferLength];

nctl::String auxString(256);
int selectedTexObject = -1;
int selectedTexFile = -1;
int selectedTexBuffer = -1;
nc::Colorf texelsColor;
nc::Recti texelsRegion;

int selectedSoundFile = -1;
int selectedSoundBuffer = -1;
int sineWaveFrequency = 1000;
float sineWaveDuration = 1.0f;

int selectedFontFile = -1;
int selectedFontBuffer = -1;

const char *audioPlayerStateToString(nc::IAudioPlayer::PlayerState state)
{
	switch (state)
	{
		case nc::IAudioPlayer::PlayerState::INITIAL: return "Initial";
		case nc::IAudioPlayer::PlayerState::PLAYING: return "Playing";
		case nc::IAudioPlayer::PlayerState::PAUSED: return "Paused";
		case nc::IAudioPlayer::PlayerState::STOPPED: return "Stopped";
	}

	return "Unknown";
}

}

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
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	for (unsigned int i = 0; i < NumTextures; i++)
	{
		nctl::UniquePtr<nc::IFile> textureFile = nc::IFile::createFileHandle(prefixDataPath("textures", TextureFiles[i]).data());
		textureFile->open(nc::IFile::OpenMode::READ);
		textureBufferSizes.pushBack(textureFile->size());
		textureBuffers.pushBack(nctl::makeUnique<uint8_t[]>(textureBufferSizes[i]));
		textureFile->read(textureBuffers[i].get(), textureBufferSizes[i]);
		textureFile->close();
	}

	for (unsigned int i = 0; i < NumSounds; i++)
	{
		nctl::UniquePtr<nc::IFile> soundFile = nc::IFile::createFileHandle(prefixDataPath("sounds", SoundFiles[i]).data());
		soundFile->open(nc::IFile::OpenMode::READ);
		soundBufferSizes.pushBack(soundFile->size());
		soundBuffers.pushBack(nctl::makeUnique<uint8_t[]>(soundBufferSizes[i]));
		soundFile->read(soundBuffers[i].get(), soundBufferSizes[i]);
		soundFile->close();
	}

	for (unsigned int i = 0; i < NumFonts; i++)
	{
		nctl::UniquePtr<nc::IFile> fontFile = nc::IFile::createFileHandle(prefixDataPath("fonts", FontFiles[i]).data());
		fontFile->open(nc::IFile::OpenMode::READ);
		fontBufferSizes.pushBack(fontFile->size());
		fontBuffers.pushBack(nctl::makeUnique<uint8_t[]>(fontBufferSizes[i]));
		fontFile->read(fontBuffers[i].get(), fontBufferSizes[i]);
		fontFile->close();
	}

#if DEFAULT_CONSTRUCTORS
	for (unsigned int i = 0; i < NumTextures; i++)
		textures_.pushBack(nctl::makeUnique<nc::Texture>());
	audioBuffer_ = nctl::makeUnique<nc::AudioBuffer>();
	streamPlayer_ = nctl::makeUnique<nc::AudioStreamPlayer>();
	font_ = nctl::makeUnique<nc::Font>();
#else
	for (unsigned int i = 0; i < NumTextures; i++)
		textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", TextureFiles[i])).data()));
	audioBuffer_ = nctl::makeUnique<nc::AudioBuffer>((prefixDataPath("sounds", SoundFiles[0])).data());
	streamPlayer_ = nctl::makeUnique<nc::AudioStreamPlayer>((prefixDataPath("sounds", SoundFiles[3])).data());
	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFiles[0])).data());
#endif

#if LOADING_FAILURES
	// Loading from non-existent files
	for (unsigned int i = 0; i < NumTextures; i++)
		textures_[i]->loadFromFile("NonExistent.png");
	audioBuffer_->loadFromFile("NonExistent.wav");
	streamPlayer_->loadFromFile("NonExistent.ogg");
	font_->loadFromFile("NonExistent.fnt");

	// Loading from non-initiliazed memory buffers
	for (unsigned int i = 0; i < NumTextures; i++)
		textures_[i]->loadFromMemory("NonExistent.png", randomBuffer, randomBufferLength);
	audioBuffer_->loadFromMemory("NonExistent.wav", randomBuffer, randomBufferLength);
	streamPlayer_->loadFromMemory("NonExistent.ogg", randomBuffer, randomBufferLength);
	font_->loadFromMemory("NonExistent.fnt", randomBuffer, randomBufferLength, "NonExistent.png");
#endif

	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		nc::Texture *texture = textures_[i % NumTextures].get();
		const nc::Vector2f position(width / (NumSprites + 3) * (i + 2), height * 0.5f);
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, texture, position));
	}

	audioPlayer_ = nctl::makeUnique<nc::AudioBufferPlayer>(audioBuffer_.get());
	textNode_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
	textNode_->setPosition(width * 0.5f, height * 0.75f);
	textNode_->setString("apptest_loading");
}

void MyEventHandler::onFrameStart()
{
	ImGui::Begin("apptest_loading");
	if (ImGui::TreeNode("Textures"))
	{
		for (int i = 0; i < NumTextures; i++)
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (i == selectedTexObject)
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			ImGui::TreeNodeEx(textures_[i].get(), nodeFlags, "Texture #%u", i);
			if (ImGui::IsItemClicked())
			{
				selectedTexObject = i;
				texelsRegion.set(0, 0, textures_[i]->width(), textures_[i]->height());
			}
		}

		if (selectedTexObject >= 0 && selectedTexObject < NumTextures)
		{
			bool textureHasChanged = false;
			nc::Texture &tex = *textures_[selectedTexObject];
			ImGui::Text("Name: \"%s\"", tex.name().data());
			ImGui::Text("Size: %d x %d, Channels: %u", tex.width(), tex.height(), tex.numChannels());

			if (ImGui::TreeNode("Load from File"))
			{
				for (int i = 0; i < NumTextures; i++)
				{
					ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					if (i == selectedTexFile)
						nodeFlags |= ImGuiTreeNodeFlags_Selected;
					ImGui::TreeNodeEx(TextureFiles[i], nodeFlags);
					if (ImGui::IsItemClicked())
						selectedTexFile = i;
				}
				if (ImGui::Button("Load##File") && selectedTexFile >= 0 && selectedTexFile < NumTextures)
				{
					const bool hasLoaded = tex.loadFromFile((prefixDataPath("textures", TextureFiles[selectedTexFile])).data());
					if (hasLoaded == false)
						LOGW_X("Cannot load from file \"%s\"", TextureFiles[selectedTexFile]);
					textureHasChanged = hasLoaded;
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Load from Memory"))
			{
				for (int i = 0; i < NumTextures; i++)
				{
					ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					if (i == selectedTexBuffer)
						nodeFlags |= ImGuiTreeNodeFlags_Selected;
					ImGui::TreeNodeEx(TextureFiles[i], nodeFlags);
					if (ImGui::IsItemClicked())
						selectedTexBuffer = i;
				}
				if (ImGui::Button("Load##Memory") && selectedTexBuffer >= 0 && selectedTexBuffer < NumTextures)
				{
					const bool hasLoaded = tex.loadFromMemory(TextureFiles[selectedTexBuffer],
					                                                   textureBuffers[selectedTexBuffer].get(), textureBufferSizes[selectedTexBuffer]);
					if (hasLoaded == false)
						LOGW_X("Cannot load from memory \"%s\"", TextureFiles[selectedTexBuffer]);
					else
					{
						auxString.format("Memory file \"%s\"", TextureFiles[selectedTexBuffer]);
						tex.setName(auxString);
					}
					textureHasChanged = hasLoaded;
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Load from Texels"))
			{
				ImGui::DragInt2("Offset", &texelsRegion.x);
				ImGui::DragInt2("Size", &texelsRegion.w);

				texelsRegion.x = nctl::clamp(texelsRegion.x, 0, tex.width());
				texelsRegion.y = nctl::clamp(texelsRegion.y, 0, tex.height());
				texelsRegion.w = nctl::clamp(texelsRegion.w, 0, tex.width() - texelsRegion.x);
				texelsRegion.h = nctl::clamp(texelsRegion.h, 0, tex.height() - texelsRegion.y);

				ImGui::ColorEdit4("Color", texelsColor.data());
				if (ImGui::Button("Load##Texels"))
				{
					const int w = tex.width();
					const int h = tex.height();
					const uint32_t a = static_cast<uint8_t>(texelsColor.a() * 255.0f) << 24;
					const uint32_t b = static_cast<uint8_t>(texelsColor.b() * 255.0f) << 16;
					const uint32_t g = static_cast<uint8_t>(texelsColor.g() * 255.0f) << 8;
					const uint32_t r = static_cast<uint8_t>(texelsColor.r() * 255.0f);
					const uint32_t color = a + b + g + r;
					uint32_t pixels[w * h];
					for (int i = 0; i < w * h; i++)
						pixels[i] = color;

					const bool hasLoaded = tex.loadFromTexels(reinterpret_cast<uint8_t*>(&pixels), texelsRegion);
					if (hasLoaded == false)
						LOGW("Cannot load from texels");
					else
					{
						auxString.format("Raw Texels (0x%x)", color);
						tex.setName(auxString.data());
					}

					// When loading from texels the format and the size does not change and
					// the `textureHasChanged` event method does not need to be called
				}
				ImGui::TreePop();
			}

			if (textureHasChanged)
			{
				texelsRegion.set(0, 0, tex.width(), tex.height());
				for (unsigned int i = 0; i < NumSprites; i++)
				{
					if (sprites_[i]->texture() == &tex)
						sprites_[i]->textureHasChanged();
				}
			}
		}
		else
			ImGui::Text("Select a texture from the list");

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Audio"))
	{
		ImGui::Text("Buffer Name: \"%s\"", audioBuffer_->name().data());
		ImGui::Text("Frequency: %d, Channels: %d, Size: %lu", audioBuffer_->frequency(), audioBuffer_->numChannels(), audioBuffer_->bufferSize());
		ImGui::Separator();
		ImGui::Text("Stream Name: \"%s\"", streamPlayer_->name().data());
		ImGui::Text("Frequency: %d, Channels: %d, Size: %lu", streamPlayer_->frequency(), streamPlayer_->numChannels(), streamPlayer_->bufferSize());

		if (ImGui::TreeNode("Load from File"))
		{
			for (int i = 0; i < NumSounds; i++)
			{
				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				if (i == selectedSoundFile)
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				ImGui::TreeNodeEx(SoundFiles[i], nodeFlags);
				if (ImGui::IsItemClicked())
					selectedSoundFile = i;
			}
			if (ImGui::Button("Load Buffer##File") && selectedSoundFile >= 0 && selectedSoundFile < NumSounds)
			{
				const bool hasLoaded = audioBuffer_->loadFromFile((prefixDataPath("sounds", SoundFiles[selectedSoundFile])).data());
				if (hasLoaded == false)
					LOGW_X("Cannot load from file \"%s\"", SoundFiles[selectedSoundFile]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Load Stream##File") && selectedSoundFile >= 0 && selectedSoundFile < NumSounds)
			{
				const bool hasLoaded = streamPlayer_->loadFromFile((prefixDataPath("sounds", SoundFiles[selectedSoundFile])).data());
				if (hasLoaded == false)
					LOGW_X("Cannot load from file \"%s\"", SoundFiles[selectedSoundFile]);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Load from Memory"))
		{
			for (int i = 0; i < NumSounds; i++)
			{
				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				if (i == selectedSoundBuffer)
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				ImGui::TreeNodeEx(SoundFiles[i], nodeFlags);
				if (ImGui::IsItemClicked())
					selectedSoundBuffer = i;
			}
			if (ImGui::Button("Load Buffer##Memory") && selectedSoundBuffer >= 0 && selectedSoundBuffer < NumSounds)
			{
				const bool hasLoaded = audioBuffer_->loadFromMemory(SoundFiles[selectedSoundBuffer],
				                                                    soundBuffers[selectedSoundBuffer].get(), soundBufferSizes[selectedSoundBuffer]);
				if (hasLoaded == false)
					LOGW_X("Cannot load from memory \"%s\"", SoundFiles[selectedSoundBuffer]);
				else
				{
					auxString.format("Memory file \"%s\"", SoundFiles[selectedSoundBuffer]);
					audioBuffer_->setName(auxString);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load Stream##Memory") && selectedSoundBuffer >= 0 && selectedSoundBuffer < NumSounds)
			{
				const bool hasLoaded = streamPlayer_->loadFromMemory(SoundFiles[selectedSoundBuffer],
				                                                     soundBuffers[selectedSoundBuffer].get(), soundBufferSizes[selectedSoundBuffer]);
				if (hasLoaded == false)
					LOGW_X("Cannot load from memory \"%s\"", SoundFiles[selectedSoundBuffer]);
				else
				{
					auxString.format("Memory file \"%s\"", SoundFiles[selectedSoundBuffer]);
					streamPlayer_->setName(auxString);
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Load from Samples"))
		{
			ImGui::SliderInt("Frequency", &sineWaveFrequency, 100, 10000);
			sineWaveFrequency = nctl::clamp(sineWaveFrequency, 100, 10000);

			ImGui::SliderFloat("Duration", &sineWaveDuration, 0.1f, 5.0f);
			sineWaveDuration = nctl::clamp(sineWaveDuration, 0.1f, 5.0f);

			if (ImGui::Button("Load##Samples"))
			{
				bool hasLoaded = false;

				const unsigned int audioSamplesSize = static_cast<unsigned int>(audioBuffer_->numChannels() * audioBuffer_->frequency() * sineWaveDuration);
				if (audioBuffer_->bytesPerSample() == 1)
				{
					int8_t audioSamples[audioSamplesSize];
					for (unsigned int i = 0; i < audioSamplesSize; i++)
						audioSamples[i] = static_cast<int8_t>(sinf(2.0f * nc::Pi * i * sineWaveFrequency / audioBuffer_->frequency()) * 128);
					hasLoaded = audioBuffer_->loadFromSamples((unsigned char*)(&audioSamples), audioSamplesSize * sizeof(int8_t));
				}
				else
				{
					int16_t audioSamples[audioSamplesSize];
					for (unsigned int i = 0; i < audioSamplesSize; i++)
						audioSamples[i] = static_cast<int16_t>(sinf(2.0f * nc::Pi * i * sineWaveFrequency / audioBuffer_->frequency()) * 32767);
					hasLoaded = audioBuffer_->loadFromSamples((unsigned char*)(&audioSamples), audioSamplesSize * sizeof(int16_t));
				}
				if (hasLoaded == false)
					LOGW("Cannot load from samples");
				else
				{
					auxString.format("Raw Samples (%dHz for %.2fs)", sineWaveFrequency, sineWaveDuration);
					audioBuffer_->setName(auxString.data());
				}
			}

			ImGui::TreePop();
		}

		ImGui::Text("Buffer player state: %s", audioPlayerStateToString(audioPlayer_->state()));
		if (ImGui::Button("Play##Buffer"))
			audioPlayer_->play();
		ImGui::SameLine();
		if (ImGui::Button("Pause##Buffer"))
			audioPlayer_->pause();
		ImGui::SameLine();
		if (ImGui::Button("Stop##Buffer"))
			audioPlayer_->stop();

		if (audioPlayer_->state() != nc::IAudioPlayer::PlayerState::INITIAL)
		{
			int sampleOffset = audioPlayer_->sampleOffset();
			if (ImGui::SliderInt("Seek##Buffer", &sampleOffset, 0, audioPlayer_->numSamples()))
			{
				sampleOffset = nctl::clamp(sampleOffset, 0, static_cast<int>(audioPlayer_->numSamples()));
				audioPlayer_->setSampleOffset(sampleOffset);
			}
		}

		ImGui::Separator();

		ImGui::Text("Stream player state: %s", audioPlayerStateToString(streamPlayer_->state()));
		if (ImGui::Button("Play##Stream"))
			streamPlayer_->play();
		ImGui::SameLine();
		if (ImGui::Button("Pause##Stream"))
			streamPlayer_->pause();
		ImGui::SameLine();
		if (ImGui::Button("Stop##Stream"))
			streamPlayer_->stop();

		if (streamPlayer_->state() != nc::IAudioPlayer::PlayerState::INITIAL)
		{
			int sampleOffset = streamPlayer_->sampleOffset();
			if (ImGui::SliderInt("Seek##Stream", &sampleOffset, 0, streamPlayer_->numStreamSamples()))
			{
				sampleOffset = nctl::clamp(sampleOffset, 0, static_cast<int>(streamPlayer_->numStreamSamples()));
				streamPlayer_->setSampleOffset(sampleOffset);
			}
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Font"))
	{
		ImGui::Text("Name: \"%s\"", font_->name().data());
		ImGui::Text("Glyphs: %u, Kerning pairs: %u", font_->numGlyphs(), font_->numKernings());
		ImGui::Text("Line height: %u, Base: %u", font_->lineHeight(), font_->base());

		bool fontHasChanged = false;
		if (ImGui::TreeNode("Load from File"))
		{
			for (int i = 0; i < NumFonts; i++)
			{
				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				if (i == selectedFontFile)
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				ImGui::TreeNodeEx(FontFiles[i], nodeFlags);
				if (ImGui::IsItemClicked())
					selectedFontFile = i;
			}
			if (ImGui::Button("Load##File") && selectedFontFile >= 0 && selectedFontFile < NumFonts)
			{
				const bool hasLoaded = font_->loadFromFile((prefixDataPath("fonts", FontFiles[selectedFontFile])).data());
				if (hasLoaded == false)
					LOGW_X("Cannot load from file \"%s\"", FontFiles[selectedFontFile]);
				fontHasChanged = hasLoaded;
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Load from Memory"))
		{
			for (int i = 0; i < NumFonts; i++)
			{
				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				if (i == selectedFontBuffer)
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				ImGui::TreeNodeEx(FontFiles[i], nodeFlags);
				if (ImGui::IsItemClicked())
					selectedFontBuffer = i;
			}
			if (ImGui::Button("Load##Memory") && selectedFontBuffer >= 0 && selectedFontBuffer < NumFonts)
			{
				const bool hasLoaded = font_->loadFromMemory(FontFiles[selectedFontBuffer], fontBuffers[selectedFontBuffer].get(), fontBufferSizes[selectedFontBuffer],
				                                             (prefixDataPath("fonts", FontTexFiles[selectedFontBuffer])).data());
				if (hasLoaded == false)
					LOGW_X("Cannot load from memory \"%s\"", FontFiles[selectedFontBuffer]);
				else
				{
					auxString.format("Memory file \"%s\"", FontFiles[selectedFontBuffer]);
					audioBuffer_->setName(auxString);
				}
				fontHasChanged = hasLoaded;
			}

			ImGui::TreePop();
		}

		if (fontHasChanged)
			textNode_->setFont(font_.get());

		ImGui::TreePop();
	}

	ImGui::End();
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}
