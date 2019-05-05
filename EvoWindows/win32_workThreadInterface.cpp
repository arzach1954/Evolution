// win32_workThreadInterface.cpp
//

#include "stdafx.h"
#include "assert.h"
#include "Resource.h"
#include "GridPoint24.h"
#include "EvolutionTypes.h"
#include "EvolutionCore.h"
#include "EvoHistorySysGlue.h"
#include "win32_worker_thread.h"
#include "win32_workThreadInterface.h"

using std::wostream;
using std::endl;

WorkThreadInterface::WorkThreadInterface( wostream * pTraceStream ) :
	m_pCore( nullptr ),
	m_pEvoHistGlue( nullptr ),
	m_pWorkThread( nullptr ),
    m_pTraceStream( pTraceStream ),
    m_bTrace( TRUE )
{ }

void WorkThreadInterface::Start
( 
	HWND                 const hwndApplication,
    ColorManager       * const pColorManager,
    PerformanceWindow  * const pPerformanceWindow,
	EditorWindow       * const pEditorWindow,
    EventInterface     * const pEvent,
    ReadBuffer         * const pReadBuffer, 
    EvolutionCore      * const pCore,
    EvoHistorySysGlue  * const pEvoHistGlue
)
{
    m_pCore        = pCore;
	m_pEvoHistGlue = pEvoHistGlue;
	m_pWorkThread  = new WorkThread
	( 
		hwndApplication, 
		pColorManager, 
		pPerformanceWindow, 
		pEditorWindow, 
		pEvent, 
		pReadBuffer, 
		pEvoHistGlue, 
		this
	);
}

WorkThreadInterface::~WorkThreadInterface( )
{
	m_pCore        = nullptr;
	m_pEvoHistGlue = nullptr;
	m_pWorkThread  = nullptr;
    m_pTraceStream = nullptr;
}

BOOL WorkThreadInterface::IsRunning( ) const
{
	return m_pWorkThread->IsRunning( );
}

void WorkThreadInterface::postGotoGeneration( HIST_GENERATION const gen )
{
    assert( gen >= 0 );

    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::GOTO_GENERATION, 0, static_cast<LPARAM>(gen.GetLong()) );
}

// procedural interface of worker thread

void WorkThreadInterface::DoProcessScript( wstring * const pwstr )
{
	m_pWorkThread->DoProcessScript( pwstr );
}

HIST_GENERATION WorkThreadInterface::GetGenDemanded( ) const 
{ 
	return m_pWorkThread->GetGenDemanded( );
}

void WorkThreadInterface::PostReset( BOOL bResetHistSys )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << (bResetHistSys ? 1 : 0) << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::RESET_MODEL, bResetHistSys, 0 );
}

void WorkThreadInterface::PostBenchmark( int const iNrOfGenerations )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << iNrOfGenerations <<endl;
//	HIST_GENERATION gen = m_pEvoHistGlue->GetCurrentGeneration( );
//	postGotoGeneration( gen + iNrOfGenerations );
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::BENCHMARK, 0, iNrOfGenerations );
}

void WorkThreadInterface::PostRefresh( LPARAM const lParam )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::REFRESH, 0, lParam );
}

void WorkThreadInterface::PostSetPOI( GridPoint const gp )
{
    if ( IsInGrid( gp ) )
    {
        if ( m_bTrace )
            * m_pTraceStream << __func__ << L" " << gp << endl;
        m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_POI, gp.GetXvalue(), gp.GetYvalue() );
    }
}

void WorkThreadInterface::PostDoEdit( GridPoint const gp )
{
    if ( IsInGrid( gp ) )
    {
        if ( m_bTrace )
            * m_pTraceStream << __func__ << L" " << gp << endl;
        m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::DO_EDIT, gp.GetXvalue(), gp.GetYvalue() );
    }
}

void WorkThreadInterface::PostSetBrushIntensity( PERCENT const intensity )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << intensity.GetValue() << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_BRUSH_INTENSITY, intensity.GetValue(), 0 );
}

void WorkThreadInterface::PostSetBrushRadius( GRID_COORD const radius )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << radius.GetValue() << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_BRUSH_RADIUS, radius.GetValue(), 0 );
}

void WorkThreadInterface::PostSetBrushMode( tBrushMode const mode )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << GetBrushModeName( mode ) << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_BRUSH_MODE, static_cast<WPARAM>( mode ), 0 );
}

void WorkThreadInterface::PostSetSimulationMode( tBoolOp const op )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << GetBoolOpName( op ) << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_SIMULATION_MODE, static_cast<WPARAM>( op ), 0 );
}

void WorkThreadInterface::PostSetBrushShape( tShape const shape )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << GetShapeName( shape ) << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_BRUSH_SHAPE, static_cast<WPARAM>( shape ), 0 );
}

void WorkThreadInterface::PostSetBrushManipulator( tManipulator const op )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" tManipulator::" << GetManipulatorName( op ) << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_BRUSH_OPERATOR, static_cast<WPARAM>( op ), 0 );
}

void WorkThreadInterface::PostSetColor( COLORREF const col, tColorObject const obj, Strategy::Id const strat )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << GetColorObjectName( obj ) << endl;
	switch ( obj )
	{
	case tColorObject::individual:
	    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_STRATEGY_COLOR, static_cast<WPARAM>( strat ), static_cast<LPARAM>( col ) );
		break;
	case tColorObject::selection:
	    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_SELECTION_COLOR, 0, static_cast<LPARAM>( col ) );
		break;
	case tColorObject::highlight:
	    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::SET_HIGHLIGHT_COLOR, 0, static_cast<LPARAM>( col ) );
		break;
	default:
		assert( false );
	}
}

void WorkThreadInterface::PostRunGenerations( bool const bFirst )
{
    if ( m_bTrace )
        * m_pTraceStream << L"PostGenerationStep" << endl;
	m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::GENERATION_RUN, 0, bFirst );
}

void WorkThreadInterface::PostRedo( )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << endl;

	HIST_GENERATION gen = m_pEvoHistGlue->GetCurrentGeneration( );

	if ( ( gen < m_pEvoHistGlue->GetYoungestGeneration( ) ) && m_pEvoHistGlue->IsEditorCommand( gen + 1 ) )
		postGotoGeneration( gen + 1 );
	else
		(void)MessageBeep(MB_OK);  // first generation reached
}

void WorkThreadInterface::PostGenerationStep( )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << endl;
	postGotoGeneration( m_pEvoHistGlue->GetCurrentGeneration( ) + 1 );
}

void WorkThreadInterface::PostRepeatGenerationStep( )
{
    if ( m_bTrace )
        * m_pTraceStream << L"PostGenerationStep" << endl;
    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::REPEAT_GENERATION_STEP, 0, 0 );
}

void WorkThreadInterface::PostUndo( )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << endl;

	HIST_GENERATION gen = m_pEvoHistGlue->GetCurrentGeneration( );

	if ( ( gen > 0 ) && m_pEvoHistGlue->IsEditorCommand( gen - 1 ) )
		postGotoGeneration( gen - 1 );
	else
		(void)MessageBeep(MB_OK);  // first generation reached
}

void WorkThreadInterface::PostPrevGeneration( )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << endl;
	if (m_pEvoHistGlue->GetCurrentGeneration() > 0)
		postGotoGeneration( m_pEvoHistGlue->GetCurrentGeneration() - 1 );
	else
		(void)MessageBeep(MB_OK);  // first generation reached
}

void WorkThreadInterface::PostHistoryAction( UINT const uiID, GridPoint const gp )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << uiID << L" " << gp << endl;

	assert( m_pCore->IsAlive(gp) );
	assert( (uiID == IDM_GOTO_ORIGIN) || (uiID == IDM_GOTO_DEATH) );

	postGotoGeneration( m_pEvoHistGlue->GetGenWithIndividual( m_pCore->GetId(gp), uiID == IDM_GOTO_DEATH ) );
}

void WorkThreadInterface::PostGotoGeneration( HIST_GENERATION const gen )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << gen << endl;
	postGotoGeneration( gen );
}

void WorkThreadInterface::PostStopComputation( )
{
	m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::STOP, 0, 0 );
}

void WorkThreadInterface::PostProcessScript( wstring const & wstrPath )
{
//lint -esym( 429, pwstr )     not freed here, happens in worker thread
    wstring * const pwstr = new wstring( wstrPath );
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" \"" << wstrPath.c_str( )  << "\""<< endl;

    m_pWorkThread->WorkMessage( WorkerThreadMessage::Id::PROCESS_SCRIPT, 0, (LPARAM)pwstr );
}

// no trace output

void WorkThreadInterface::TerminateThread( )
{
	m_pWorkThread->Terminate( );    // wait until thread has stopped
}
