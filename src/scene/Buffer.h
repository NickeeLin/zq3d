#ifndef BUFFER_H
#define BUFFER_H

#include "header/globals.h"

/**
* Constant Buffers
*/

class LightSource;
class Component;

struct CBLight
{
	CBLight(LightSource* lightSource);
	CBLight() {}

	glm::vec4 Active = {}; // { Bool<float>, Type<IconType>, 0, 0 }
	glm::vec4 Ambient = {};
	glm::vec4 Angles = {};
	glm::vec4 Attenuation = {};
	glm::vec4 Diffuse = {};
	glm::vec4 Direction = {};
	glm::vec4 Position = {};
	glm::vec4 Specular = {};
	glm::mat4 ViewProjection = {};
};

struct CBMatrix
{
	CBMatrix(Component* mesh, bool removeTranslation);
	CBMatrix(LightSource* lightSource, Component* mesh);
	CBMatrix() {}

	glm::mat4 Normal = {};
	glm::mat4 Model = {};
	glm::mat4 VP[MAX_TEXTURES];
	glm::mat4 MVP = {};
};

struct CBColor
{
	CBColor(const glm::vec4& color);
	CBColor() {}

	glm::vec4 Color = {};
};

struct CBDefault
{
	CBDefault(Component* mesh, const DrawProperties& properties);
	CBDefault() {}

	CBLight LightSources[MAX_LIGHT_SOURCES];

	glm::vec4 IsTextured[MAX_TEXTURES];
	glm::vec4 TextureScales[MAX_TEXTURES];

	glm::vec4 MeshSpecular = {};
	glm::vec4 MeshDiffuse = {};

	glm::vec4 ClipMax = {};
	glm::vec4 ClipMin = {};
	glm::vec4 EnableClipping = {};

	glm::vec4 CameraPosition = {};
	glm::vec4 ComponentType = {};
	glm::vec4 EnableSRGB = {};
	glm::vec4 WaterProps = {}; // { MoveFactor, WaveStrength, 0, 0 }
};

struct CBDepth
{
	CBDepth(const glm::vec3& lightPosition, int depthLayer);
	CBDepth() {}

	glm::vec4 lightPosition = {}; // { x, y, z, depthLayer }
};

struct CBHUD
{
	CBHUD(const glm::vec4& color, bool transparent);
	CBHUD() {}

	glm::vec4 MaterialColor = {};
	glm::vec4 IsTransparent = {};
};

class Buffer
{
public:
	Buffer(std::vector<uint32_t>& indices);
	Buffer(std::vector<float>& data);
	Buffer(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords);
	Buffer();
	~Buffer();

public:
	UINT           BufferStride;
	//VkBuffer       IndexBuffer;
	//VkDeviceMemory IndexBufferMemory;
	//VKPipeline     Pipeline;
	//VKUniform      Uniform;
	//VkBuffer       VertexBuffer;
	//VkDeviceMemory VertexBufferMemory;

private:
	GLuint             id;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<float> vertices;

public:
	GLuint ID();
	size_t Normals();
	void   ResetPipelines();
	size_t TexCoords();
	size_t Vertices();

private:
	void init();
};
#endif // BUFFER_H	