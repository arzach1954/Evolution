// win32_status.h 
//

#pragma once

#include <string>
#include "PixelTypes.h"
#include "EvolutionTypes.h"
#include "win32_rootWindow.h"

class ReadBuffer;

using std::wstring;

class StatusBar : public RootWindow
{
public:
    void  Start( HWND const, ReadBuffer * );
	void  SetSimuMode( BOOL const );
	void  SetRunMode( BOOL const );
    PIXEL GetHeight( ) const;
    void  Resize( ) const;
    void  SetSizeTrackBar ( PIXEL const ) const;
    void  SetSpeedTrackBar( DWORD const ) const;
    void  ClearStatusLine( );
    void  DisplayStatusLine( std::wstring const & );
    void  DisplayScriptLine( std::wstring const &, int, std::wstring const & );
    void  DisplayCurrentGeneration( EVO_GENERATION const );

private:

    enum class tPart
    {
        Generation,
		Mode,
        Size,
        SimuEdit,
        ScriptLine,
        Stop
    };

	HWND WINAPI createControl      ( LPCTSTR, LPCTSTR, DWORD, HMENU );
    HWND WINAPI createStaticControl( LPCTSTR );
    HWND WINAPI createButton       ( LPCTSTR const, HMENU const, DWORD const  );
    HWND WINAPI createTrackBar     ( HMENU );

	void WINAPI createSizeControl ( );
    void WINAPI createSimulationControl( );
    void WINAPI createEditorControl( );

	PIXEL m_pixClientHeight;
    PIXEL m_pixBorderX;
    PIXEL m_pixBorderY;
    PIXEL m_pixPosX;
    wstring m_wstrGeneration;
    wstring m_wstrScriptLine;

	ReadBuffer * m_pReadBuffer;

friend static LRESULT CALLBACK OwnerDrawStatusBar( HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR );
};
