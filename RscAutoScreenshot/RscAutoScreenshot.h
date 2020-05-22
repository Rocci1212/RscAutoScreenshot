#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

class RscAutoScreenshot : public BakkesMod::Plugin::BakkesModPlugin
{
	virtual void onLoad();
	virtual void onUnload();

	void StatsScreen(std::string eventName);
	void takeScreenshot();
	void Render(CanvasWrapper canvas);
};
