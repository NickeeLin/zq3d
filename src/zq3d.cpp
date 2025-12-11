#include "zq3d.h"
#include "ui/ZQFrame.h"
#include "ui/ZQGLCanvas.h"
#include "render/RenderEngine.h"
#include <time/TimeManager.h>
#include <utils/Utils.h>
#include <scene/SceneManager.h>
#include <scene/Model.h>

#include <crtdbg.h>

wxIMPLEMENT_APP(ZQApp);

const wxSize   UI_RENDER_SIZE = wxSize(640, 360);
const wxSize   UI_WINDOW_SIZE = wxSize(1280, 875);
const wxSize   UI_PROPS_SIZE = wxSize(590, 280);
const wxSize   UI_TABS_SIZE = wxSize(1245, 85);

void ZQApp::GameLoop(wxIdleEvent& event)
{
	if (RenderEngine::Ready)
	{
		event.RequestMore();

		RenderEngine::Canvas.Size = RenderEngine::Canvas.Canvas->GetClientSize() * RenderEngine::Canvas.Canvas->GetContentScaleFactor();
		
		TimeManager::UpdateFPS();
		RenderEngine::Draw();
	}
}

bool ZQApp::OnInit()
{
	//_CrtSetBreakAlloc(175232);

	if (!wxApp::OnInit())
		return false;

	wxInitAllImageHandlers();
	m_frame = new ZQFrame("engine", wxDefaultPosition, wxSize(1280, 875));
	m_frame->Show();

	int result = RenderEngine::Init(this->m_frame, UI_RENDER_SIZE);

	Model* mymodel = new Model("resources/model/Pikachu_Body_Parts_Polymon.stl");
	Utils::CheckGLError();
	SceneManager::AddComponent(mymodel);

	TimeManager::Start();
	this->Connect(wxEVT_IDLE, wxIdleEventHandler(ZQApp::GameLoop));

	return true;
}

int ZQApp::OnExit()
{
	RenderEngine::Close();
	return wxApp::OnExit();
}

