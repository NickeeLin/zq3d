#include <glad/glad.h>

#include "Texture.h"
#include <wx/image.h>

wxImage* LoadImageFile(const wxString& file, wxBitmapType type = wxBITMAP_TYPE_ANY)
{
	wxImage* image = new wxImage(file, type);

	if ((image != nullptr) && image->IsOk())
		return image;

	return nullptr;
}

GLenum GetImageFormat(const wxImage& image, bool srgb, bool in)
{
	return (in ? (srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8) : GL_RGBA);
}

uint8_t* ToRGBA(const wxImage& image)
{
	int      size = (image.GetWidth() * image.GetHeight() * 4);
	uint8_t* rgb = image.GetData();
	uint8_t* alpha = image.GetAlpha();
	uint8_t* rgba = (uint8_t*)std::malloc(size);

	if ((rgb == nullptr) || (rgba == nullptr))
		return nullptr;

	for (int i = 0, r = 0, a = 0; i < size; i += 4, r += 3, a++)
	{
		rgba[i + 0] = rgb[r + 0];
		rgba[i + 1] = rgb[r + 1];
		rgba[i + 2] = rgb[r + 2];
		rgba[i + 3] = (alpha != nullptr ? alpha[a] : 0xFF);
	}

	return rgba;
}

Texture::Texture(wxImage* image, bool repeat, bool flipY, bool transparent, const glm::vec2& scale)
	: Scale(scale), flipY(flipY), id(0), mipLevels(1), repeat(repeat), srgb(false), type(TEXTURE_2D), transparent(transparent), glType(GL_TEXTURE_2D)
{
	if (image != nullptr)
	{
		this->glType = GL_TEXTURE_2D;

		glEnable(this->glType);
		glCreateTextures(this->glType, 1, &this->id);
		if (this->id > 0)
			this->loadTextureImageGL(image);

		delete image;
	}
}

Texture::Texture(const wxString& imageFile, bool srgb, bool repeat, bool flipY, bool transparent, const glm::vec2& scale)
	: Scale(scale), flipY(flipY), id(0), mipLevels(1), repeat(repeat), srgb(false), type(TEXTURE_2D), transparent(transparent), glType(GL_TEXTURE_2D)
{
	wxImage* image = nullptr;

	if (!imageFile.empty())
		image = LoadImageFile(imageFile);

	if (image != nullptr)
	{
		this->glType = GL_TEXTURE_2D;
		glEnable(this->glType);
		glCreateTextures(this->glType, 1, &this->id);
		if (this->id > 0)
			this->loadTextureImageGL(image);

		delete image;
	}
}

Texture::~Texture()
{
}

void Texture::loadTextureImageGL(wxImage* image, bool cubemap, int index)
{
	wxImage  image2 = (this->flipY ? image->Mirror(false) : *image);
	GLenum   formatIn = GetImageFormat(image2, this->srgb, true);
	GLenum   formatOut = GetImageFormat(image2, false, false);
	uint8_t* pixels = ToRGBA(image2);

	glBindTexture(this->glType, this->id);

	this->size = wxSize(image2.GetWidth(), image2.GetHeight());
	this->mipLevels = ((uint32_t)(std::floor(std::log2(std::max(this->size.GetWidth(), this->size.GetHeight())))) + 1);
	this->transparent = (this->transparent && image2.HasAlpha());

	if (this->transparent)
		this->setAlphaBlendingGL(true);

	if (cubemap)
	{
		glTexParameteri(this->glType, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(this->glType, GL_TEXTURE_MAX_LEVEL, 0);

		glTexImage2D(
			(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index), 0, formatIn,
			image2.GetWidth(), image2.GetHeight(), 0, formatOut, GL_UNSIGNED_BYTE, pixels
		);

		this->setWrappingCubemapGL();
		this->setFilteringGL(false);
	}
	else
	{
		//glTexParameteri(this->glType, GL_TEXTURE_BASE_LEVEL, 0);
		//glTexParameteri(this->glType, GL_TEXTURE_MAX_LEVEL, this->mipLevels - 1);

		//// https://www.khronos.org/opengl/wiki/Common_Mistakes#Automatic_mipmap_generation
		//glTexStorage2D(this->glType, this->mipLevels, formatIn, image2.GetWidth(), image2.GetHeight());
		//glTexSubImage2D(this->glType, 0, 0, 0, image2.GetWidth(), image2.GetHeight(), formatOut, GL_UNSIGNED_BYTE, pixels);

		//glGenerateMipmap(this->glType);

		//this->setWrappingGL();
		//this->setFilteringGL(true);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image2.GetWidth(), image2.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image2.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

	}

	if (this->transparent)
		this->setAlphaBlendingGL(false);

	glBindTexture(this->glType, 0);

	std::free(pixels);

	if (this->flipY)
		image2.Destroy();
}

void Texture::reload()
{
}

void Texture::setAlphaBlendingGL(bool enable)
{
}

void Texture::setFilteringGL(bool mipmap)
{
	glTexParameteri(this->glType, GL_TEXTURE_MIN_FILTER, (mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
	glTexParameteri(this->glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::setWrappingGL()
{
	glTexParameteri(this->glType, GL_TEXTURE_WRAP_S, (this->repeat && !this->transparent ? GL_REPEAT : GL_CLAMP_TO_EDGE));
	glTexParameteri(this->glType, GL_TEXTURE_WRAP_T, (this->repeat && !this->transparent ? GL_REPEAT : GL_CLAMP_TO_EDGE));
}

void Texture::setWrappingCubemapGL()
{
}

bool Texture::FlipY()
{
	return this->flipY;
}
GLuint Texture::ID()
{
	return this->id;
}
wxString Texture::ImageFile(int index)
{
	if ((index >= 0) && (index < static_cast<int>(this->imageFiles.size())))
		return this->imageFiles[index];
	return wxString();
}
bool Texture::IsOK()
{
	return (this->id > 0);
}
uint32_t Texture::MipLevels()
{
	return this->mipLevels;
}
bool Texture::Repeat()
{
	return this->repeat;
}
void Texture::SetFlipY(bool newFlipY)
{
	this->flipY = newFlipY;
}

void Texture::SetRepeat(bool newRepeat)
{
}

void Texture::SetTransparent(bool newTransparent)
{
}

wxSize Texture::Size()
{
	return wxSize();
}

bool Texture::SRGB()
{
	return false;
}

bool Texture::Transparent()
{
	return false;
}

TextureType Texture::Type()
{
	return TextureType();
}

GLenum Texture::TypeGL()
{
	return this->glType;
}
