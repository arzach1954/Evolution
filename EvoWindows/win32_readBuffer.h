// win32_readBuffer.h : 
//
// EvoWindows

#pragma once

#include "synchapi.h"
#include "EvolutionCore.h"
#include "observerInterface.h"

class ReadBuffer : public ObserverInterface
{
public:
	ReadBuffer( ) : 
		m_pObservers( nullptr ),
		m_pCoreWork( nullptr ),
		m_pCore4Display( nullptr )
	{
		InitializeSRWLock( & m_SRWLock );
	}

	~ReadBuffer( ) { }

	void Initialize
	(
		ObserverInterface   * pObservers,
		EvolutionCore const * pCoreWork, 
		EvolutionCore       * pCore4Display 
	)
	{
		m_pObservers    = pObservers;
		m_pCoreWork     = pCoreWork;
		m_pCore4Display = pCore4Display;
	}

	// called by consumer threads

	EvolutionCore const * LockReadBuffer( ) 
	{
		AcquireSRWLockShared( & m_SRWLock );
		return m_pCore4Display;
	}

	void ReleaseReadBuffer( )
	{
		ReleaseSRWLockShared( & m_SRWLock );
	}

	// called by producer thread

	virtual void Notify( bool const bImmediate )
	{
		if ( bImmediate )
		{
			AcquireSRWLockExclusive( & m_SRWLock );               // if locked by readers, wait
		}
		else if ( ! TryAcquireSRWLockExclusive( & m_SRWLock ))    // if buffer is locked by readers
		{                                                         // just continue your work. 
			return; 											  // readers can synchronize with
		}														  // later version

		m_pCore4Display->CopyEvolutionCoreData( m_pCoreWork );  
		ReleaseSRWLockExclusive( & m_SRWLock );                  
		m_pObservers->Notify( bImmediate );                     
	}

private:
	SRWLOCK               m_SRWLock;
	ObserverInterface   * m_pObservers;
	EvolutionCore       * m_pCore4Display;
	EvolutionCore const * m_pCoreWork;
};