#pragma once

#include <wx/wx.h>

class ZQFrame;
class ZQApp : public wxApp
{
public:
	void GameLoop(wxIdleEvent& event);
	virtual bool OnInit() override;
	virtual int OnExit() override;

private:
	ZQFrame* m_frame;
};
