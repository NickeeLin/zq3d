#include <glad/glad.h>

#include "ShaderProgram.h"
#include "scene/Mesh.h"
#include "scene/Texture.h"

#include <string>
#include <fstream>
#include <utils/Utils.h>
#include <scene/Buffer.h>
#include <scene/Light.h>
#include <scene/LightSource.h>

ShaderProgram::ShaderProgram(const wxString& name, ShaderID id) : m_id(id), m_name(name)
{
	m_program = glCreateProgram();
}


ShaderProgram::~ShaderProgram()
{
	if (m_program > 0)
		glDeleteProgram(m_program);
}

void ShaderProgram::Use()
{
	if (IsOK())
		glUseProgram(m_program);
}

ShaderID ShaderProgram::ID()
{
	return this->m_id;

	//if (this->m_name == Utils::SHADER_RESOURCES_DX[SHADER_ID_COLOR].Name)
	//	return SHADER_ID_COLOR;
	//else if (this->m_name == Utils::SHADER_RESOURCES_DX[SHADER_ID_DEFAULT].Name)
	//	return SHADER_ID_DEFAULT;
	//else if (this->m_name == Utils::SHADER_RESOURCES_DX[SHADER_ID_DEPTH].Name)
	//	return SHADER_ID_DEPTH;
	//else if (this->m_name == Utils::SHADER_RESOURCES_DX[SHADER_ID_DEPTH_OMNI].Name)
	//	return SHADER_ID_DEPTH_OMNI;
	//else if (this->m_name == Utils::SHADER_RESOURCES_DX[SHADER_ID_HUD].Name)
	//	return SHADER_ID_HUD;
	//else if (this->m_name == Utils::SHADER_RESOURCES_DX[SHADER_ID_SKYBOX].Name)
	//	return SHADER_ID_SKYBOX;
	//else if (this->m_name == Utils::SHADER_RESOURCES_DX[SHADER_ID_WIREFRAME].Name)
	//	return SHADER_ID_WIREFRAME;

	//return SHADER_ID_UNKNOWN;
}

bool ShaderProgram::IsOK()
{
	return (m_program > 0);
}

int ShaderProgram::Link()
{
	GLint resultLink;
	glLinkProgram(m_program);
	glGetProgramiv(m_program, GL_LINK_STATUS, &resultLink);

	if (resultLink != GL_TRUE)
		return -1;
	return 0;
}

int ShaderProgram::Load(const wxString& shaderFile)
{
	return 0;
}

int ShaderProgram::LoadAndLink(const wxString& vs, const wxString& fs, const wxString& gs)
{
	auto vsText = Utils::LoadTextFile(vs);
	auto fsText = Utils::LoadTextFile(fs);
	auto gsText = Utils::LoadTextFile(gs);

	if (vsText.empty() || fsText.empty())
	{
		wxLogError("Failed to load shader files: %s, %s", vs, fs);
		return -1;
	}
	auto vertexShader = loadShaderGL(GL_VERTEX_SHADER, vsText);
	if (vertexShader < 0)
	{
		wxLogError("Failed to load vertex shader: %s", vs);
		return -1;
	}
	auto fragmentShader = loadShaderGL(GL_FRAGMENT_SHADER, fsText);
	if (fragmentShader < 0)
	{
		wxLogError("Failed to load fragment shader: %s", fs);
		glDeleteShader((GLuint)vertexShader);
		return -1;
	}

	if (!gsText.empty())
	{
		auto geometryShader = loadShaderGL(GL_GEOMETRY_SHADER, gsText);
		if (geometryShader < 0)
		{
			wxLogError("Failed to load geometry shader: %s", gs);
			glDeleteShader((GLuint)vertexShader);
			glDeleteShader((GLuint)fragmentShader);
			return -1;
		}
		glDeleteShader(geometryShader);
	}

	if (this->Link() < 0)
		return -4;

	this->setAttribsGL();
	this->setUniformsGL();

	return 0;
}

void ShaderProgram::Log()
{
#if defined _DEBUG
	std::vector<GLchar> error;
	GLint               errorLength = 0;

	glGetProgramiv(this->m_program, GL_INFO_LOG_LENGTH, &errorLength);

	if (errorLength > 0)
	{
		error.resize(errorLength);
		glGetProgramInfoLog(this->m_program, errorLength, &errorLength, &error[0]);

		wxLogDebug("%s\n", (char*)&error[0]);
	}
#endif
}

void ShaderProgram::Log(GLuint shader)
{
#if defined _DEBUG
	std::vector<GLchar> error;
	GLint               errorLength = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);

	if (errorLength > 0)
	{
		error.resize(errorLength);
		glGetShaderInfoLog(shader, errorLength, &errorLength, &error[0]);

		wxLogDebug("%s\n", (char*)&error[0]);
	}
#endif
}

wxString ShaderProgram::Name()
{
	return m_name;
}

GLuint ShaderProgram::Program()
{
	return m_program;
}

int ShaderProgram::UpdateAttribsGL(Component* mesh)
{
	GLint id;
	Mesh* mesh2 = dynamic_cast<Mesh*>(mesh);

	if ((mesh2->NBO() > 0) && ((id = this->Attribs[ATTRIB_NORMAL]) >= 0))
		mesh2->BindBuffer(mesh2->NBO(), id, 3, GL_FLOAT, GL_FALSE);

	if ((mesh2->VBO() > 0) && ((id = this->Attribs[ATTRIB_POSITION]) >= 0))
		mesh2->BindBuffer(mesh2->VBO(), id, 3, GL_FLOAT, GL_FALSE);

	if ((mesh2->TBO() > 0) && ((id = this->Attribs[ATTRIB_TEXCOORDS]) >= 0))
		mesh2->BindBuffer(mesh2->TBO(), id, 2, GL_FLOAT, GL_FALSE);

	return 0;
}

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

int ShaderProgram::UpdateUniformsGL(Component* mesh, const DrawProperties& properties)
{
	if (mesh == nullptr)
		return -1;

	ShaderID shaderID = this->ID();

	// MATRIX BUFFER
	GLint id = this->Uniforms[UBO_GL_MATRIX];

	if (id >= 0)
	{
		CBMatrix mb;

		if ((shaderID == SHADER_ID_DEPTH) || (shaderID == SHADER_ID_DEPTH_OMNI))
			mb = CBMatrix(properties.Light, mesh);
		else
			mb = CBMatrix(mesh, (shaderID == SHADER_ID_SKYBOX));

		this->updateUniformGL(id, UBO_GL_MATRIX, &mb, sizeof(mb));
	}
	CheckGLError();
	// COLOR BUFFER
	id = this->Uniforms[UBO_GL_COLOR];

	if (id >= 0) {
		CBColor cb = CBColor(mesh->ComponentMaterial.diffuse); // dynamic_cast<Mesh*>(mesh)->GetBoundingVolume() != nullptr ? mesh->ComponentMaterial.diffuse : mesh->Parent->ComponentMaterial.diffuse);
		this->updateUniformGL(id, UBO_GL_COLOR, &cb, sizeof(cb));
	}
	CheckGLError();
	// DEFAULT BUFFER
	id = this->Uniforms[UBO_GL_DEFAULT];

	if (id >= 0) {
		CBDefault db = CBDefault(mesh, properties);
		this->updateUniformGL(id, UBO_GL_DEFAULT, &db, sizeof(db));
	}
	CheckGLError();
	// DEPTH BUFFER
	id = this->Uniforms[UBO_GL_DEPTH];

	if (id >= 0) {
		CBDepth db = CBDepth(properties.Light->GetLight().position, properties.DepthLayer);
		this->updateUniformGL(id, UBO_GL_DEPTH, &db, sizeof(db));
	}
	CheckGLError();
	// HUD BUFFER
	id = this->Uniforms[UBO_GL_HUD];

	if (id >= 0) {
		//CBHUD hb = CBHUD(mesh->ComponentMaterial.diffuse, dynamic_cast<HUD*>(mesh->Parent)->Transparent);
		//this->updateUniformGL(id, UBO_GL_HUD, &hb, sizeof(hb));
	}

	// BIND MESH TEXTURES - Texture slots: [GL_TEXTURE0, GL_TEXTURE5]
	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		id = this->Uniforms[UBO_GL_TEXTURES0 + i];

		if (id >= 0) {
			glActiveTexture(GL_TEXTURE0 + i);
			glUniform1i(id, i);
			glBindTexture(mesh->Textures[i]->TypeGL(), mesh->Textures[i]->ID());
		}
		else {
			glBindTexture(GL_TEXTURE0 + i, 0);
		}
	}
	CheckGLError();
	// BIND DEPTH MAP - 2D TEXTURE ARRAY
	//id = this->Uniforms[UBO_GL_TEXTURES6];

	//if ((id >= 0) && (SceneManager::DepthMap2D != nullptr)) {
	//	glActiveTexture(GL_TEXTURE6);
	//	glUniform1i(this->Uniforms[UBO_GL_TEXTURES6], 6);
	//	glBindTexture(GL_TEXTURE_2D_ARRAY, SceneManager::DepthMap2D->GetTexture()->ID());
	//}
	//else {
	//	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	//}

	// BIND DEPTH MAP - CUBE MAP ARRAY
	//id = this->Uniforms[UBO_GL_TEXTURES7];

	//if ((id >= 0) && (SceneManager::DepthMapCube != nullptr)) {
	//	glActiveTexture(GL_TEXTURE7);
	//	glUniform1i(this->Uniforms[UBO_GL_TEXTURES7], 7);
	//	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SceneManager::DepthMapCube->GetTexture()->ID());
	//}
	//else {
	//	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
	//}

	CheckGLError();
#if defined _DEBUG
	glValidateProgram(this->m_program);

	GLint resultValid = GL_FALSE;
	glGetProgramiv(this->m_program, GL_VALIDATE_STATUS, &resultValid);

	if (resultValid != GL_TRUE)
		return -2;
#endif

	return 0;
}

int ShaderProgram::loadShaderGL(GLuint type, const wxString& sourceText)
{
	//if (RenderEngine::SelectedGraphicsAPI != GRAPHICS_API_OPENGL)
	//	return -1;

	if ((type != GL_VERTEX_SHADER) && (type != GL_FRAGMENT_SHADER) && (type != GL_GEOMETRY_SHADER))
		return -2;

	GLuint shader = glCreateShader(type);

	if (shader < 1)
		return -3;

	const GLchar* sourceTextGLchar = (const GLchar*)sourceText.c_str();
	GLint         sourceTextGlint = (const GLint)sourceText.size();

	glShaderSource(shader, 1, &sourceTextGLchar, &sourceTextGlint);
	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (result != GL_TRUE) {
		this->Log(shader);
		return -4;
	}

	glAttachShader(this->m_program, shader);

	return shader;
}

void ShaderProgram::setAttribsGL()
{
	glUseProgram(this->m_program);

	// ATTRIBS (BUFFERS)
	this->Attribs[ATTRIB_NORMAL] = glGetAttribLocation(this->m_program, "VertexNormal");
	this->Attribs[ATTRIB_POSITION] = glGetAttribLocation(this->m_program, "VertexPosition");
	this->Attribs[ATTRIB_TEXCOORDS] = glGetAttribLocation(this->m_program, "VertexTextureCoords");

	glUseProgram(0);
}

void ShaderProgram::setUniformsGL()
{
	glUseProgram(this->m_program);

	// MATRIX BUFFER
	this->Uniforms[UBO_GL_MATRIX] = glGetUniformBlockIndex(this->m_program, "MatrixBuffer");
	glGenBuffers(1, &this->UniformBuffers[UBO_GL_MATRIX]);

	// COLOR BUFFER
	this->Uniforms[UBO_GL_COLOR] = glGetUniformBlockIndex(this->m_program, "ColorBuffer");
	glGenBuffers(1, &this->UniformBuffers[UBO_GL_COLOR]);

	// DEFAULT BUFFER
	this->Uniforms[UBO_GL_DEFAULT] = glGetUniformBlockIndex(this->m_program, "DefaultBuffer");
	glGenBuffers(1, &this->UniformBuffers[UBO_GL_DEFAULT]);

	// DEPTH BUFFER
	this->Uniforms[UBO_GL_DEPTH] = glGetUniformBlockIndex(this->m_program, "DepthBuffer");
	glGenBuffers(1, &this->UniformBuffers[UBO_GL_DEPTH]);

	// HUD BUFFER
	this->Uniforms[UBO_GL_HUD] = glGetUniformBlockIndex(this->m_program, "HUDBuffer");
	glGenBuffers(1, &this->UniformBuffers[UBO_GL_HUD]);

	// MESH TEXTURES
	for (int i = 0; i < MAX_TEXTURES; i++)
		this->Uniforms[UBO_GL_TEXTURES0 + i] = glGetUniformLocation(this->m_program, wxString("Textures[" + std::to_string(i) + "]").c_str());

	// DEPTH MAP 2D TEXTURES
	this->Uniforms[UBO_GL_TEXTURES6] = glGetUniformLocation(this->m_program, wxString("DepthMapTextures2D").c_str());

	// DEPTH MAP CUBE TEXTURES
	this->Uniforms[UBO_GL_TEXTURES7] = glGetUniformLocation(this->m_program, wxString("DepthMapTexturesCube").c_str());

	glUseProgram(0);
}

void ShaderProgram::updateUniformGL(GLint id, UniformBufferTypeGL buffer, void* values, size_t valuesSize)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, id, this->UniformBuffers[buffer]);
	glBufferData(GL_UNIFORM_BUFFER, valuesSize, values, GL_STATIC_DRAW);
	glUniformBlockBinding(this->m_program, id, id);
}

// const void* ShaderProgram::getBufferValues(const CBMatrix& matrices, Component* mesh, const DrawProperties& properties, size_t& bufferSize)
// {
// 	return nullptr;
// }
