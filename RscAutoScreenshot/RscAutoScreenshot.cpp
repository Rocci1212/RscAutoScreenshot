// RscAutoScreenshot.cpp : Defines the exported functions for the DLL application.
//

#include "RscAutoScreenshot.h"
#include <chrono>
#include <time.h>

BAKKESMOD_PLUGIN(RscAutoScreenshot, "RSC Auto Screenshot", "1.2", 0)

// make sure you're building the x64 release version
// right now, to enable the plugin, open up the BM console with f6 and type: plugin load rscautoscreenshot
void RscAutoScreenshot::onLoad()
{
	// settings
	cvarManager->registerCvar("autoscreenshot_enabled", "1", "Enable or Disable the Auto Screenshot Plugin", true, true, 0, true, 4, true);
	cvarManager->registerCvar("screenshot_delay", "15", "How long to wait before taking screenshot", true, true, 0.0f, true, 60.0f, true);
	cvarManager->registerCvar("screenshot_button", "0", "Keyboard button to send for screenshot", true, true, 0, true, 3, true);

	// called when game ends
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnMatchWinnerSet", bind(&RscAutoScreenshot::StatsScreen, this, std::placeholders::_1));
}

bool RscAutoScreenshot::isRSCMatchTime()
{
	// constants for weekdays
	const int TUESDAY = 2;
	const int WEDNESDAY = 3;
	const int THURSDAY = 4;
	const int FRIDAY = 5;

	#pragma region Get EST Time

	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	//gmtime(&rawtime);rawtime
	//gmtime_s(&timeinfo, &rawtime);

	time(&rawtime);
	gmtime_s(&timeinfo, &rawtime);

	strftime(buffer, 80, "%F %R %z", &timeinfo);
	//cvarManager->log("Date " + std::string(buffer));

	//cvarManager->log("Day: " + std::to_string((&timeinfo)->tm_wday));
	//cvarManager->log("Hour: " + std::to_string((&timeinfo)->tm_hour));

	int dstOffset = getDSTOffset();
	int EST09PM = 1 + dstOffset;
	int EST10PM = 2 + dstOffset;
	int EST11PM = 3 + dstOffset;

	#pragma endregion

	float screenshotMode = cvarManager->getCvar("autoscreenshot_enabled").getFloatValue();

	if (screenshotMode == 1.00f)
		return true;
	else if (screenshotMode == 2.00f || screenshotMode == 5.00f || screenshotMode == 6.00f || screenshotMode == 8.00f)
	{
		// Match days are 10 EST 
		if ((&timeinfo)->tm_wday == TUESDAY || (&timeinfo)->tm_wday == THURSDAY) {
			if ((&timeinfo)->tm_hour >= EST10PM && (&timeinfo)->tm_hour < EST11PM) {
				return true;
			}
		}
	}
	else if (screenshotMode == 4.00f || screenshotMode == 6.00f || screenshotMode == 7.00f || screenshotMode == 8.00f) // 1s nights
	{
		// Match days in UTC/GM are Tuesdays and Thursdays from 3am-4am
		if ((&timeinfo)->tm_wday == FRIDAY) {
			if ((&timeinfo)->tm_hour >= EST10PM && (&timeinfo)->tm_hour < EST11PM) {
				return true;
			}
		}
	}
	else if (screenshotMode == 3.00f || screenshotMode == 5.00f || screenshotMode == 7.00f || screenshotMode == 8.00f) // 2s nights
	{
		// Match days in UTC/GM are Wednesdays from 2:30am-4am
		if ((&timeinfo)->tm_wday == WEDNESDAY) {
			if (((&timeinfo)->tm_hour >= EST10PM || ((&timeinfo)->tm_hour >= EST09PM && (&timeinfo)->tm_min >= 30.00f)) && (&timeinfo)->tm_hour < EST11PM) {
				return true;
			}
		}
	}

	return false;
}

void RscAutoScreenshot::StatsScreen(std::string eventName)
{
	if (!isRSCMatchTime())
		return;

	// else, enabled all the time
	screenshotDelay = cvarManager->getCvar("screenshot_delay").getFloatValue();

	// take a screenshot after 10 seconds
	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		this->takeScreenshot();
		}, screenshotDelay);
}

int RscAutoScreenshot::getDSTOffset()
{
	char marval[80];
	char novval[80];
	char curval[80];

	// get second sunday of march of the year parameter
	time_t marchtime;
	struct tm marchtimeinfo;

	time(&marchtime);
	gmtime_s(&marchtimeinfo, &marchtime);

	(&marchtimeinfo)->tm_mon = 2;
	(&marchtimeinfo)->tm_mday = 8;
	mktime(&marchtimeinfo);

	if (&marchtimeinfo.tm_wday != 0)
	{
		(&marchtimeinfo)->tm_mday = (&marchtimeinfo)->tm_mday + (7 - (&marchtimeinfo)->tm_wday);
		mktime(&marchtimeinfo);
	}

	strftime(marval, 80, "%F %R %w", &marchtimeinfo);
	//cvarManager->log("March Date " + std::string(marval));
	
	// get second sunday of november of the year parameter
	time_t novembertime;
	struct tm novembertimeinfo;

	time(&novembertime);
	gmtime_s(&novembertimeinfo, &novembertime);

	(&novembertimeinfo)->tm_mon = 10;
	(&novembertimeinfo)->tm_mday = 1;
	mktime(&novembertimeinfo);

	if (&novembertimeinfo.tm_wday != 0)
	{
		(&novembertimeinfo)->tm_mday = (&novembertimeinfo)->tm_mday + (7 - (&novembertimeinfo)->tm_wday);
		mktime(&novembertimeinfo);
	}

	strftime(novval, 80, "%F %R %w", &novembertimeinfo);
	//cvarManager->log("November Date " + std::string(novval));
	
	time_t rawtime;
	struct tm timeinfo;

	//gmtime(&rawtime);rawtime
	//gmtime_s(&timeinfo, &rawtime);

	time(&rawtime);
	gmtime_s(&timeinfo, &rawtime);

	//(&timeinfo)->tm_mon = 8;
	//mktime(&timeinfo);

	strftime(curval, 80, "%F %R %w", &timeinfo);

	cvarManager->log("Current time: " + std::string(curval));
	cvarManager->log("March time: " + std::string(marval));
	cvarManager->log("November time: " + std::string(novval));

	if (std::strcmp(curval, marval) >= 0 && std::strcmp(curval, novval) < 0)
	{
		//cvarManager->log("returning true");
		return 0;
	}
	else
	{
		//cvarManager->log("returning false");
		return 1;
	}
}

void RscAutoScreenshot::takeScreenshot()
{
	screenshotButton = cvarManager->getCvar("screenshot_button").getFloatValue();
	if (screenshotButton == 3.00f)
	{
		// press and release print screen (nice for debugging when you don't want to save anything)
		keybd_event(VK_SNAPSHOT, 0x2C, 0, 0);
		keybd_event(VK_SNAPSHOT, 0x2C, KEYEVENTF_KEYUP, 0);
	}
	else if (screenshotButton == 2.00f)
	{
		// press and release Win-G / Win-Alt-print Screen
		keybd_event(VK_LWIN, 0x5B, 0, 0);
		keybd_event(VkKeyScan('G'), 0, 0, 0);
		keybd_event(VkKeyScan('G'), 0, KEYEVENTF_KEYUP, 0);
		keybd_event(VK_LWIN, 0x5B, KEYEVENTF_KEYUP, 0);

		// win-alt-print screen to take screenshot
		keybd_event(VK_LWIN, 0x5B, 0, 0);
		keybd_event(VK_MENU, 0, 0, 0);
		keybd_event(VK_SNAPSHOT, 0, 0, 0);
		keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(VK_LWIN, 0x5B, KEYEVENTF_KEYUP, 0);
	}
	else if (screenshotButton == 1.00f)
	{
		// press and release f9, gyg's default screenshot button
		keybd_event(VK_F9, 0x78, 0, 0);
		keybd_event(VK_F9, 0x78, KEYEVENTF_KEYUP, 0);
	}
	else
	{
		// press and release f12, steam's default screenshot button
		keybd_event(VK_F12, 0x7B, 0, 0);
		keybd_event(VK_F12, 0x7B, KEYEVENTF_KEYUP, 0);
	}
}

void RscAutoScreenshot::onUnload()
{
	// do i need this? it's on RankViewer even though there's no event hooked to this
	gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded");
}
