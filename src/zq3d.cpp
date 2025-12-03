// zq3d.cpp: 定义应用程序的入口点。
//
#include <glad/glad.h>
#include <fmt/core.h>
#include "zq3d.h"


using namespace std;

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

	if (stereoWindow)
	{
		const wxString vendor = glGetwxString(GL_VENDOR).Lower();
		const wxString renderer = glGetwxString(GL_RENDERER).Lower();
		if (vendor.find("nvidia") != wxString::npos &&
			renderer.find("quadro") == wxString::npos)
			ShowFullScreen(true);
	}
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
	m_stereoWarningAlreadyDisplayed(false)
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
	//wxLogMessage("OpenGL Version: %s", version);

	InitGL();
}

void ZQGLCanvas::InitGL()
{
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	CheckGLError();

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	CheckGLError();

	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);
	glLinkProgram(m_shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top   
	};

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	CheckGLError();
}

void ZQGLCanvas::OnPaint(wxPaintEvent& event)
{
	// This is required even though dc is not used otherwise.
	wxPaintDC dc(this);

	// Set the OpenGL viewport according to the client size of this canvas.
	// This is done here rather than in a wxSizeEvent handler because our
	// OpenGL rendering context (and thus viewport setting) is used with
	// multiple canvases: If we updated the viewport in the wxSizeEvent
	// handler, changing the size of one canvas causes a viewport setting that
	// is wrong when next another canvas is repainted.
	const wxSize ClientSize = GetClientSize() * GetContentScaleFactor();
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ZQGLContext& canvas = wxGetApp().GetContext(this, m_useStereo);
	glViewport(0, 0, ClientSize.x, ClientSize.y);
	canvas.DrawRotatedCube(m_xangle, m_yangle);

	SetCurrent(*m_glContext);
	glViewport(0, 0, ClientSize.x, ClientSize.y);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glClearColor((GLfloat)0.15, (GLfloat)0.15, 0.0, (GLfloat)1.0); // Dark, but not black.
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);

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
