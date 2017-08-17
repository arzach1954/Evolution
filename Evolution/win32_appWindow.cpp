// win32_appWindow.cpp
//

#include "stdafx.h"

// EvolutionCore interfaces

#include "config.h"
#include "gridRect.h"
#include "wrappers.h"
#include "EvolutionModelData.h"
#include "EvolutionCore.h"

// history system

#include "EvoHistorySys.h"
#include "win32_histWorkerThread.h"

// interfaces of various windows

#include "win32_gridWindow.h"
#include "win32_evoHistWindow.h"
#include "win32_editor.h"
#include "win32_displayOptions.h"
#include "win32_appWindow.h"
#include "win32_status.h"
#include "win32_statistics.h"
#include "win32_crsrWindow.h"
#include "win32_performanceWindow.h"
#include "win32_aboutBox.h"

// infrastructure

#include "win32_winManager.h"
#include "win32_worker_thread.h"
#include "win32_focusPoint.h"
#include "win32_displayAll.h"

// scripting and tracing

#include "script.h"
#include "errhndl.h"
#include "win32_wrappers.h"
#include "win32_editorWrappers.h"
#include "win32_histWrappers.h"

// system and resources

#include "d3d_system.h"
#include "Resource.h"

// application

#include "EvoModelData.h"
#include "EvoModelFactory.h"
#include "EvoController.h"
#include "win32_appWindow.h"

class ScriptHook : public Script_Functor
{
public:

    explicit ScriptHook( StatusBar * const pStatusBar ) : m_pStatusBar( pStatusBar ) {};

    virtual void operator() ( Script & script ) const
    {
        if ( m_pStatusBar != nullptr )
        {
            wstring const & wszPath = script.GetActPath( );
            wstring const & wszLine = script.GetActLine( );
            int     const   iLineNr = script.GetActLineNr( );
            m_pStatusBar->DisplayScriptLine( wszPath, iLineNr, wszLine );
        }
    }

private:

    StatusBar * m_pStatusBar;
};

AppWindow::AppWindow( ) :
    BaseWindow( ),
    m_displayGridFunctor( ),
    m_pMainGridWindow( nullptr ),
    m_pMiniGridWindow( nullptr ),
    m_pWorkThread( nullptr ),
    m_pPerfWindow( nullptr ),
    m_pEditorWindow( nullptr ),
    m_pCrsrWindow( nullptr ),
    m_pStatusBar( nullptr ),
    m_pStatistics( nullptr ),
    m_pDspOptWindow( nullptr ),
    m_pGridRectSel( nullptr ),
    m_pFocusPoint( nullptr ),
    m_pWinManager( nullptr ),
    m_pModelWork( nullptr ),
    m_pEvolutionCore( nullptr ),
    m_pScriptHook( nullptr ),
    m_pEvoHistWindow( nullptr ),
    m_pHistWorkThread( nullptr ),
	m_pEvoHistorySys( nullptr ),
	m_pEvoController( nullptr ),
    m_traceStream( )
{};

void AppWindow::Start( HINSTANCE const hInstance, LPTSTR const lpCmdLine )
{
    HWND const hWndApp = StartBaseWindow( nullptr, CS_HREDRAW | CS_VREDRAW, L"ClassAppWindow", WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN );

    SendMessage( WM_SETICON, ICON_BIG,   (LPARAM)LoadIcon( hInstance, MAKEINTRESOURCE( IDI_EVOLUTION ) ) );
    SendMessage( WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon( hInstance, MAKEINTRESOURCE( IDI_SMALL     ) ) );

    m_traceStream.open( L"main_trace.out", ios::out );
    assert( m_traceStream.good( ) );

	ScriptErrorHandler::ScrSetOutputStream( & wcout );

	Config::SetDefaultConfiguration( );
    Config::DefineConfigWrapperFunctions( );

    // evaluate command line parameters

    std::wstring const wstrCmdLine( lpCmdLine );
    if ( wstrCmdLine.compare( L"/nohist" ) == 0 )
        Config::SetConfigValue( Config::tId::maxGeneration, 0 );

	Script::ProcessScript( L"std_configuration.in" );

	EvolutionCore::InitClass( );

    m_pEvolutionCore = EvolutionCore::CreateCore( );

    D3dSystem::Create( hWndApp, GridPoint::GRID_WIDTH, GridPoint::GRID_HEIGHT );
	
    // create window objects

    m_pGridRectSel    = new GridRect( );  
    m_pFocusPoint     = new FocusPoint( );                          
    m_pWinManager     = new WinManager( );                          
    m_pStatusBar      = new StatusBar( );       
    m_pCrsrWindow     = new CrsrWindow( );   
    m_pDspOptWindow   = new DspOptWindow( );       
    m_pEditorWindow   = new EditorWindow( );    
    m_pStatistics     = new StatisticsWindow( );   
    m_pMainGridWindow = new GridWindow( );   
    m_pMiniGridWindow = new GridWindow( );   
    m_pPerfWindow     = new PerformanceWindow( );  
	m_pEvoController  = new EvoController( );

    SetMenu( hWndApp, LoadMenu( hInstance, MAKEINTRESOURCE( IDC_EVOLUTION_MAIN ) ) );
    {
        static int const MAX_LOADSTRING = 100;
        static TCHAR szTitle[ MAX_LOADSTRING ];			// Titelleistentext
        (void)LoadString( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
        SetWindowText( hWndApp, szTitle );
    }

    m_pModelWork = EvolutionModelData::CreateModelData( );
    DefineModelWrapperFunctions( m_pModelWork );

    if ( Config::UseHistorySystem( ) )
    {
		m_pEvoHistorySys  = new EvoHistorySys( );
		m_pHistWorkThread = new HistWorkThread( & m_traceStream, m_pModelWork, m_pEvoHistorySys );
		m_pWorkThread     = m_pHistWorkThread;
		m_pEvoHistorySys->Start( m_pModelWork, m_pHistWorkThread, m_pStatusBar, EvolutionCore::GetModelSize( ) );
        DefineWin32HistWrapperFunctions( m_pHistWorkThread );

        m_pEvoHistWindow = new EvoHistWindow( );
		m_pEvoHistWindow->Start( hWndApp, m_pFocusPoint, m_pEvoHistorySys, m_pHistWorkThread );
        m_pWinManager->AddWindow( L"IDM_HIST_WINDOW", IDM_HIST_WINDOW, m_pEvoHistWindow, FALSE, 75 );
    }
    else
    {
        m_pWorkThread = new WorkThread( & m_traceStream );
        EnableMenuItem( GetMenu( hWndApp ), IDM_BACKWARDS, MF_GRAYED );
    }

	m_pFocusPoint    ->Start( m_pEvoHistorySys, m_pModelWork );
	m_pWorkThread    ->Start( m_pStatusBar, m_pEditorWindow, m_pPerfWindow, & m_displayGridFunctor, m_pEvolutionCore, m_pModelWork );
	m_pDspOptWindow  ->Start( hWndApp, m_pWorkThread,    m_pModelWork );
    m_pEditorWindow  ->Start( hWndApp, m_pWorkThread,    m_pModelWork, m_pDspOptWindow );
    m_pStatusBar     ->Start( hWndApp, m_pEvoController, m_pModelWork );
    m_pMainGridWindow->Start( hWndApp, m_pWorkThread,    m_pGridRectSel, m_pEditorWindow, m_pFocusPoint, m_pDspOptWindow, m_pPerfWindow, m_pStatusBar, m_pEvolutionCore, m_pModelWork, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 16 );
    m_pMiniGridWindow->Start( hWndApp, m_pWorkThread,    m_pGridRectSel, m_pEditorWindow, m_pFocusPoint, m_pDspOptWindow, m_pPerfWindow, m_pStatusBar, m_pEvolutionCore, m_pModelWork, WS_POPUPWINDOW | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CAPTION, 1 );
    m_pStatistics    ->Start( hWndApp, m_pModelWork,     m_pGridRectSel );
    m_pCrsrWindow    ->Start( hWndApp, m_pFocusPoint,    m_pModelWork );
    m_pPerfWindow    ->Start( hWndApp, 100 );
	m_pEvoController ->Start( & m_traceStream, m_pHistWorkThread, m_pWinManager, m_pPerfWindow, m_pStatusBar, m_pMainGridWindow );

    m_pWinManager->AddWindow( L"IDM_APPL_WINDOW", IDM_APPL_WINDOW, this,              TRUE,  -1 );
    m_pWinManager->AddWindow( L"IDM_DISP_WINDOW", IDM_DISP_WINDOW, m_pDspOptWindow,   TRUE,  -1 );
    m_pWinManager->AddWindow( L"IDM_EDIT_WINDOW", IDM_EDIT_WINDOW, m_pEditorWindow,   TRUE,  -1 );
    m_pWinManager->AddWindow( L"IDM_CRSR_WINDOW", IDM_CRSR_WINDOW, m_pCrsrWindow,     TRUE, 500 );
    m_pWinManager->AddWindow( L"IDM_STAT_WINDOW", IDM_STAT_WINDOW, m_pStatistics,     TRUE, 500 );
    m_pWinManager->AddWindow( L"IDM_PERF_WINDOW", IDM_PERF_WINDOW, m_pPerfWindow,     TRUE, 500 );
    m_pWinManager->AddWindow( L"IDM_MINI_WINDOW", IDM_MINI_WINDOW, m_pMiniGridWindow, TRUE, 300 );
    m_pWinManager->AddWindow( L"IDM_MAIN_WINDOW", IDM_MAIN_WINDOW, m_pMainGridWindow, TRUE, 100 );

    m_pMiniGridWindow->Observe( m_pMainGridWindow );
    m_pMiniGridWindow->Size( );

    m_displayGridFunctor.SetWinManager( m_pWinManager );

    m_pEvolutionCore->SetGridDisplayFunctor( & m_displayGridFunctor );   // display callback for core

    m_pScriptHook = new ScriptHook( m_pStatusBar );
    Script::ScrSetWrapHook( m_pScriptHook );

    DefineWin32WrapperFunctions( m_pWorkThread, m_pEvoController, m_pStatusBar );
    DefineWin32EditorWrapperFunctions( m_pEditorWindow );

    //  CheckMenuItem( GetMenu( hWndApp ), IDM_STAT_WINDOW, MF_CHECKED );

    m_pWinManager->GetWindowConfiguration( );

    (void)m_pMainGridWindow->SendMessage( WM_COMMAND, IDM_FIT_ZOOM, 0 );

//	Script::ProcessScript( L"std_script.in" );
}

AppWindow::~AppWindow( )
{
    try
    {
        if ( Config::UseHistorySystem( ) )
        {
            delete m_pEvoHistWindow;
			delete m_pEvoHistorySys;
        }
        else
        {
            delete m_pModelWork;
            delete m_pWorkThread;
        }

        delete m_pMiniGridWindow;
        delete m_pMainGridWindow;
        delete m_pStatusBar;
        delete m_pStatistics;
        delete m_pPerfWindow;
        delete m_pCrsrWindow; 
        delete m_pEditorWindow;
        delete m_pDspOptWindow;
        delete m_pGridRectSel;
        delete m_pFocusPoint;
        delete m_pWinManager;
        delete m_pScriptHook;
		delete m_pEvoController;

        EvolutionCore::DeleteCore( );
    }
    catch ( ... )
    {
        exit( 1 );
    };
}

LRESULT AppWindow::UserProc
( 
    UINT   const message, 
    WPARAM const wParam, 
    LPARAM const lParam 
)
{
    switch (message)
    {

    case WM_COMMAND:
    {
        int const wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            ShowAboutBox( GetWindowHandle( ) );
            break;

        case IDM_EXIT:
            PostMessage( WM_CLOSE, 0, 0 );
            break;

        default:
			m_pEvoController->ProcessCommand( wParam, lParam );
            break;
        }
    }
    break;

    case WM_SIZE:
    case WM_MOVE:
        adjustChildWindows( );
        break;

    case WM_CLOSE:
        m_pWinManager->StoreWindowConfiguration( );
        m_pWorkThread->PostEndThread( GetWindowHandle( ) );
        return 1;  // Do not call DefWindowProc. Worker thread will call DestroyWindow. 

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        break;
    }
    
    return DefWindowProc( message, wParam, lParam );
}

void AppWindow::adjustChildWindows( )
{
    static int const HIST_WINDOW_HEIGHT = 30;

    PixelRectSize pntAppClientSize( GetClRectSize( ) );

    if ( ! pntAppClientSize.IsEmpty( ) )
    {
        m_pStatusBar->Resize( );
        pntAppClientSize.ReduceHeight( m_pStatusBar->GetHeight( ) );

        if ( m_pEvoHistWindow != nullptr )
        {
            m_pEvoHistWindow->Move   // adapt history window to new size
			( 
				0, 
				pntAppClientSize.GetHeight( ) - HIST_WINDOW_HEIGHT, 
				pntAppClientSize.GetWidth(), 
				HIST_WINDOW_HEIGHT, 
				TRUE 
			); 
        }

        m_pMainGridWindow->Move( 0, 0, pntAppClientSize.GetWidth( ), pntAppClientSize.GetHeight( ), TRUE );
    }
}
