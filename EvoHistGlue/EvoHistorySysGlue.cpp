// EvoHistorySysGlue.cpp
//

#include "stdafx.h"
#include <limits.h>
#include "config.h"
#include "win32_rootWindow.h"
#include "HistAllocThread.h"
#include "HistoryGeneration.h"
#include "HistorySystem.h"
#include "EvoModelFactory.h"
#include "EvolutionCore.h"
#include "EvolutionTypes.h"
#include "EvoHistorySysGlue.h"

class WorkThread;
class ObserverInterface;

GenerationCmd const EvoHistorySysGlue::NEXT_GEN_CMD = EvoHistorySysGlue::EvoCmd( tEvoCmd::nextGen, 0 );

EvoHistorySysGlue::EvoHistorySysGlue( ) :
	m_pEvoModelFactory( nullptr ),
    m_pHistorySystem  ( nullptr ),
	m_pHistAllocThread( nullptr )
{}

EvoModelDataGlue * EvoHistorySysGlue::Start
(
	HistorySystem     * const pHistorySystem,
	ObserverInterface * const pObserver
)
{
	m_pHistorySystem   = pHistorySystem;
	m_pEvoModelFactory = new EvoModelFactory( ); //ok

	ModelData * pModelWork = m_pHistorySystem->StartHistorySystem
    (
        Config::GetConfigValue( Config::tId::maxGeneration ),
        Config::GetConfigValue( Config::tId::nrOfHistorySlots ),
		Util::GetPhysicalMemory( ),
        m_pEvoModelFactory,
		pObserver,
		GenerationCmd::ApplicationCmd( static_cast< tGenCmd >( tEvoCmd::reset ), 0 )
    );

	// delegate allocation of history slots to a work thread

	m_pHistAllocThread = new HistAllocThread( m_pHistorySystem, TRUE );  //ok 

	return static_cast< EvoModelDataGlue * >( pModelWork );
}

void EvoHistorySysGlue::Stop( )
{
	m_pHistAllocThread->Terminate();
	m_pHistorySystem->StopHistorySystem( );

	delete m_pEvoModelFactory;  //ok
	delete m_pHistAllocThread;  //ok

	m_pEvoModelFactory = nullptr;
	m_pHistorySystem   = nullptr;
	m_pHistAllocThread = nullptr;
}

class FindGridPointFunctor : public GenerationProperty
{
public:

    FindGridPointFunctor( IND_ID const id ) : m_id( id ) {}

    virtual bool operator() ( ModelData const * pModelData ) const
    {
        EvoModelDataGlue const * pEvoModelData = static_cast< EvoModelDataGlue const * >( pModelData );
        return ( pEvoModelData->FindGridPointFromId( m_id ).IsNotNull( ) );  // id is alive
    }

private:
    IND_ID const m_id;
};

HIST_GENERATION EvoHistorySysGlue::GetGenWithIndividual( GridPoint const gp, bool const bReverse ) const
{
	HIST_GENERATION       gen   = m_pHistorySystem->GetCurrentGeneration( );
	EvolutionCore const * pCore = GetEvolutionCore( gen );
	IND_ID                id    = pCore->GetId( gp );
	return id.IsNull( ) 
		? HIST_GENERATION()
		: m_pHistorySystem->FindGenerationWithProperty( FindGridPointFunctor( id ), bReverse );
}

void EvoHistorySysGlue::EvoClearHistory(  ) 
{ 
    m_pHistorySystem->ApproachHistGen( 0 );
	assert( m_pHistorySystem->GetCurrentGeneration( ) == 0 );
	m_pHistorySystem->ClearHistory( 0 );
}

EvolutionCore const * EvoHistorySysGlue::GetEvolutionCore( HIST_GENERATION const gen ) const
{
	ModelData const * pModelData { GetModelData( gen ) };
	return ( pModelData == nullptr )
	? nullptr  // gen not in cache
	: (static_cast< EvoModelDataGlue const * >( pModelData ))->GetEvolutionCoreC( );
}

void EvoHistorySysGlue::EvoCreateEditorCommand( GenerationCmd cmd ) 
{ 
	m_pHistorySystem->CreateAppCommand( cmd ); 
}
