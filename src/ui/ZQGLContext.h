#ifndef ZQGLCONTEXT_H
#define ZQGLCONTEXT_H

#include "header/globals.h"

// the rendering context used by all GL canvases
class ZQGLContext : public wxGLContext
{
public:
	ZQGLContext(wxGLCanvas* canvas);
	~ZQGLContext();

	// render the cube showing it at given angles
	void DrawRotatedCube(float xangle, float yangle);

	void DrwaTraiangle();
private:
	// textures for the cube faces
	GLuint m_textures[6];
};

#endif
