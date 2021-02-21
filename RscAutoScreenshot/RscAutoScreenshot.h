#pragma once
#pragma comment( lib, "pluginsdk.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

class RscAutoScreenshot : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	bool isEnabled;
	bool isEnabledForMatchNight;
	int screenshotDelay;
	int screenshotButton;
	int getDSTOffset();

public:
	virtual void onLoad();
	bool isRSCMatchTime();
	virtual void onUnload();

	void StatsScreen(std::string eventName);
	void takeScreenshot();
};
