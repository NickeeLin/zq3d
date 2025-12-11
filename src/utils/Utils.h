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
	static const wxString APP_NAME;
	static const uint8_t  APP_VERSION_MAJOR;
	static const uint8_t  APP_VERSION_MINOR;
	static const uint8_t  APP_VERSION_PATCH;
	static const wxString APP_VERSION;
	static const wxString ASPECT_RATIOS[];
	static const wxChar*  BOUNDING_VOLUMES[];
	static const wxString COPYRIGHT;
	static const wxString DRAW_MODES[];
	static const wxString FOVS[];
	//static const wxString GOOGLE_ADS_URL;
	static const wxString IMAGE_FILE_FORMATS;
	static const wxString SCENE_FILE_FORMAT;
	static const wxString TESTED;
	static const wxSize   UI_ADS_SIZE;
	static const wxSize   UI_LIST_BOX_SIZE;
	static const wxSize   UI_RENDER_SIZE;
	static const wxSize   UI_WINDOW_SIZE;
	static const wxSize   UI_PROPS_SIZE;
	static const wxSize   UI_TABS_SIZE;

	static void CheckGLError();
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