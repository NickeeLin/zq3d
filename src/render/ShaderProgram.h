#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <wx/glcanvas.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

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

class Component;

class ShaderProgram
{
public:
	ShaderProgram(const wxString& name);
	~ShaderProgram();

public:
	//GLuint Attribs[NR_OF_ATTRIBS];
	//GLint  Uniforms[NR_OF_UBOS_GL];
	//GLuint UniformBuffers[NR_OF_UBOS_GL];

private:
	wxString       m_name;
	GLuint         m_program;

public:
	bool           IsOK();
	int            Link();
	int            Load(const wxString& shaderFile);
	int            LoadAndLink(const wxString& vs, const wxString& fs, const wxString& gs = "");
	void           Log();
	void           Log(GLuint shader);
	wxString       Name();
	GLuint         Program();
	int            UpdateAttribsGL(Component* mesh);

	void Use();
	void SetInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(m_program, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(m_program, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(m_program, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
private:
	int  loadShaderGL(GLuint type, const wxString& sourceText);
	void setAttribsGL();
	void setUniformsGL();
	void updateUniformGL(GLint id, UniformBufferTypeGL buffer, void* values, size_t valuesSize);

	// const void* getBufferValues(const CBMatrix& matrices, Component* mesh, const DrawProperties& properties, size_t& bufferSize);
};

#endif