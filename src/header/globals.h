#ifndef GLOBALS_H
#define GLOBALS_H

#include <glad/glad.h>

// GLM - OpenGL Mathematics
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

// AssImp
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <wx/wx.h>
#include <wx/event.h>
#include "wx/glcanvas.h"
#include <wx/timer.h>
#include <wx/gdicmn.h>

class ZQFrame;
class LightSource;
class Camera;
class Component;
class Mesh;
class ShaderProgram;

static const uint32_t  BUFFER_SIZE = 1024;
static const glm::vec4 CLEAR_VALUE_COLOR = { 0.0f, 0.0f, 1.0f, 1.0f };
static const glm::vec4 CLEAR_VALUE_DEFAULT = { 0.0f, 0.2f, 0.4f, 1.0f };
static const glm::vec4 CLEAR_VALUE_DEPTH = { 1.0f, 1.0f, 1.0f, 1.0f };
static const int       FBO_TEXTURE_SIZE = 1024;
static const uint32_t  LZMA_OFFSET_ID = 8;
static const uint32_t  LZMA_OFFSET_SIZE = 8;
static const uint32_t  MAX_CONCURRENT_FRAMES = 2;
static const uint32_t  MAX_LIGHT_SOURCES = 13;
static const uint32_t  MAX_TEXTURES = 6;
static const uint32_t  MAX_TEXTURE_SLOTS = (MAX_TEXTURES + MAX_LIGHT_SOURCES + MAX_LIGHT_SOURCES);
static const uint32_t  NR_OF_FRAMEBUFFERS = 2;

enum ShaderID
{
	SHADER_ID_UNKNOWN = -1,
	SHADER_ID_COLOR,
	SHADER_ID_DEFAULT,
	SHADER_ID_DEPTH,
	SHADER_ID_DEPTH_OMNI,
	SHADER_ID_HUD,
	SHADER_ID_SKYBOX,
	SHADER_ID_WIREFRAME,
	NR_OF_SHADERS
};


struct MouseState
{
	wxPoint Position = wxPoint(0, 0);
};

struct Resource
{
	wxString File = "";
	wxString Name = "";
	wxString Result = "";
};

struct GLCanvas
{
	float          AspectRatio = 0.0f;
	wxGLCanvas* Canvas = nullptr;
	//DXContext* DX = nullptr;
	wxGLContext* GL = nullptr;
	wxPoint        Position = wxPoint(0, 0);
	wxSize         Size = wxSize(0, 0);
	//VKContext* VK = nullptr;
	//WindowFrame* Window = nullptr;
	ZQFrame* Window = nullptr;
};

struct DrawProperties
{
	glm::vec3       ClipMax = {};
	glm::vec3       ClipMin = {};
	int             DepthLayer = 0;
	bool            DrawBoundingVolume = false;
	bool            DrawSelected = false;
	bool            EnableClipping = false;
	//FrameBuffer* FBO = nullptr;
	LightSource* Light = nullptr;
	ShaderID        Shader = SHADER_ID_UNKNOWN;
	//VkCommandBuffer VKCommandBuffer = nullptr;
};

struct GPUDescription
{
	wxString Renderer = "";
	wxString Vendor = "";
	wxString Version = "";
};

enum GraphicsAPI
{
	GRAPHICS_API_UNKNOWN = -1,
	GRAPHICS_API_OPENGL,
	GRAPHICS_API_VULKAN,
	GRAPHICS_API_DIRECTX11,
	GRAPHICS_API_DIRECTX12,
	NR_OF_GRAPHICS_ENGINES
};

enum IconType
{
	ID_ICON_UNKNOWN,
	ID_ICON_BROWSE,
	ID_ICON_PLANE,
	ID_ICON_CUBE,
	ID_ICON_UV_SPHERE,
	ID_ICON_ICO_SPHERE,
	ID_ICON_CYLINDER,
	ID_ICON_CONE,
	ID_ICON_TORUS,
	ID_ICON_MONKEY_HEAD,
	ID_ICON_SKYBOX,
	ID_ICON_TERRAIN,
	ID_ICON_WATER,
	ID_ICON_HUD,
	ID_ICON_QUAD,
	ID_ICON_LIGHT_DIRECTIONAL,
	ID_ICON_LIGHT_POINT,
	ID_ICON_LIGHT_SPOT,
	ID_CANVAS,
	ID_ASPECT_RATIO,
	ID_FOV,
	ID_DRAW_MODE,
	ID_DRAW_BOUNDING,
	ID_GRAPHICS_API,
	ID_VSYNC,
	ID_SRGB,
	ID_COMPONENTS,
	ID_CHILDREN,
	ID_SCENE_DETAILS,
	ID_SCENE_CLEAR,
	ID_SCENE_LOAD,
	ID_SCENE_SAVE,
	ID_REMOVE_COMPONENT,
	ID_REMOVE_CHILD,
	ID_RESET_CAMERA,
	ID_TABS,
	ID_TABS_GEOMETRY,
	ID_TABS_LIGHTS
};


enum BoundingVolumeType
{
	BOUNDING_VOLUME_NONE, BOUNDING_VOLUME_BOX, BOUNDING_VOLUME_SPHERE, NR_OF_BOUNDING_VOLUMES
};

enum DrawModeType
{
	DRAW_MODE_UNKNOWN = -1, DRAW_MODE_FILLED, DRAW_MODE_WIREFRAME, NR_OF_DRAW_MODES
};

enum Attrib
{
	ATTRIB_NORMAL, ATTRIB_POSITION, ATTRIB_TEXCOORDS, NR_OF_ATTRIBS
};

enum UniformBufferTypeGL
{
	UBO_GL_MATRIX,
	UBO_GL_COLOR,
	UBO_GL_DEFAULT,
	UBO_GL_DEPTH,
	UBO_GL_HUD,
	UBO_GL_TEXTURES0, UBO_GL_TEXTURES1, UBO_GL_TEXTURES2, UBO_GL_TEXTURES3, UBO_GL_TEXTURES4, UBO_GL_TEXTURES5,
	UBO_GL_TEXTURES6,
	UBO_GL_TEXTURES7,
	NR_OF_UBOS_GL
};

#ifndef _DELETEP
#define _DELETEP(x) if (x != nullptr) { delete x; x = nullptr; }
#endif

#endif // GLOBALS_H