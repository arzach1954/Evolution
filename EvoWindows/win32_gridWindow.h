// win32_gridWindow.h : 
//

#pragma once

#include "win32_util.h"
#include "win32_editor.h"
#include "win32_crsrWindow.h"
#include "win32_baseWindow.h"
#include "win32_focusPoint.h"

//lint -esym( 763, WorkThread )    redundant declaration
//lint -esym( 763, DspOptWindow )  redundant declaration

class GridRect;
class StatusBar;
class DrawFrame;
class PixelCoordinates;
class PixelCore;
class WorkThread;
class DspOptWindow;
class EvolutionCore;
class EvolutionModelData;
class ObserverInterface;
class PerformanceWindow;

class GridWindow : public BaseWindow
{
public:
    GridWindow( );

    void Start
    ( 
        HWND, 
        WorkThread         * const,
        EditorWindow       * const,
        FocusPoint         * const,
        DspOptWindow       * const,
        PerformanceWindow  * const, 
        StatusBar          * const,
        EvolutionCore      *, 
        EvolutionModelData *,
        DWORD const, 
        SHORT const
    );

    ~GridWindow( );

    void Observe( GridWindow * );
	void SetZoom( SHORT const );
    void Size( );

private:
    GridWindow             ( GridWindow const & );  // noncopyable class 
    GridWindow & operator= ( GridWindow const & );  // noncopyable class 
    
    WorkThread         * m_pWorkThread;
    PixelCoordinates   * m_pPixelCoordinates;  // My own PixelCoordinates
    GridWindow         * m_pGWObserved;	 // Observed GridWindow (or nullptr)
    StatusBar          * m_pStatusBar; 
	EvolutionCore      * m_pCore;
    EvolutionModelData * m_pModelWork;
    EditorWindow       * m_pEditorWindow;
    PerformanceWindow  * m_pPerformanceWindow;
    DrawFrame          * m_pDrawFrame;
    PixelPoint 	         m_ptLast;	 	 // Last cursor position during selection 
    BOOL                 m_bMoveAllowed;  // TRUE: move with mouse is possible
    FocusPoint         * m_pFocusPoint;
    ObserverInterface  * m_pObserverInterface;
	PixelCore          * m_pPixelCore;

    virtual LRESULT UserProc( UINT const, WPARAM const, LPARAM const );

	void fit( );
	void zoom( BOOL const );
	void mouseWheelAction( int );
    BOOL inObservedClientRect( LPARAM );
    void moveGrid( PixelPoint const & );
    void onMouseMove( LPARAM, WPARAM );
    void contextMenu( LPARAM );
    void doPaint( );
};
