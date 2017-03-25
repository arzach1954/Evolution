// win32_evoHistWindow.cpp
//

#include "stdafx.h"
#include "config.h"
#include "EvolutionCore.h"
#include "EvoHistorySys.h"
#include "win32_focusPoint.h"
#include "win32_histWorkerThread.h"
#include "win32_evoHistWindow.h"

EvoHistWindow::EvoHistWindow( ) :
    HistWindow( ),
    m_pHistWorkThread( nullptr ),
    m_pFocusPoint( nullptr )
{ }

EvoHistWindow::~EvoHistWindow( )
{
    m_pHistWorkThread = nullptr;
	m_pFocusPoint = nullptr;
}

void EvoHistWindow::Start
(
    HWND                 const hWndParent,
    FocusPoint         * const pFocusPoint,
	EvoHistorySys      * const pEvoHistorySys,
	HistWorkThread     * const pHistWorkThread
)
{
    Config::tOnOffAuto const displayMode = static_cast<Config::tOnOffAuto>( Config::GetConfigValue( Config::tId::historyDisplay ) );
    BOOL               const bShow       = ( displayMode == Config::tOnOffAuto::on );

    HistWindow::Start( hWndParent, pEvoHistorySys->GetHistorySystem( ) );
	m_pHistWorkThread = pHistWorkThread;
	m_pFocusPoint     = pFocusPoint;
    m_pFocusPoint->AttachFocusPointObserver( this, 75 );
    Show( bShow );
}

void EvoHistWindow::DoPaint( HDC const hDC )
{
    PaintAllGenerations( hDC );

    PaintHighlightGenerations( hDC, m_pHistWorkThread->GetGenDemanded( ) );

    if ( m_pFocusPoint->IsInGrid( ) && m_pFocusPoint->IsAlive( ) )
    {
		PaintLifeLine( hDC, m_pFocusPoint->GetGenBirth( ), m_pFocusPoint->GetGenDeath( ) );
	}
}
