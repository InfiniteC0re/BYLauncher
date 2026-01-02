#pragma once
#include "IUIScreen.h"

class MainScreen
    : public IUIScreen
{
public:
	MainScreen();
	~MainScreen();

	void Render() override;
	void Update( TFLOAT flDeltaTime ) override;

	void EnableGameButtons( TBOOL bEnable );

private:
	void Button_PlayGame();
	void Button_Settings();
	void Button_Mods();
	void Button_Contacts();

private:
	TBOOL m_bEnableGameButtons = TFALSE;
};
