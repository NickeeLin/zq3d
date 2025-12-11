#ifndef ZQFRAME_H
#define ZQFRAME_H

#include "header/globals.h"

class ZQFrame : public wxFrame
{
public:
	ZQFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	wchar_t     Title[BUFFER_SIZE];

	void SetCanvas(wxGLCanvas* canvas);
private:
	void OnClose(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

#endif