#pragma once

struct GlobalState
{
	enum Flag : TUINT32
	{
		kFlag_None = 0,
		kFlag_CheckingUpdates = BITFLAG( 0 ),
		kFlag_Updating = BITFLAG( 1 ),
		kFlag_Updated = BITFLAG( 2 ),
		kFlag_UpdateFailed = BITFLAG( 3 ),
	};

	TUINT32 eFlags = kFlag_None;
};

struct GlobalStateAccessor
{
	TUINT32 GetFlags();
	void SetFlags( TUINT32 a_eFlags, TBOOL a_bAdd );
};
