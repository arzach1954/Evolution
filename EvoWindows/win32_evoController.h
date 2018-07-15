// win32_evoController.h
//

#pragma once

#include <string>
#include <fstream>

using namespace std;

class PerformanceWindow;
class StatusBar;
class WorkThread;
class WinManager;
class GridWindow;
class EditorWindow;

class EvoController
{
public:
	EvoController( );

	virtual ~EvoController( );

	void EvoController::Start
	( 
		wostream          *, 
		WorkThread        * const,
		WinManager        * const,
		PerformanceWindow * const,
		StatusBar         * const,
		GridWindow        * const,
		EditorWindow      * const
	);

	void ProcessCommand( WPARAM const, LPARAM const );
	void SetSimulationMode( tBoolOp const );

private:
	void scriptDialog( );

	bool                m_bSimulationMode;   // if TRUE: simu mode, FALSE: edit more
	BOOL                m_bTrace;
    wostream          * m_pTraceStream;
	WorkThread        * m_pWorkThread;
	WinManager        * m_pWinManager;
    PerformanceWindow * m_pPerformanceWindow;
	StatusBar         * m_pStatusBar;
	GridWindow        * m_pGridWindow;
	EditorWindow      * m_pEditorWindow;
};
