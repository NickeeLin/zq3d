// zq3d.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。
#pragma once

#include "header/globals.h"

// TODO: 在此处引用程序需要的其他标头。


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

// Define a new application type
class ZQApp : public wxApp
{
public:
	ZQApp() { m_glContext = nullptr; m_glStereoContext = nullptr; }

	// Returns the shared context used by all frames and sets it as current for
	// the given canvas.
	ZQGLContext& GetContext(wxGLCanvas* canvas, bool useStereo);

	// virtual wxApp methods
	virtual bool OnInit() override;
	virtual int OnExit() override;

private:
	// the GL context we use for all our mono rendering windows
	ZQGLContext* m_glContext;
	// the GL context we use for all our stereo rendering windows
	ZQGLContext* m_glStereoContext;
};

// Define a new frame type
class ZQFrame : public wxFrame
{
public:
	ZQFrame(bool stereoWindow = false);

private:
	void OnClose(wxCommandEvent& event);
	void OnNewWindow(wxCommandEvent& event);
	void OnNewStereoWindow(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

class ShaderProgram;
class Texture;
class Camera;
class ZQGLCanvas : public wxGLCanvas
{
public:
	ZQGLCanvas(ZQFrame* parent, bool useStereo);
	~ZQGLCanvas();
private:
	void InitGL();
	void GameLoop(wxIdleEvent& event);
	void OnPaint(wxPaintEvent& event);
	void Spin(float xSpin, float ySpin);
	void OnKeyDown(wxKeyEvent& event);
	void OnMouse(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseScroll(wxMouseEvent& event);
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

enum
{
	NEW_STEREO_WINDOW = wxID_HIGHEST
};