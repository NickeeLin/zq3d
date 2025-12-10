#include "Buffer.h"
#include <render/RenderEngine.h>
#include <scene/Camera.h>
#include <scene/Component.h>
#include <scene/Texture.h>
#include <utils/Utils.h>
#include "Light.h"
#include "LightSource.h"
#include "SceneManager.h"

CBLight::CBLight(LightSource* lightSource)
{
	Light light = lightSource->GetLight();

	this->Active = Utils::ToVec4Float(light.active, (int)lightSource->SourceType());
	this->Ambient = glm::vec4(light.material.ambient, 0.0f);
	this->Attenuation = glm::vec4(light.attenuation.constant, light.attenuation.linear, light.attenuation.quadratic, 0.0f);
	this->Diffuse = light.material.diffuse;
	this->Direction = glm::vec4(light.direction, 0.0f);
	this->Specular = glm::vec4(light.material.specular.intensity, light.material.specular.shininess);
	this->Position = glm::vec4(light.position, 0.0f);

	if ((light.innerAngle > 0.1f) && (light.outerAngle > light.innerAngle))
		this->Angles = glm::vec4(glm::cos(light.innerAngle), glm::cos(light.outerAngle), 0.0f, 0.0f);

	this->ViewProjection = (lightSource->Projection() * lightSource->View(0));
}

CBMatrix::CBMatrix(Component* mesh, bool removeTranslation)
{
	this->Model = mesh->Matrix();
	this->Normal = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->Model))));
	this->MVP = RenderEngine::CameraMain->MVP(this->Model, removeTranslation);
}

CBMatrix::CBMatrix(LightSource* lightSource, Component* mesh)
{
	glm::mat4 depthTransform = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f
	);

	this->Model = mesh->Matrix();
	this->MVP = lightSource->MVP(this->Model);

	glm::mat4 projection = lightSource->Projection();

	for (uint32_t i = 0; i < MAX_TEXTURES; i++)
		this->VP[i] = (projection * lightSource->View(i));

	// Apply the depth transform
	switch (RenderEngine::SelectedGraphicsAPI) {
	case GRAPHICS_API_DIRECTX11:
	case GRAPHICS_API_DIRECTX12:
		depthTransform[1][1] *= -1.0f;

		this->MVP = (depthTransform * this->MVP);

		for (uint32_t i = 0; i < MAX_TEXTURES; i++)
			this->VP[i] = (depthTransform * this->VP[i]);

		break;
	case GRAPHICS_API_VULKAN:
		this->MVP = (depthTransform * this->MVP);
		break;
	default:
		break;
	}
}

CBColor::CBColor(const glm::vec4& color)
{
	this->Color = color;
}

CBDefault::CBDefault(Component* mesh, const DrawProperties& properties)
{
	for (uint32_t i = 0; i < MAX_LIGHT_SOURCES; i++) {
		if (SceneManager::LightSources[i] != nullptr)
			this->LightSources[i] = CBLight(SceneManager::LightSources[i]);
	}

	for (int i = 0; i < MAX_TEXTURES; i++)
		this->IsTextured[i] = Utils::ToVec4Float(mesh->IsTextured(i));

	for (int i = 0; i < MAX_TEXTURES; i++)
		this->TextureScales[i] = glm::vec4(mesh->Textures[i]->Scale.x, mesh->Textures[i]->Scale.y, 0.0f, 0.0f);

	this->MeshSpecular = glm::vec4(mesh->ComponentMaterial.specular.intensity, mesh->ComponentMaterial.specular.shininess);
	this->MeshDiffuse = mesh->ComponentMaterial.diffuse;

	this->ClipMax = glm::vec4(properties.ClipMax, 0.0f);
	this->ClipMin = glm::vec4(properties.ClipMin, 0.0f);
	this->EnableClipping = Utils::ToVec4Float(properties.EnableClipping);

	this->CameraPosition = glm::vec4(RenderEngine::CameraMain->Position(), 0.0f);
	this->ComponentType = Utils::ToVec4Float(static_cast<int>(mesh->Type()));
	this->EnableSRGB = Utils::ToVec4Float(RenderEngine::EnableSRGB);
	this->WaterProps = {};

	if (mesh->Type() == COMPONENT_WATER) {
		//auto water = dynamic_cast<Water*>(mesh->Parent);
		//this->WaterProps = { water->FBO()->MoveFactor(), water->FBO()->WaveStrength, 0.0f, 0.0f };
	}
}

CBDepth::CBDepth(const glm::vec3& lightPosition, int depthLayer)
{
	this->lightPosition = glm::vec4(lightPosition, static_cast<float>(depthLayer));
}

CBHUD::CBHUD(const glm::vec4& color, bool transparent)
{
	this->MaterialColor = color;
	this->IsTransparent = Utils::ToVec4Float(transparent);
}

Buffer::Buffer(std::vector<uint32_t>& indices)
{
	this->id = 0;
	this->BufferStride = sizeof(uint32_t);
	glCreateBuffers(1, &this->id);
	//glNamedBufferData(this->id, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	if (id > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indices.size() * sizeof(uint32_t)), &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

Buffer::Buffer(std::vector<float>& data)
{
	this->id = 0;
	this->BufferStride = sizeof(float);
	glCreateBuffers(1, &this->id);
	//glNamedBufferData(this->id, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	if (id > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, (data.size() * sizeof(float)), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

Buffer::Buffer(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords)
{
	this->id = 0;
	this->vertices = vertices;
	this->normals = normals;
	this->texCoords = texCoords;
	this->BufferStride = sizeof(float);
	this->init();
}

Buffer::Buffer()
{
	this->id = 0;
	this->BufferStride = sizeof(float);
	this->init();
}

Buffer::~Buffer()
{
	if (this->id > 0) {
		glDeleteBuffers(1, &this->id);
		this->id = 0;
	}
}

GLuint Buffer::ID()
{
	return this->id;
}

size_t Buffer::Normals()
{
	return this->normals.size();
}

void Buffer::ResetPipelines()
{
}

size_t Buffer::TexCoords()
{
	return this->texCoords.size();
}

size_t Buffer::Vertices()
{
	return this->vertices.size();
}

void Buffer::init()
{
	this->BufferStride = 0;
	this->id = 0;
	//this->IndexBuffer = nullptr;
	//this->IndexBufferMemory = nullptr;
	//this->Pipeline = {};
	//this->Uniform = {};
	//this->VertexBuffer = nullptr;
	//this->VertexBufferMemory = nullptr;
}
