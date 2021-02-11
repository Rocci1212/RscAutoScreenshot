#pragma once
#pragma comment( lib, "pluginsdk.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

class RscAutoScreenshot : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	bool isEnabled;
	int screenshotDelay;
	int screenshotButton;

public:
	virtual void onLoad();
	virtual void onUnload();

	void StatsScreen(std::string eventName);
	void takeScreenshot();
};
