#pragma once
#include "IUIScreen.h"

class ContactsScreen
	: public IUIScreen
{
public:
	ContactsScreen();
	~ContactsScreen();

	void Render() override;
};
