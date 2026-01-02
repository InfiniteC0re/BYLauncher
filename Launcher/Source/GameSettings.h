#pragma once

struct GameSettingsProperties
{
	TBOOL bExperimental     = TFALSE;
	TBOOL bFun              = TFALSE;
	TBOOL bWindowed         = TFALSE;
	TBOOL bEnableController = TFALSE;
	TBOOL bDXVK             = TTRUE;
	TINT  iWidth            = 800;
	TINT  iHeight           = 600;
};

inline struct GameSettings
    : public GameSettingsProperties
{

	GameSettings();
	~GameSettings();

	void Save();
	void Load();

	void Apply();

} g_oSettings;
