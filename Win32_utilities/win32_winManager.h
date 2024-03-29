// win32_winManager.h : 
//

#pragma once

#include <string>
#include <unordered_map>
#include "win32_util.h"
#include "win32_rootWindow.h"

class RootWindow;

class WinManager
{
public:
    WinManager( );
    virtual ~WinManager( ) { };

	void AddWindow( std::wstring const, UINT const, HWND, BOOL const, BOOL const );

	void AddWindow
	( 
		wstring    const wstrName, 
		UINT       const id, 
		RootWindow const & rootWindow,
		BOOL       const bTrackPosition,
		BOOL       const bTrackSize
	)
	{
		AddWindow( wstrName, id, rootWindow.GetWindowHandle(), bTrackPosition, bTrackSize );
	}

	void RemoveWindow( UINT const id )
	{
		m_map.erase( id );
	}

	std::wstring const GetWindowName( UINT const id )  const // can throw out_of_range exception
    {
        return m_map.at( id ).m_wstr;
    }

    HWND const GetHWND( UINT const id )  const // can throw out_of_range exception
    {
        return m_map.at( id ).m_hwnd;
    }

	INT const GetIdFromRootWindow( HWND const hwnd )
	{
		for ( auto pp : m_map )
			if ( pp.second.m_hwnd == hwnd )
				return pp.first; 

		return -1;
	}

    BOOL const IsMoveable( UINT const id )  const // can throw out_of_range exception
    {
        return m_map.at( id ).m_bTrackPosition;
    }

    BOOL const IsSizeable( UINT const id )  const // can throw out_of_range exception
    {
        return m_map.at( id ).m_bTrackSize;
    }

    void Show( UINT const id, tBoolOp const op ) const
    {
        Util::Show( GetHWND( id ), op );
    }

    void SetWindowConfigurationFile( std::wstring const fileName ) 
    { 
        m_strWindowConfigurationFile = fileName; 
    };

    void IncNrOfMonitorConfigurations( ) 
    { 
        ++m_iNrOfMonitorConfigurations; 
    };

    BOOL GetWindowConfiguration( );
    void StoreWindowConfiguration( );

private:
    std::wstring const MONITOR_CONFIG_FILE     = L"MonitorConfigurations.cnf";
    std::wstring const WINDOW_CONFIG_FILE_STUB = L"WindowConfiguration";

    struct MAP_ELEMENT
    {
        std::wstring const m_wstr;
        HWND         const m_hwnd;
		BOOL         const m_bTrackPosition; // if TRUE, winManager sets window position from config file
		BOOL         const m_bTrackSize;     // if TRUE, winManager sets window size from config file
    };

    std::unordered_map< UINT, MAP_ELEMENT > m_map;
    
    std::wstring m_strWindowConfigurationFile;
    int          m_iNrOfMonitorConfigurations;

    void dumpMonitorConfiguration( ) const;
    void dumpWindowCoordinates( ) const;
};
