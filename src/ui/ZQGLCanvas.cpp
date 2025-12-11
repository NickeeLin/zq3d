#include "ZQGLCanvas.h"

#include "render/ShaderProgram.h"
#include "scene/Texture.h"
#include "utils/TestUtils.h"
#include "scene/Camera.h"
#include "render/ShaderManager.h"
#include "render/RenderEngine.h"
#include <scene/Model.h>
#include <scene/SceneManager.h>
#include <utils/Utils.h>

// control ids
enum
{
	SpinTimer = wxID_HIGHEST
};

wxBEGIN_EVENT_TABLE(ZQGLCanvas, wxGLCanvas)
//EVT_PAINT(ZQGLCanvas::OnPaint)
EVT_KEY_DOWN(ZQGLCanvas::OnKeyDown)
EVT_TIMER(SpinTimer, ZQGLCanvas::OnSpinTimer)
EVT_MOUSE_EVENTS(ZQGLCanvas::OnMouse)
wxEND_EVENT_TABLE()

ZQGLCanvas::ZQGLCanvas(wxFrame* parent)
	: m_xangle(30.0),
	m_yangle(30.0),
	m_spinTimer(this, SpinTimer),
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
		RenderEngine::Canvas.Position, RenderEngine::Canvas.Size,
		wxFULL_REPAINT_ON_RESIZE))
	{
		wxLogError("Creating OpenGL window failed.");
	}
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

void ZQGLCanvas::TestInitGL()
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

	Utils::CheckGLError();

	m_camera = new Camera();
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

	m_camera->UpdateProjection();

	m_shader->setMat4("projection", m_camera->Projection());
	m_shader->setMat4("view", m_camera->View());

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
}

void ZQGLCanvas::OnKeyDown(wxKeyEvent& event)
{
	float angle = 5.0;

	RenderEngine::CameraMain->InputKeyboard(event.GetKeyCode());

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

void ZQGLCanvas::OnMouse(wxMouseEvent& event)
{
	event.Skip();
	static MouseState m_mouseState;
	if (event.LeftIsDown() && RenderEngine::CameraMain) {
		if (!event.Dragging()) {
			m_mouseState.Position = event.GetPosition();
		}
		else {
			RenderEngine::CameraMain->InputMouseMove(event, m_mouseState);
			m_mouseState.Position = event.GetPosition();
		}
	}
	else if (event.GetEventType() == wxEVT_MOUSEWHEEL) {
		RenderEngine::CameraMain->InputMouseScroll(event);
	}

}

void ZQGLCanvas::OnSpinTimer(wxTimerEvent& WXUNUSED)
{
	Spin(0.0, 4.0);
}