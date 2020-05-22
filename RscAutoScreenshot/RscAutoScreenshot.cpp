// RscAutoScreenshot.cpp : Defines the exported functions for the DLL application.
//

#include "RscAutoScreenshot.h"

BAKKESMOD_PLUGIN(RscAutoScreenshot, "RSC Auto Screenshot", "0.1", 0)

// make sure you're building the x64 release version
// right now, to enable the plugin, open up the BM console with f6 and type: plugin load rscautoscreenshot
void RscAutoScreenshot::onLoad()
{
	// called when game ends
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnMatchWinnerSet", bind(&RscAutoScreenshot::StatsScreen, this, std::placeholders::_1));
}

void RscAutoScreenshot::StatsScreen(std::string eventName)
{
	// eventually let user enable and disable outside of using the console
	//isEnabled = cvarManager->getCvar("rankviewer_enabled").getBoolValue();
	//if (!isEnabled) {
	//	return;
	//}

	// take a screenshot after 10 seconds
	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		this->takeScreenshot();
	}, 10.00f); // ten seconds is a rough estimate of when the game ends and when the screenshot happens - put this as a config option
}

void RscAutoScreenshot::takeScreenshot()
{
	// press and release f12, steam's default screenshot button
	keybd_event(VK_F12, 0x7B, 0, 0);
	keybd_event(VK_F12, 0x7B, KEYEVENTF_KEYUP, 0);
}

void RscAutoScreenshot::onUnload()
{
	// do i need this? it's on RankViewer even though there's no event hooked to this
	gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded");
}
