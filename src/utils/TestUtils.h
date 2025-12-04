#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <wx/glcanvas.h>
class ShaderProgram;
class Texture;
class TestUtils
{
public:
	static void BuildTestTextureGL(GLuint& VAO, GLuint& VBO, GLuint& EBO);
	static void DrawTestTextureGL(ShaderProgram* pShader, Texture* m_texture, GLuint VAO);

	static void BuildTestCameraGL(GLuint& VAO, GLuint& VBO, GLuint& EBO);
};

#endif // TESTUTILS_H