#ifndef ZQGLCANVAS_H
#define ZQGLCANVAS_H

#include "header/globals.h"

class ShaderProgram;
class Texture;
class Camera;
class ZQGLCanvas : public wxGLCanvas
{
public:
	ZQGLCanvas(wxFrame* parent);
	~ZQGLCanvas();
private:
	void TestInitGL();

	void OnPaint(wxPaintEvent& event);
	void Spin(float xSpin, float ySpin);
	void OnKeyDown(wxKeyEvent& event);
	void OnMouse(wxMouseEvent& event);
	void OnSpinTimer(wxTimerEvent& WXUNUSED(event));

	// angles of rotation around x- and y- axis
	float m_xangle,
		m_yangle;

	wxTimer m_spinTimer;
	bool m_useStereo,
		m_stereoWarningAlreadyDisplayed;

	ShaderProgram* m_shader;
	Texture* m_texture1, * m_texture2;
	Camera* m_camera;
	// timing
	double      DeltaTime = 0.0;
	int         FPS = 0;
	wxStopWatch deltaTimer;


	unsigned int m_VBO, m_VAO, m_EBO, texture;;
	wxGLContext* m_glContext;

	wxDECLARE_EVENT_TABLE();
};

#endif