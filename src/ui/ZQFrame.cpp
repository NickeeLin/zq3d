#include "ZQFrame.h"
#include "ZQGLCanvas.h"
#include <zq3d.h>
#include <render/RenderEngine.h>

wxBEGIN_EVENT_TABLE(ZQFrame, wxFrame)
//EVT_MENU(wxID_NEW, ZQFrame::OnNewWindow)
//EVT_MENU(NEW_STEREO_WINDOW, ZQFrame::OnNewStereoWindow)
EVT_MENU(wxID_CLOSE, ZQFrame::OnClose)
wxEND_EVENT_TABLE()

ZQFrame::ZQFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, "wxWidgets OpenGL Cube Sample")
{
	SetIcon(wxICON(sample));

	wxMenu* menu = new wxMenu;
	menu->Append(wxID_NEW);
	//menu->Append(NEW_STEREO_WINDOW, "New Stereo Window");
	menu->AppendSeparator();
	menu->Append(wxID_CLOSE);
	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menu, "&Cube");

	SetMenuBar(menuBar);

	CreateStatusBar();

	//SetClientSize(400, 400);
	

	// test IsDisplaySupported() function:
	//wxGLAttributes attribs;
	//attribs.RGBA().DoubleBuffer().EndList();
	//wxLogStatus("Double-buffered display %s supported",
	//	wxGLCanvas::IsDisplaySupported(attribs) ? "is" : "not");
}

void ZQFrame::SetCanvas(wxGLCanvas* canvas)
{
	//if (this->sizerMiddle->GetItemCount() > 1)
	//	this->sizerMiddle->Detach(0);

	//wxSizerItem* canvasItem = this->sizerMiddle->Prepend(canvas);

	//this->sizerMiddle->Layout();

	//RenderEngine::Canvas.Position = canvasItem->GetPosition();
}

void ZQFrame::OnClose(wxCommandEvent& event)
{
	WXUNUSED(event);
	Close(true);
}
