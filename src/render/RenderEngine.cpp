#include "RenderEngine.h"
#include "ShaderManager.h"
#include "ShaderProgram.h"
#include "scene/Mesh.h"
#include "scene/Camera.h"
#include <scene/SceneManager.h>

static void CheckGLError()
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

GLCanvas                RenderEngine::Canvas = {};
DrawModeType            RenderEngine::drawMode = DRAW_MODE_FILLED;
Camera* RenderEngine::CameraMain = nullptr;
GPUDescription          RenderEngine::GPU = {};
bool                    RenderEngine::DrawBoundingVolume = false;
bool                    RenderEngine::EnableSRGB = true;
Mesh* RenderEngine::Skybox = nullptr;
std::vector<Component*> RenderEngine::HUDs;
std::vector<Component*> RenderEngine::LightSources;
bool                    RenderEngine::Ready = false;
std::vector<Component*> RenderEngine::Renderables;
GraphicsAPI             RenderEngine::SelectedGraphicsAPI = GRAPHICS_API_UNKNOWN;

void RenderEngine::Close()
{
	//InputManager::Reset();
	//SceneManager::Clear();
	ShaderManager::Close();

	//_DELETEP(SceneManager::DepthMap2D);
	//_DELETEP(SceneManager::DepthMapCube);

	//_DELETEP(SceneManager::EmptyCubemap);
	//_DELETEP(SceneManager::EmptyTexture);

	//_DELETEP(RenderEngine::Canvas.DX);
	_DELETEP(RenderEngine::Canvas.GL);
	//_DELETEP(RenderEngine::Canvas.VK);

	//if (RenderEngine::Canvas.Canvas != nullptr) {
	//	RenderEngine::Canvas.Canvas->DestroyChildren();
	//	RenderEngine::Canvas.Canvas->Destroy();
	//	RenderEngine::Canvas.Canvas = nullptr;
	//}
}

void RenderEngine::Draw()
{
	RenderEngine::createDepthFBO();
	RenderEngine::createWaterFBOs();
	CheckGLError();
	RenderEngine::clear(CLEAR_VALUE_DEFAULT, {});
	CheckGLError();
	RenderEngine::drawScene();
	CheckGLError();
	if (RenderEngine::Canvas.Canvas != nullptr)
		RenderEngine::Canvas.Canvas->SwapBuffers();
}

uint16_t RenderEngine::GetDrawMode()
{
	if (RenderEngine::drawMode == DRAW_MODE_FILLED)
	{
		return GL_TRIANGLES;

		/*switch (RenderEngine::SelectedGraphicsAPI) {
#if defined _WINDOWS
		case GRAPHICS_API_DIRECTX11:
		case GRAPHICS_API_DIRECTX12:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
#endif
		case GRAPHICS_API_OPENGL:
			return GL_TRIANGLES;
		case GRAPHICS_API_VULKAN:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		default:
			throw;
		}*/
	}
	else if (RenderEngine::drawMode == DRAW_MODE_WIREFRAME)
	{
		return GL_LINE_STRIP;
		/*switch (RenderEngine::SelectedGraphicsAPI) {
#if defined _WINDOWS
		case GRAPHICS_API_DIRECTX11:
		case GRAPHICS_API_DIRECTX12:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
#endif
		case GRAPHICS_API_OPENGL:
			return GL_LINE_STRIP;
		case GRAPHICS_API_VULKAN:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		default:
			throw;
		}*/
	}

	return DRAW_MODE_UNKNOWN;
}

int RenderEngine::Init(ZQFrame* window, const wxSize& size)
{
	RenderEngine::Canvas.AspectRatio = (float)((float)size.GetHeight() / (float)size.GetWidth());
	RenderEngine::Canvas.Position = wxDefaultPosition;
	RenderEngine::Canvas.Size = size;
	RenderEngine::Canvas.Window = window;

	int result = RenderEngine::setGraphicsAPI(GRAPHICS_API_OPENGL);

	if (result < 0)
		return result;

	return 0;
}

int RenderEngine::RemoveMesh(Component* mesh)
{
	return 0;
}

void RenderEngine::SetAspectRatio(const wxString& ratio)
{
}

void RenderEngine::SetCanvasSize(int width, int height)
{
}

void RenderEngine::SetDrawMode(DrawModeType mode)
{
}

void RenderEngine::SetDrawMode(const wxString& mode)
{
}

int RenderEngine::SetGraphicsAPI(const wxString& api)
{
	return 0;
}

void RenderEngine::SetVSync(bool enable)
{
}

void RenderEngine::clear(const glm::vec4& colorRGBA, const DrawProperties& properties)
{
	glClearColor(colorRGBA.r, colorRGBA.g, colorRGBA.b, colorRGBA.a);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderEngine::createDepthFBO()
{
}

void RenderEngine::createWaterFBOs()
{
}

int RenderEngine::drawBoundingVolumes()
{
	return 0;
}

int RenderEngine::drawSelected()
{
	return 0;
}

int RenderEngine::drawHUDs()
{
	return 0;
}

int RenderEngine::drawLightSources()
{
	return 0;
}

void RenderEngine::drawScene()
{
	DrawProperties properties = {}, properties2 = {};
	RenderEngine::drawRenderables(properties);
	RenderEngine::drawLightSources();
	RenderEngine::drawSelected();
	RenderEngine::drawBoundingVolumes();
	RenderEngine::drawSkybox(properties2);
	RenderEngine::drawHUDs();
}

int RenderEngine::initResources()
{
	//wxString              emptyFile = Utils::RESOURCE_IMAGES["emptyTexture"];
	//std::vector<wxString> emptyFiles = { emptyFile, emptyFile, emptyFile, emptyFile, emptyFile, emptyFile };

	//SceneManager::EmptyTexture = new Texture(emptyFile);
	//SceneManager::EmptyCubemap = new Texture(emptyFiles);

	//if (!SceneManager::EmptyTexture->IsOK() || !SceneManager::EmptyCubemap->IsOK())
	//	return -1;

	//SceneManager::DepthMap2D = new FrameBuffer(wxSize(FBO_TEXTURE_SIZE, FBO_TEXTURE_SIZE), FBO_DEPTH, TEXTURE_2D_ARRAY);
	//SceneManager::DepthMapCube = new FrameBuffer(wxSize(FBO_TEXTURE_SIZE, FBO_TEXTURE_SIZE), FBO_DEPTH, TEXTURE_CUBEMAP_ARRAY);

	//if ((SceneManager::DepthMap2D->GetTexture() == nullptr) || (SceneManager::DepthMapCube->GetTexture() == nullptr))
	//	return -2;

	return 0;
}

void RenderEngine::setDrawSettingsGL(ShaderID shaderID)
{
	switch (shaderID) {
	case SHADER_ID_HUD:
		glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_CLAMP);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		break;
	case SHADER_ID_SKYBOX:
		glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_CLAMP);
		glDisable(GL_STENCIL_TEST);
		break;
	case SHADER_ID_DEPTH:
	case SHADER_ID_DEPTH_OMNI:
		glEnable(GL_CULL_FACE);  glCullFace(GL_FRONT); glFrontFace(GL_CCW);
		glEnable(GL_DEPTH_CLAMP);
		glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); glDepthMask(GL_TRUE);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		break;
	default:
		glEnable(GL_CULL_FACE);  glCullFace(GL_BACK);  glFrontFace(GL_CCW);
		glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_CLAMP);
		glDisable(GL_STENCIL_TEST);
		break;
	}
}

int RenderEngine::setGraphicsAPI(GraphicsAPI api)
{
	RenderEngine::Ready = false;
	RenderEngine::SelectedGraphicsAPI = api;

	RenderEngine::Close();	

	// RE-CREATE THE CANVAS
	if (RenderEngine::setGraphicsApiCanvas() < 0)
		return -1;

	int result = -2;

	// RE-CREATE THE GRAPHICS CONTEXT
	switch (api) {
#if defined _WINDOWS
	case GRAPHICS_API_DIRECTX11:
	case GRAPHICS_API_DIRECTX12:
		result = RenderEngine::setGraphicsApiDX(api);
		break;
#endif
	case GRAPHICS_API_OPENGL:
		result = RenderEngine::setGraphicsApiGL();
		break;
	case GRAPHICS_API_VULKAN:
		result = RenderEngine::setGraphicsApiVK();
		break;
	default:
		throw;
	}

	if (result < 0)
		return result;

	CheckGLError();
	// RE-INITIALIZE ENGINE MODULES AND RESOURCES
	if (ShaderManager::Init() < 0) {
		RenderEngine::Close();
		return -3;
	}
	CheckGLError();
	if (RenderEngine::initResources() < 0) {
		RenderEngine::Close();
		return -4;
	}

	//if (InputManager::Init() < 0) {
	//	RenderEngine::Close();
	//	return -5;
	//}

	if (RenderEngine::CameraMain == nullptr) {
		//RenderEngine::CameraMain = new Camera(RenderEngine::Canvas.Canvas);
		SceneManager::AddComponent(new Camera(RenderEngine::Canvas.Canvas));
		SceneManager::LoadLightSource(ID_ICON_LIGHT_DIRECTIONAL);
	}

	RenderEngine::Ready = true;
	return 0;
}

int RenderEngine::setGraphicsApiCanvas()
{
	// Defaults: RGBA, Z-depth 16 bits, double buffering, 1 sample buffer, 4 samplers.
	wxGLAttributes attribs = {};
	attribs.PlatformDefaults().Defaults().Samplers(16).EndList();

	//RenderEngine::Canvas.Canvas = new wxGLCanvas(
	//	RenderEngine::Canvas.Window, attribs, wxID_ANY,
	//	RenderEngine::Canvas.Position, RenderEngine::Canvas.Size
	//);

	//RenderEngine::Canvas.Window->SetCanvas(RenderEngine::Canvas.Canvas);
	//RenderEngine::SetDrawMode(RenderEngine::Canvas.Window->SelectedDrawMode());

	RenderEngine::Canvas.GL = new wxGLContext(RenderEngine::Canvas.Canvas);

	if (!RenderEngine::Canvas.GL->IsOK())
		return -1;

	RenderEngine::Canvas.Canvas->SetCurrent(*RenderEngine::Canvas.GL);

	if (!gladLoadGL())
		return -2;
	//if (glewInit() != GLEW_OK)
	//	return -2;
	CheckGLError();
	return 0;
}

int RenderEngine::setGraphicsApiDX(GraphicsAPI api)
{
	return 0;
}

int RenderEngine::drawRenderables(DrawProperties& properties)
{
	if (RenderEngine::Renderables.empty())
		return 1;

	if (RenderEngine::SelectedGraphicsAPI == GRAPHICS_API_OPENGL)
		RenderEngine::setDrawSettingsGL(SHADER_ID_DEFAULT);

	if (properties.Shader == SHADER_ID_UNKNOWN)
		properties.Shader = (RenderEngine::drawMode == DRAW_MODE_FILLED ? SHADER_ID_DEFAULT : SHADER_ID_WIREFRAME);

	RenderEngine::drawMeshes(RenderEngine::Renderables, properties);

	properties.Shader = SHADER_ID_UNKNOWN;

	return 0;
}

int RenderEngine::drawSkybox(DrawProperties& properties)
{
	return 0;
}

int RenderEngine::drawMeshGL(Component* mesh, ShaderProgram* shaderProgram, DrawProperties& properties)
{
	if ((RenderEngine::CameraMain == nullptr) ||
		(shaderProgram == nullptr) || (shaderProgram->Program() < 1) ||
		(mesh == nullptr) || (dynamic_cast<Mesh*>(mesh)->IBO() < 1))
	{
		return -1;
	}

	// SHADER ATTRIBUTES AND UNIFORMS
	shaderProgram->UpdateAttribsGL(mesh);
	shaderProgram->UpdateUniformsGL(mesh, properties);

	// DRAW
	if (dynamic_cast<Mesh*>(mesh)->IBO() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dynamic_cast<Mesh*>(mesh)->IBO());
		glDrawElements(RenderEngine::GetDrawMode(), (GLsizei)dynamic_cast<Mesh*>(mesh)->NrOfIndices(), GL_UNSIGNED_INT, nullptr);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_VERTEX_ARRAY, dynamic_cast<Mesh*>(mesh)->VBO());
		glDrawArrays(RenderEngine::GetDrawMode(), 0, (GLsizei)dynamic_cast<Mesh*>(mesh)->NrOfVertices());
		glBindBuffer(GL_VERTEX_ARRAY, 0);
	}

	// UNBIND TEXTURES
	for (int i = 0; i < MAX_TEXTURES; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	glActiveTexture(GL_TEXTURE0);
	return 0;
}

void RenderEngine::drawMesh(Component* mesh, ShaderProgram* shaderProgram, DrawProperties& properties)
{
	switch (RenderEngine::SelectedGraphicsAPI) {
#if defined _WINDOWS
	case GRAPHICS_API_DIRECTX11:
		//RenderEngine::drawMeshDX11(mesh, shaderProgram, properties);
		break;
	case GRAPHICS_API_DIRECTX12:
		//RenderEngine::drawMeshDX12(mesh, shaderProgram, properties);
		break;
#endif
	case GRAPHICS_API_OPENGL:
		RenderEngine::drawMeshGL(mesh, shaderProgram, properties);
		break;
	case GRAPHICS_API_VULKAN:
		//RenderEngine::drawMeshVK(mesh, shaderProgram, properties);
		break;
	default:
		throw;
	}
}

void RenderEngine::drawMeshes(const std::vector<Component*> meshes, DrawProperties& properties)
{
	ShaderProgram* shaderProgram = RenderEngine::setShaderProgram(true, properties.Shader);

	for (auto mesh : meshes)
	{
		if (!properties.DrawBoundingVolume &&
			((properties.DrawSelected && !dynamic_cast<Mesh*>(mesh)->IsSelected()) ||
				(!properties.DrawSelected && dynamic_cast<Mesh*>(mesh)->IsSelected())))
		{
			continue;
		}

		// SKIP RENDERING WATER WHEN CREATING FBO
		//if ((mesh->Type() == COMPONENT_WATER) && (properties.FBO != nullptr) && (properties.FBO->Type() != FBO_UNKNOWN))
		//	continue;

		glm::vec4 oldColor = mesh->ComponentMaterial.diffuse;

		//if (properties.DrawSelected)
		//	mesh->ComponentMaterial.diffuse = SceneManager::SelectColor;
		if (properties.DrawBoundingVolume) {
			//RenderEngine::drawMesh(dynamic_cast<Mesh*>(mesh)->GetBoundingVolume(), shaderProgram, properties);
		}
		else {
			RenderEngine::drawMesh(mesh, shaderProgram, properties);
		}

		if (properties.DrawSelected)
			mesh->ComponentMaterial.diffuse = oldColor;
	}

	RenderEngine::setShaderProgram(false);
}

int RenderEngine::setGraphicsApiGL()
{
	glViewport(0, 0, RenderEngine::Canvas.Size.GetWidth(), RenderEngine::Canvas.Size.GetHeight());

	//RenderEngine::SetVSync(RenderEngine::Canvas.Window->VSyncEnable->GetValue());
	CheckGLError();
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP);
	CheckGLError();
	RenderEngine::GPU.Renderer = glGetString(GL_RENDERER);
	RenderEngine::GPU.Vendor = glGetString(GL_VENDOR);
	RenderEngine::GPU.Version = wxString("OpenGL ").append(glGetString(GL_VERSION));

	return 0;
}

int RenderEngine::setGraphicsApiVK()
{
	return 0;
}

ShaderProgram* RenderEngine::setShaderProgram(bool enable, ShaderID program)
{
	if (RenderEngine::SelectedGraphicsAPI == GRAPHICS_API_OPENGL)
		glUseProgram(enable ? ShaderManager::Programs[program]->Program() : 0);

	return (enable ? ShaderManager::Programs[program] : nullptr);
}
