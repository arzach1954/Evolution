// HistAllocThread.h
//

#pragma once

#include "HistorySystem.h"
#include "win32_thread.h"

class HistorySystem;

int MyAllocHook
(
	int allocType, 
	void *userData, 
	size_t size,
	int blockType, 
	long requestNumber,
	const unsigned char *filename, 
	int lineNumber
)
{
	return true;
}

class HistAllocThread: public Util::Thread
{
public:

	HistAllocThread::HistAllocThread
	( 
		HistorySystem const * const pHistSys,
		BOOL                  const bAsync      
	) :
	m_pHistorySys( pHistSys )
	{
		if ( bAsync )
		{
			StartThread( L"HistAlloc" );  
		}
		else
		{
			while (m_pHistorySys->AddHistorySlot()) {}
		}
	_CrtSetAllocHook( MyAllocHook );
	}

	~HistAllocThread() {};

	void HistAllocThread::ThreadStartupFunc( )
	{
		ThreadMsgDispatcher( MSG{0,0,0,0} );
	}

	void HistAllocThread::ThreadMsgDispatcher( MSG msg )
	{
		if ( m_pHistorySys->AddHistorySlot() ) 
			PostThreadMsg( msg );
	}

private:
	HistorySystem const * m_pHistorySys;
};
