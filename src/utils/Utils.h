#ifndef UTILS_H
#define UTILS_H
#include "header/globals.h"
#include "scene/Material.h"
#include <map>

class Component;

struct AssImpMesh
{
	aiMesh* Mesh = nullptr;
	Material       MeshMaterial = {};
	wxString       Name = "";
	const aiScene* Scene = nullptr;
	aiMatrix4x4    Transformation;
};

class Utils
{
private:
	Utils() {}
	~Utils() {}
public:
	static float ToRadians(float degrees);
	static wxString LoadTextFile(const wxString& file);
	static std::vector<AssImpMesh*> LoadModelFile(const wxString& file);
	static std::vector<Component*> LoadModelFile(const wxString& file, Component* parent);

	static GLsizei GetStride(GLsizei size, GLenum arrayType);

	static glm::vec4 ToVec4Float(bool boolean);
	static glm::vec4 ToVec4Float(int integer);
	static glm::vec4 ToVec4Float(bool boolean, int integer);

public:
	static std::map<IconType, wxString> RESOURCE_MODELS;
};

#endif // UTILS_H