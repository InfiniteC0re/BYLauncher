#include "pch.h"
#include "GlobalState.h"
#include "Thread/T2AtomicMutex.h"

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

TOSHI_NAMESPACE_USING

static GlobalState s_oGlobalState;
static T2AtomicMutex s_oAtomicMutex;

TUINT32 GlobalStateAccessor::GetFlags()
{
	T2MUTEX_LOCK_SCOPE( s_oAtomicMutex );
	return s_oGlobalState.eFlags;
}

void GlobalStateAccessor::SetFlags( TUINT32 a_eFlags, TBOOL a_bAdd )
{
	T2MUTEX_LOCK_SCOPE( s_oAtomicMutex );

	if ( a_bAdd ) s_oGlobalState.eFlags |= a_eFlags;
	else s_oGlobalState.eFlags &= ~a_eFlags;
}
