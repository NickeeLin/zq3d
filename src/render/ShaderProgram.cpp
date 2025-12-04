#include <glad/glad.h>

#include "ShaderProgram.h"

#include <string>
#include <fstream>

wxString LoadTextFile(const wxString& file)
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

ShaderProgram::ShaderProgram(const wxString& name)
{
	m_name = name;
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
	auto vsText = LoadTextFile(vs);
	auto fsText = LoadTextFile(fs);
	auto gsText = LoadTextFile(gs);

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
}

void ShaderProgram::Log(GLuint shader)
{
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
}

void ShaderProgram::setUniformsGL()
{
}

void ShaderProgram::updateUniformGL(GLint id, UniformBufferTypeGL buffer, void* values, size_t valuesSize)
{
}

// const void* ShaderProgram::getBufferValues(const CBMatrix& matrices, Component* mesh, const DrawProperties& properties, size_t& bufferSize)
// {
// 	return nullptr;
// }
