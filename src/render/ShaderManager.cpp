#include "ShaderManager.h"
#include "ShaderProgram.h"
#include "utils/Utils.h"

ShaderProgram* ShaderManager::Programs[NR_OF_SHADERS];

const std::vector<Resource> SHADER_RESOURCES_GL_VK = {
	{ "resources/shader/color.vs.glsl",      "color_vs",      "" },
	{ "resources/shader/color.fs.glsl",      "color_fs",      "" },
	{ "resources/shader/default.vs.glsl",    "default_vs",    "" },
	{ "resources/shader/default.fs.glsl",    "default_fs",    "" },
	{ "resources/shader/depth.vs.glsl",      "depth_vs",      "" },
	{ "resources/shader/depth.fs.glsl",      "depth_fs",      "" },
	{ "resources/shader/depth.omni.vs.glsl", "depth.omni_vs", "" },
	{ "resources/shader/depth.omni.fs.glsl", "depth.omni_fs", "" },
	{ "resources/shader/hud.vs.glsl",        "hud_vs",        "" },
	{ "resources/shader/hud.fs.glsl",        "hud_fs",        "" },
	{ "resources/shader/skybox.vs.glsl",     "skybox_vs",     "" },
	{ "resources/shader/skybox.fs.glsl",     "skybox_fs",     "" },
	{ "resources/shader/color.vs.glsl",      "wireframe_vs",  "" },
	{ "resources/shader/color.fs.glsl",      "wireframe_fs",  "" }
};

void ShaderManager::Close()
{
	for(int i=0; i < NR_OF_SHADERS; i++) {
		_DELETEP(Programs[i]);
	}
}

int  ShaderManager::Init()
{
	ShaderManager::Close();

	for(int i = 0; i  < NR_OF_SHADERS; i++) {
		Resource gs = {};
		Resource vs = SHADER_RESOURCES_GL_VK[(i * 2) + 0];
		Resource fs = SHADER_RESOURCES_GL_VK[(i * 2) + 1];

		if (vs.Name.rfind("_vs") == wxString::npos)
			continue;

		vs.Result = Utils::LoadTextFile(vs.File);
		fs.Result = Utils::LoadTextFile(fs.File);

		if (i == SHADER_ID_DEPTH_OMNI) {
			gs.Name = (vs.Name.substr(0, vs.Name.rfind("_vs")) + "_gs");
			gs.File = (vs.File.substr(0, vs.File.rfind(".vs.glsl")) + ".gs.glsl");
			gs.Result = Utils::LoadTextFile(gs.File);
		}

		wxString shaderName = vs.Name.substr(0, vs.Name.rfind("_"));
		ShaderManager::Programs[i] = new ShaderProgram(shaderName, ShaderID(i));

		int result;
		result = ShaderManager::Programs[i]->LoadAndLink(vs.File, fs.File, gs.File);
		//if (RenderEngine::SelectedGraphicsAPI == GRAPHICS_API_OPENGL)
		//	result = ShaderManager::Programs[i]->LoadAndLink(vs.Result, fs.Result, gs.Result);
		//else
		//	result = ShaderManager::Programs[i]->LoadAndLink(vs.File, fs.File, gs.File);

		if ((result < 0) || !ShaderManager::Programs[i]->IsOK()) {
			ShaderManager::Programs[i]->Log();
			return result;
		}
	}

	return 0;
}