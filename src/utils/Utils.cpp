#include "Utils.h"
#include <scene/Mesh.h>
#include <fstream>

const wxString Utils::APP_NAME = "3D Engine";
const uint8_t  Utils::APP_VERSION_MAJOR = 1;
const uint8_t  Utils::APP_VERSION_MINOR = 0;
const uint8_t  Utils::APP_VERSION_PATCH = 0;
const wxString Utils::APP_VERSION = "1.0.0";
const wxString Utils::ASPECT_RATIOS[] = { "16:9", "4:3" };
const wxChar* Utils::BOUNDING_VOLUMES[] = { wxT("none"), wxT("box"), wxT("sphere"), nullptr };
const wxString Utils::COPYRIGHT = "\u00A9 2025;";
const wxString Utils::DRAW_MODES[] = { "Filled", "Wireframe" };
const wxString Utils::FOVS[] = { "45\u00B0", "60\u00B0", "75\u00B0", "90\u00B0" };
//const wxString Utils::GOOGLE_ADS_URL = "http://jammary.com/google_ad_horizontal.html";
const wxString Utils::IMAGE_FILE_FORMATS = "All supported formats|*.bmp;*.png;*.jpg;*.tif;*.gif;*.pnm;*.pcx;*.ico;*.cur;*.ani;*.tga;*.xpm";
const wxString Utils::SCENE_FILE_FORMAT = "Scene file (*.scene)|*.scene";
const wxString Utils::TESTED = "Tested on Windows 10 (64-bit)";
const wxSize   Utils::UI_ADS_SIZE = wxSize(730, 90);
const wxSize   Utils::UI_LIST_BOX_SIZE = wxSize(290, 150);
const wxSize   Utils::UI_RENDER_SIZE = wxSize(640, 360);
const wxSize   Utils::UI_WINDOW_SIZE = wxSize(1280, 875);
const wxSize   Utils::UI_PROPS_SIZE = wxSize(590, 280);
const wxSize   Utils::UI_TABS_SIZE = wxSize(1245, 85);

std::map<IconType, wxString> Utils::RESOURCE_MODELS = {
	{ ID_ICON_QUAD,        "resources/models/quad.blend" },
	{ ID_ICON_PLANE,       "resources/models/plane.blend" },
	{ ID_ICON_CUBE,        "resources/models/cube.blend" },
	{ ID_ICON_UV_SPHERE,   "resources/models/uv_sphere.blend" },
	{ ID_ICON_ICO_SPHERE,  "resources/models/ico_sphere.blend" },
	{ ID_ICON_CYLINDER,    "resources/models/cylinder.blend" },
	{ ID_ICON_CONE,        "resources/models/cone.blend" },
	{ ID_ICON_TORUS,       "resources/models/torus.blend" },
	{ ID_ICON_MONKEY_HEAD, "resources/models/monkey_head.blend" }
};

void Utils::CheckGLError()
{
	GLenum errLast = GL_NO_ERROR;

	for (;; )
	{
		GLenum err = glGetError();
		if (err == GL_NO_ERROR)
			return;

		// normally the error is reset by the call to glGetError() but if
		// glGetError() itself returns an error, we risk looping forever here
		// so check that we get a different error than the last time
		if (err == errLast)
		{
			wxLogError("OpenGL error state couldn't be reset.");
			return;
		}

		errLast = err;

		wxLogError("OpenGL error %d", err);
	}
}
float Utils::ToRadians(float degrees)
{
	return (degrees * glm::pi<float>() / 180.0f);
}

wxString Utils::LoadTextFile(const wxString& file)
{
	if (file.empty())
		return "";

	std::string   line;
	wxString       result = "";
	std::fstream fileStream(file.c_str().AsChar());

	if (!fileStream.good())
	{
		fileStream.close();
		return "";
	}

	while (std::getline(fileStream, line))
		result.append(line + std::string("\n"));

	fileStream.close();

	return result;
}

std::vector<AssImpMesh*> Utils::LoadModelFile(const wxString& file)
{
	std::vector<AssImpMesh*> meshes;
	const aiScene* scene = aiImportFile(file.c_str(), (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes));

	if ((scene == nullptr) || !scene->HasMeshes() || (scene->mNumMeshes == 0))
	{
		//wxMessageBox("ERROR! Failed to load " + file + "\n\n" + aiGetErrorString(), RenderEngine::Canvas.Window->GetTitle().c_str(), wxOK | wxICON_ERROR);

		if (scene != nullptr)
			aiReleaseImport(scene);

		return meshes;
	}

	AssImpMesh* mesh;
	aiNode* node;
	uint32_t nrOfChildren = (scene->mRootNode->mNumChildren > 0 ? scene->mRootNode->mNumChildren : 1);

	// SCENE CHILDREN
	for (uint32_t i = 0; i < nrOfChildren; i++) {
		node = (scene->mRootNode->mNumChildren > 0 ? scene->mRootNode->mChildren[i] : scene->mRootNode);

		// NODE MESHES
		for(uint32_t j = 0; j < node->mNumMeshes; j++) {
			mesh = new AssImpMesh();
			mesh->Mesh = scene->mMeshes[node->mMeshes[j]];
			mesh->Name = (mesh->Mesh->mName.length > 0 ? mesh->Mesh->mName.C_Str() : "Mesh");
			mesh->Scene = scene;
			mesh->Transformation = node->mTransformation;

			// MESH MATERIALS: http://assimp.sourceforge.net/lib_html/materials.html
			if (scene->mNumMaterials > 0) {
				aiString textures[MAX_TEXTURES] = {};
				aiMaterial* material = scene->mMaterials[mesh->Mesh->mMaterialIndex];
				aiColor4D diffuse = {};
				aiColor3D specIntensity = {};
				float specShininess = 0.0f;
				size_t pathSeparator = file.rfind("/");

				if (pathSeparator == wxString::npos)
					pathSeparator = file.rfind("\\");

				wxString path = file.substr(0, pathSeparator + 1);

				material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
				material->Get(AI_MATKEY_COLOR_SPECULAR, specIntensity);
				material->Get(AI_MATKEY_SHININESS, specShininess);
				material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textures[0]);
				material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), textures[1]);

				mesh->MeshMaterial.diffuse = { diffuse.r, diffuse.g, diffuse.b, diffuse.a };
				mesh->MeshMaterial.specular.intensity = { specIntensity.r, specIntensity.g, specIntensity.b };
				mesh->MeshMaterial.specular.shininess = specShininess;

				if (textures[0].length > 0)
					mesh->MeshMaterial.textures[0] = (path + textures[0].C_Str());

				if (textures[1].length > 0)
					mesh->MeshMaterial.textures[1] = (path + textures[1].C_Str());
			}
			if (mesh->Mesh != nullptr)
				meshes.push_back(mesh);
			else
				delete mesh;
		}
	}

	return meshes;
}

std::vector<Component*> Utils::LoadModelFile(const wxString& file, Component* parent)
{
	std::vector<Component*>  children;
	Mesh* mesh;
	std::vector<AssImpMesh*> aiMeshes = Utils::LoadModelFile(file);

	for (auto aiMesh : aiMeshes)
	{
		mesh = new Mesh(parent, aiMesh->Name);

		if (mesh == nullptr)
			continue;

		mesh->ComponentMaterial = aiMesh->MeshMaterial;

		mesh->LoadModelFile(aiMesh->Mesh, aiMesh->Transformation);

		if (!mesh->IsValid()) {
			_DELETEP(mesh);
			continue;
		}

		children.push_back(mesh);
	}

	if (!aiMeshes.empty())
		aiReleaseImport(aiMeshes[0]->Scene);

	for (auto it : aiMeshes) {
		_DELETEP(it);
	}

	return children;
}

GLsizei Utils::GetStride(GLsizei size, GLenum arrayType)
{
	GLsizei stride = 0;

	switch (arrayType) {
	case GL_BYTE:           stride = (size * sizeof(char));           break;
	case GL_UNSIGNED_BYTE:  stride = (size * sizeof(unsigned char));  break;
	case GL_SHORT:          stride = (size * sizeof(short));          break;
	case GL_UNSIGNED_SHORT: stride = (size * sizeof(unsigned short)); break;
	case GL_INT:            stride = (size * sizeof(int));            break;
	case GL_UNSIGNED_INT:   stride = (size * sizeof(unsigned int));   break;
	case GL_FLOAT:          stride = (size * sizeof(float));          break;
	default: throw;
	}

	return stride;
}

glm::vec4 Utils::ToVec4Float(bool boolean)
{
	float value = (boolean ? 1.0f : 0);
	return glm::vec4(value, value, value, value);
}

glm::vec4 Utils::ToVec4Float(int integer)
{
	float value = static_cast<float>(integer);
	return glm::vec4(value, value, value, value);
}

glm::vec4 Utils::ToVec4Float(bool boolean, int integer)
{
	return glm::vec4((boolean ? 1.0f : 0.0f), (float)integer, 0.0f, 0.0f);
}
