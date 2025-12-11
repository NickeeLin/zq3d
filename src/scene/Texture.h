#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/vec2.hpp>
#include <wx/glcanvas.h>

enum TextureType
{
	TEXTURE_UNKNOWN = -1,
	TEXTURE_2D,
	TEXTURE_2D_ARRAY,
	TEXTURE_CUBEMAP,
	TEXTURE_CUBEMAP_ARRAY,
	NR_OF_TEXTURE_TYPES
};

class wxImage;
class wxString;

class Texture
{
public:
	Texture(wxImage* image, bool repeat = false, bool flipY = false, bool transparent = false, const glm::vec2& scale = { 1.0f, 1.0f });
	Texture(const wxString& imageFile, bool srgb = false, bool repeat = false, bool flipY = false, bool transparent = false, const glm::vec2& scale = { 1.0f, 1.0f });
	Texture(const std::vector<wxString>& imageFiles, bool repeat = false, bool flipY = false, bool transparent = false, const glm::vec2& scale = { 1.0f, 1.0f });
	~Texture();
public:
	glm::vec2 Scale;

private:
	bool                  flipY;
	GLuint                id;
	std::vector<wxString> imageFiles;
	uint32_t              mipLevels;
	bool                  repeat;
	wxSize                size;
	bool                  srgb;
	TextureType           type;
	bool                  transparent;
	GLenum                glType;

public:
	bool        FlipY();
	GLuint      ID();
	wxString    ImageFile(int index = 0);
	bool        IsOK();
	uint32_t    MipLevels();
	bool        Repeat();
	void        SetFlipY(bool newFlipY);
	void        SetRepeat(bool newRepeat);
	void        SetTransparent(bool newTransparent);
	wxSize      Size();
	bool        SRGB();
	bool        Transparent();
	TextureType Type();
	GLenum      TypeGL();

private:
	void loadTextureImageGL(wxImage* image, bool cubemap = false, int index = 0);
	//void loadTextureImagesVK(const std::vector<wxImage*>& images);
	void reload();
	void setAlphaBlendingGL(bool enable);
	void setFilteringGL(bool mipmap = true);
	//void setFilteringVK(VkSamplerCreateInfo& samplerInfo);
	void setWrappingGL();
	//void setWrappingVK(VkSamplerCreateInfo& samplerInfo);
	void setWrappingCubemapGL();
	//void setWrappingCubemapVK(VkSamplerCreateInfo& samplerInfo);
};

#endif