#include <fmt/core.h>

#include "zq3d.h"

#include "render/ShaderProgram.h"
#include "scene/Texture.h"
#include "utils/TestUtils.h"

// control ids
enum
{
	SpinTimer = wxID_HIGHEST
};

wxString glGetwxString(GLenum name)
{
	const GLubyte* v = glGetString(name);
	if (v == nullptr)
	{
		// The error is not important. It is GL_INVALID_ENUM.
		// We just want to clear the error stack.
		glGetError();

		return wxString();
	}

	return wxString((const char*)v);
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

// function to draw the texture for cube faces
static wxImage DrawDice(int size, unsigned num)
{
	wxASSERT_MSG(num >= 1 && num <= 6, "invalid dice index");

	const int dot = size / 16;        // radius of a single dot
	const int gap = 5 * size / 32;      // gap between dots

	wxBitmap bmp(size, size);
	wxMemoryDC dc;
	dc.SelectObject(bmp);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	dc.SetBrush(*wxBLACK_BRUSH);

	// the upper left and lower right points
	if (num != 1)
	{
		dc.DrawCircle(gap + dot, gap + dot, dot);
		dc.DrawCircle(size - gap - dot, size - gap - dot, dot);
	}

	// draw the central point for odd dices
	if (num % 2)
	{
		dc.DrawCircle(size / 2, size / 2, dot);
	}

	// the upper right and lower left points
	if (num > 3)
	{
		dc.DrawCircle(size - gap - dot, gap + dot, dot);
		dc.DrawCircle(gap + dot, size - gap - dot, dot);
	}

	// finally those 2 are only for the last dice
	if (num == 6)
	{
		dc.DrawCircle(gap + dot, size / 2, dot);
		dc.DrawCircle(size - gap - dot, size / 2, dot);
	}

	dc.SelectObject(wxNullBitmap);

	return bmp.ConvertToImage();
}

ZQGLContext::ZQGLContext(wxGLCanvas* canvas) : wxGLContext(canvas)
{
	SetCurrent(*canvas);

	// set up the parameters we want to use
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);

	// add slightly more light, the default lighting is rather dark
	GLfloat ambient[] = { 0.5, 0.5, 0.5, 0.5 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// set viewing projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.5, 0.5, -0.5, 0.5, 1, 3);

	// create the textures to use for cube sides: they will be reused by all
	// canvases (which is probably not critical in the case of simple textures
	// we use here but could be really important for a real application where
	// each texture could take many megabytes)
	glGenTextures(WXSIZEOF(m_textures), m_textures);

	for (unsigned i = 0; i < WXSIZEOF(m_textures); i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		const wxImage img(DrawDice(256, i + 1));

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(),
			0, GL_RGB, GL_UNSIGNED_BYTE, img.GetData());
	}

	CheckGLError();
}

ZQGLContext::~ZQGLContext()
{
	for (unsigned i = 0; i < WXSIZEOF(m_textures); i++)
	{
		glDeleteTextures(1, &m_textures[i]);
	}
}

void ZQGLContext::DrawRotatedCube(float xangle, float yangle)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2.0f);
	glRotatef(xangle, 1.0f, 0.0f, 0.0f);
	glRotatef(yangle, 0.0f, 1.0f, 0.0f);

	// draw six faces of a cube of size 1 centered at (0, 0, 0)
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0, 0); glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1, 0); glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1, 1); glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(0, 1); glVertex3f(0.5f, -0.5f, 0.5f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_textures[1]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 0); glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0, 1); glVertex3f(0.5f, -0.5f, -0.5f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_textures[2]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0, 0); glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1, 0); glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_textures[3]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 1); glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0, 1); glVertex3f(-0.5f, -0.5f, 0.5f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_textures[4]);
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 0); glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 1); glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(0, 1); glVertex3f(0.5f, 0.5f, -0.5f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_textures[5]);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
	glEnd();

	glFlush();

	CheckGLError();
}

void ZQGLContext::DrwaTraiangle()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor((GLfloat)0.15, (GLfloat)0.15, 0.0, (GLfloat)1.0); // Dark, but not black.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// ----------------------------------------------------------------------------
// MyApp: the application object
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(ZQApp);
ZQGLContext& ZQApp::GetContext(wxGLCanvas* canvas, bool useStereo)
{
	ZQGLContext* glContext;
	if (useStereo)
	{
		if (!m_glStereoContext)
		{
			// Create the OpenGL context for the first stereo window which needs it:
			// subsequently created windows will all share the same context.
			m_glStereoContext = new ZQGLContext(canvas);
		}
		glContext = m_glStereoContext;
	}
	else
	{
		if (!m_glContext)
		{
			// Create the OpenGL context for the first mono window which needs it:
			// subsequently created windows will all share the same context.
			m_glContext = new ZQGLContext(canvas);
		}
		glContext = m_glContext;
	}

	glContext->SetCurrent(*canvas);

	return *glContext;
}

bool ZQApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	wxInitAllImageHandlers();
	new ZQFrame();

	return true;
}

int ZQApp::OnExit()
{
	delete m_glContext;
	delete m_glStereoContext;

	return wxApp::OnExit();
}

wxBEGIN_EVENT_TABLE(ZQFrame, wxFrame)
EVT_MENU(wxID_NEW, ZQFrame::OnNewWindow)
EVT_MENU(NEW_STEREO_WINDOW, ZQFrame::OnNewStereoWindow)
EVT_MENU(wxID_CLOSE, ZQFrame::OnClose)
wxEND_EVENT_TABLE()

ZQFrame::ZQFrame(bool stereoWindow) :
	wxFrame(nullptr, wxID_ANY, "wxWidgets OpenGL Cube Sample")
{
	new ZQGLCanvas(this, stereoWindow);

	SetIcon(wxICON(sample));

	wxMenu* menu = new wxMenu;
	menu->Append(wxID_NEW);
	menu->Append(NEW_STEREO_WINDOW, "New Stereo Window");
	menu->AppendSeparator();
	menu->Append(wxID_CLOSE);
	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menu, "&Cube");

	SetMenuBar(menuBar);

	CreateStatusBar();

	SetClientSize(400, 400);
	Show();

	// test IsDisplaySupported() function:
	wxGLAttributes attribs;
	attribs.RGBA().DoubleBuffer().EndList();
	wxLogStatus("Double-buffered display %s supported",
		wxGLCanvas::IsDisplaySupported(attribs) ? "is" : "not");
}

void ZQFrame::OnClose(wxCommandEvent& event)
{
	WXUNUSED(event);
	Close(true);
}

void ZQFrame::OnNewWindow(wxCommandEvent& event)
{
	WXUNUSED(event);
	new ZQFrame();
}

void ZQFrame::OnNewStereoWindow(wxCommandEvent& event)
{
	WXUNUSED(event);

	wxGLAttributes attribs;
	attribs.RGBA().DoubleBuffer().Stereo().EndList();
	if (wxGLCanvas::IsDisplaySupported(attribs))
	{
		new ZQFrame(true);
	}
	else
	{
		wxLogError("Stereo not supported by OpenGL on this system, sorry.");
	}
}


wxBEGIN_EVENT_TABLE(ZQGLCanvas, wxGLCanvas)
EVT_PAINT(ZQGLCanvas::OnPaint)
EVT_KEY_DOWN(ZQGLCanvas::OnKeyDown)
EVT_TIMER(SpinTimer, ZQGLCanvas::OnSpinTimer)
wxEND_EVENT_TABLE()

ZQGLCanvas::ZQGLCanvas(wxWindow* parent, bool useStereo)
// With perspective OpenGL graphics, the wxFULL_REPAINT_ON_RESIZE style
// flag should always be set, because even making the canvas smaller should
// be followed by a paint event that updates the entire canvas with new
// viewport settings.
	: m_xangle(30.0),
	m_yangle(30.0),
	m_spinTimer(this, SpinTimer),
	m_useStereo(useStereo),
	m_stereoWarningAlreadyDisplayed(false),
	m_glContext(nullptr),
	m_shader(nullptr),
	m_texture1(nullptr),
	m_texture2(nullptr)
{
	wxGLAttributes attribs = wxGLAttributes();
	// 必须启用下面这些属性才能使用OpenGL3.3以上的新版本
	attribs.PlatformDefaults().RGBA().DoubleBuffer().Depth(16).EndList();
	auto accepted = wxGLCanvas::IsDisplaySupported(attribs);

	if (!wxGLCanvas::Create(parent, attribs, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		wxFULL_REPAINT_ON_RESIZE))
	{
		wxLogError("Creating OpenGL window failed.");
	}
	m_glContext = new wxGLContext(this);
	m_glContext->SetCurrent(*this);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	// 检查实际版本
	const char* version = (const char*)glGetString(GL_VERSION);
	// wxLogMessage("OpenGL Version: %s", version);

	InitGL();
}

ZQGLCanvas::~ZQGLCanvas()
{
	if (m_shader)
		delete m_shader;
	if (m_texture1)
		delete m_texture1;
	if (m_texture2)
		delete m_texture2;

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);
	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);
	if (m_EBO > 0)
		glDeleteBuffers(1, &m_EBO);

	if (m_glContext)
		delete m_glContext;
}

void ZQGLCanvas::InitGL()
{
	m_glContext->SetCurrent(*this);
	glEnable(GL_DEPTH_TEST);

	//m_shader = new ShaderProgram("textureShader");
	//m_shader->LoadAndLink("resources/shader/texture.vs", "resources/shader/texture.fs");
	//m_texture = new Texture("resources/texture/wall.jpg");
	//TestUtils::BuildTestTextureGL(m_VAO, m_VBO, m_EBO);

	m_shader = new ShaderProgram("cameraShader");
	m_shader->LoadAndLink("resources/shader/camera.vs", "resources/shader/camera.fs");

	TestUtils::BuildTestCameraGL(m_VAO, m_VBO, m_EBO);

	m_texture1 = new Texture("resources/texture/container.jpg", false, false, true);
	m_texture2 = new Texture("resources/texture/awesomeface.png");
	m_shader->Use();
	m_shader->SetInt("texture1", 0);
	m_shader->SetInt("texture2", 1);
	// pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
	// -----------------------------------------------------------------------------------------------------------
	const wxSize ClientSize = GetClientSize() * GetContentScaleFactor();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)ClientSize.x / (float)ClientSize.y, 0.1f, 100.0f);
	m_shader->setMat4("projection", projection);


	CheckGLError();
}

void ZQGLCanvas::OnPaint(wxPaintEvent& event)
{
	// This is required even though dc is not used otherwise.
	wxPaintDC dc(this);

	const wxSize ClientSize = GetClientSize() * GetContentScaleFactor();

	//ZQGLContext& canvas = wxGetApp().GetContext(this, m_useStereo);
	//glViewport(0, 0, ClientSize.x, ClientSize.y);
	//canvas.DrawRotatedCube(m_xangle, m_yangle);

	SetCurrent(*m_glContext);
	glViewport(0, 0, ClientSize.x, ClientSize.y);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture1->ID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texture2->ID());

	m_shader->Use();
	// camera/view transformation
	//glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	//float radius = 10.0f;
	//float camX = static_cast<float>(sin(2) * radius);
	//float camZ = static_cast<float>(cos(14) * radius);
	//view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//m_shader->setMat4("view", view);
	m_shader->setMat4("view", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f)));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(m_xangle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(m_yangle), glm::vec3(0.0f, 1.0f, 0.0f));
	m_shader->setMat4("model", model);

	// render boxes
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//TestUtils::DrawTestTextureGL(m_shader, m_texture, m_VAO);

	SwapBuffers();
}

void ZQGLCanvas::Spin(float xSpin, float ySpin)
{
	m_xangle += xSpin;
	m_yangle += ySpin;

	Refresh(false);
}

void ZQGLCanvas::OnKeyDown(wxKeyEvent& event)
{
	float angle = 5.0;

	switch (event.GetKeyCode())
	{
	case WXK_RIGHT:
		Spin(0.0, -angle);
		break;

	case WXK_LEFT:
		Spin(0.0, angle);
		break;

	case WXK_DOWN:
		Spin(-angle, 0.0);
		break;

	case WXK_UP:
		Spin(angle, 0.0);
		break;

	case WXK_SPACE:
		if (m_spinTimer.IsRunning())
			m_spinTimer.Stop();
		else
			m_spinTimer.Start(25);
		break;

	default:
		event.Skip();
		return;
	}
}

void ZQGLCanvas::OnSpinTimer(wxTimerEvent& WXUNUSED)
{
	Spin(0.0, 4.0);
}
