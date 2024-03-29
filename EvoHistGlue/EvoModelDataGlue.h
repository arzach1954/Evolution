// EvoModelDataGlue.h
//
// Glue code for adapting EVOLUTION to HISTORY system
//
// EvoModelDataGlue is ** below ** HISTORY (is called by HISTORY)
//

#pragma once

#include "gridRect.h"
#include "GridDimensions.h"
#include "ModelData.h"
#include "EvolutionCore.h"
#include "win32_stopwatch.h"

class EvoModelDataGlue: public ModelData
{
public:
	
    EvoModelDataGlue( )
    { 
		m_pEvolutionCore = EvolutionCore::CreateCore( );
	}

	~EvoModelDataGlue( )
	{
		EvolutionCore::DestroyCore( m_pEvolutionCore );
	}

    EvoModelDataGlue & operator= ( EvoModelDataGlue const & );  // noncopyable class 

	EvolutionCore * GetEvolutionCore( )
	{
		return m_pEvolutionCore;
	}

	EvolutionCore const * GetEvolutionCoreC( ) const
	{
		return m_pEvolutionCore;
	}

	virtual BYTES GetModelSize( ) const
	{
		return m_pEvolutionCore->GetCoreSize( ) + BYTES(sizeof(EvoModelDataGlue));
	}

	virtual void CopyFrom( ModelData const * const src )
	{
//		stopwatch.Start();
		m_pEvolutionCore->CopyEvolutionCoreData( static_cast< EvoModelDataGlue const * const >( src )->m_pEvolutionCore );
//		stopwatch.Stop( L"Copy model" );
	}

    GridPoint FindGridPointFromId( IND_ID const id ) const
	{ 
		return m_pEvolutionCore->FindGridPointFromId( id );
	}

	virtual void OnAppCommand( GenerationCmd const );

private:
   static Stopwatch stopwatch;

   EvolutionCore * m_pEvolutionCore;
};
