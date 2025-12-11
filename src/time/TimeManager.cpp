#include "TimeManager.h"
#include <render/RenderEngine.h>
#include "utils/Utils.h"
#include "ui/ZQFrame.h"

double      TimeManager::DeltaTime = 0.0;
int         TimeManager::FPS = 0;
wxStopWatch TimeManager::deltaTimer;
wxStopWatch TimeManager::totalTimer;

struct Time
{
	long Hours = 0, Minutes = 0, Seconds = 0, MilliSeconds = 0, Total = 0;

	Time(long ms)
	{
		this->Hours = (ms / 3600000);
		this->Minutes = ((ms % 3600000) / 60000);
		this->Seconds = (((ms % 3600000) % 60000) / 1000);
		this->MilliSeconds = (((ms % 3600000) % 60000) % 1000);
		this->Total = ms;
	}
};

void TimeManager::Start()
{
	TimeManager::DeltaTime = (1.0 / 60.0);
	TimeManager::FPS = 0;

	TimeManager::deltaTimer.Start();
	TimeManager::totalTimer.Start();
}

long TimeManager::TimeElapsedMS()
{
	return totalTimer.Time();
}


void TimeManager::UpdateFPS()
{
	if (TimeManager::deltaTimer.Time() >= 1000)
	{
		TimeManager::DeltaTime = (1.0 / (double)TimeManager::FPS);
		Time              time = Time(totalTimer.Time());

		std::swprintf(
			RenderEngine::Canvas.Window->Title,
			BUFFER_SIZE,
			L"%ls v.%ls - %ls %ls - %ls - %d FPS (%f dT) - %02ld:%02ld:%02ld",
			Utils::APP_NAME.c_str().AsWChar(),
			Utils::APP_VERSION.c_str().AsWChar(),
			RenderEngine::GPU.Vendor.c_str().AsWChar(),
			RenderEngine::GPU.Renderer.c_str().AsWChar(),
			RenderEngine::GPU.Version.c_str().AsWChar(),
			TimeManager::FPS,
			TimeManager::DeltaTime,
			time.Hours, time.Minutes, time.Seconds
		);

		//RenderEngine::Canvas.Window->SetTitle(RenderEngine::Canvas.Window->Title);

		TimeManager::FPS = 0;
		TimeManager::deltaTimer.Start();
	}

	TimeManager::FPS++;
}
