// win32_rootWindow.h : 
//

#pragma once

#include <chrono>
#include "Windowsx.h"
#include "commctrl.h"
#include "PixelTypes.h"
#include "ObserverInterface.h"
#include "win32_tooltip.h"
#include "win32_util.h"

class Observer;

using namespace std::chrono;

class RootWindow: public ObserverInterface
{
public:

    RootWindow( );
    virtual ~RootWindow( );

    HWND GetWindowHandle( ) const { return m_hwnd; };

    PIXEL         const GetWindowTop( )            const { return Util::GetWindowTop           ( m_hwnd ); }
    PIXEL         const GetWindowWidth( )          const { return Util::GetWindowWidth         ( m_hwnd ); }
    PIXEL         const GetWindowHeight( )         const { return Util::GetWindowHeight        ( m_hwnd ); }
    PixelRectSize const GetWindowSize( )           const { return Util::GetWindowSize          ( m_hwnd ); }
    PixelRectSize const GetClRectSize( )           const { return Util::GetClRectSize          ( m_hwnd ); }
    PixelRect     const GetClPixelRect( )          const { return Util::GetClPixelRect         ( m_hwnd ); }
    PIXEL         const GetClientWindowHeight( )   const { return Util::GetClientWindowHeight  ( m_hwnd ); }
    PIXEL         const GetClientWindowWidth( )    const { return Util::GetClientWindowWidth   ( m_hwnd ); }
    BOOL          const CrsrInClientRect( )        const { return Util::CrsrInClientRect       ( m_hwnd ); }
    PixelPoint    const GetRelativeCrsrPosition( ) const { return Util::GetRelativeCrsrPosition( m_hwnd ); }
    PixelPoint    const GetClRectCenter( )         const { return Util::GetClRectCenter        ( m_hwnd ); }

	PixelPoint    const Client2Screen( PixelPoint  const & p ) const { return Util::Client2Screen ( m_hwnd, p ); }
	PixelPoint    const Screen2Client( PixelPoint  const & p ) const { return Util::Screen2Client ( m_hwnd, p ); }

	BOOL          const IsInClientRect( PixelPoint const & p ) const { return Util::IsInClientRect( m_hwnd, p ); }
	BOOL          const IsInClientRect( PixelRect  const & r ) const { return Util::IsInClientRect( m_hwnd, r ); }

	BOOL          const DestroyWindow( )              const { return ::DestroyWindow  ( m_hwnd ); }
	BOOL          const IsWindowVisible( )            const { return ::IsWindowVisible( m_hwnd ); }
	HWND          const SetCapture( )                 const { return ::SetCapture     ( m_hwnd ); }
    HWND          const SetFocus( )                   const { return ::SetFocus       ( m_hwnd ); }
    HWND          const GetDlgItem( int const iItem ) const { return ::GetDlgItem     ( m_hwnd, iItem ); }
    BOOL          const IsCaptured( )                 const { return ::GetCapture( ) == m_hwnd; }

	void         SetRefreshRate( milliseconds const msRate ) { m_msRefreshRate = msRate; }
	milliseconds GetRefreshRate( )                           { return m_msRefreshRate; }
	
    void Show( tBoolOp const op ) const { Util::Show( m_hwnd, op ); }
    void Show( BOOL    const b  ) const { Util::Show( m_hwnd, b  ); }

    HDC  BeginPaint( LPPAINTSTRUCT lpPaint ) const { return ::BeginPaint( m_hwnd, lpPaint ); }
    BOOL EndPaint  ( LPPAINTSTRUCT lpPaint ) const { return ::EndPaint  ( m_hwnd, lpPaint ); }

	HWND CreateBalloonToolTip( int const id, LPWSTR const t ) 
	{ 
		return ::CreateBalloonToolTip( m_hwnd, id, t );	
	}

	HWND CreateStdToolTip( int const id, LPWSTR const t ) 
	{ 
		return ::CreateStdToolTip( m_hwnd, id, t );	
	}

	HWND CreateWindowToolTip( LPWSTR const t ) 
	{ 
		return ::CreateWindowToolTip( m_hwnd, t );	
	}

	HWND CreateRectToolTip( int const id, PixelRect * const pRect, LPWSTR const t ) 
	{ 
		return ::CreateRectToolTip( m_hwnd, id, pRect, t );	
	}

	HBITMAP const CreateCompatibleBitmap( HDC const hDC )
	{
		PixelRectSize rectSize = GetClRectSize( );
		return ::CreateCompatibleBitmap( hDC, rectSize.GetXvalue(), rectSize.GetYvalue() );
	}
	
	LRESULT const PostMessage( UINT const msg, WPARAM const wParam, LPARAM const lParam ) const
    {
        return ::PostMessage( m_hwnd, msg, wParam, lParam );
    }

    LRESULT const SendMessage( UINT const msg, WPARAM const wParam, LPARAM const lParam ) const
    {
        return ::SendMessage( m_hwnd, msg, wParam, lParam );
    }

    LRESULT const SendNotifyMessage( UINT const msg, WPARAM const wParam, LPARAM const lParam ) const
    {
        return ::SendNotifyMessage( m_hwnd, msg, wParam, lParam );
    }

    LRESULT const SendDlgItemMessage( int const iItem, UINT msg, WPARAM wParam, LPARAM lParam ) const
    {
        return ::SendDlgItemMessage( m_hwnd, iItem, msg, wParam, lParam );
    }

	void SetDlgText( int const iItem, wchar_t const * const wstrText )
	{
		Util::SetText( GetDlgItem( iItem ), wstrText );
	}

    LRESULT const DefWindowProc( UINT msg, WPARAM wParam, LPARAM lParam ) const
    {
        return ::DefWindowProc( m_hwnd, msg, wParam, lParam );
    }

	void CheckRadioButton( int iFirst, int iLast, int iCheck ) const
	{
		(void)::CheckRadioButton( m_hwnd, iFirst, iLast, iCheck );
	}

	UINT IsDlgButtonChecked( int iIdButton ) const
	{
		return ::IsDlgButtonChecked( m_hwnd, iIdButton );
	}

	void Move( PIXEL const xPos, PIXEL const yPos, PIXEL const width, PIXEL const height, BOOL const bRedraw )
    {
        (void)::MoveWindow( m_hwnd, xPos.GetValue(), yPos.GetValue(), width.GetValue(), height.GetValue(), bRedraw );
    }

    void Move( PixelPoint const pos, PixelRectSize const size, BOOL const bRedraw )
    {
        Move( pos.GetX(), pos.GetY(), size.GetX(), size.GetY(), bRedraw );
    }

    void Move( PixelRect const rect, BOOL const bRedraw )
    {
        Move( rect.GetStartPoint(), rect.GetSize(), bRedraw );
    }

    void Invalidate( BOOL const bRedraw ) 
    {
		if ( m_hwnd != nullptr )
			(void)::InvalidateRect( m_hwnd, nullptr, bRedraw );
    }

    void FillBackground( HDC const hDC, COLORREF const col ) const
    {
        SetBkColor( hDC, col );
        Util::FastFill( hDC, m_hwnd );
    }

	short GetTrackBarPos( INT const idTrackbar ) const
	{
		return CastToShort( SendDlgItemMessage( idTrackbar, TBM_GETPOS, 0, 0 ) ); 
	}

	void SetTrackBarPos( INT const idTrackbar, LONG const lPos ) const
	{
		(void)SendDlgItemMessage
		(   
			idTrackbar, TBM_SETPOS, 
			static_cast<WPARAM>( TRUE ),                   // redraw flag 
			static_cast<LPARAM>( lPos )
		); 
	}

	void SetTrackBarRange( INT const idTrackbar, LONG const lMin, LONG const lMax ) const
	{
		(void)SendDlgItemMessage
		( 
			idTrackbar, 
			TBM_SETRANGEMIN, 
			static_cast<WPARAM>( FALSE ),                   // redraw flag 
			static_cast<LPARAM>( lMin ) 
		);
		(void)SendDlgItemMessage
		( 
			idTrackbar, 
			TBM_SETRANGEMAX, 
			static_cast<WPARAM>( TRUE ),                   // redraw flag 
			static_cast<LPARAM>( lMax ) 
		);
	}

	LRESULT PostCommand2Application( WPARAM const wParam, LPARAM const lParam )
	{
		return ::PostMessage( m_hwndApp, WM_COMMAND, wParam, lParam );
	}

	PixelPoint const GetCrsrPosFromLparam( LPARAM const lParam ) const
	{
		return PixelPoint{ PIXEL(GET_X_LPARAM( lParam )), PIXEL(GET_Y_LPARAM( lParam )) };
	}

	virtual void Notify( bool const );

protected:

	void SetWindowHandle( HWND const );

	virtual void Trigger( )
	{
		Invalidate( FALSE );
	}

private:

    static void CALLBACK TimerProc( void * const, BOOL const );

	void invalidate( )
	{
		Trigger( );
		m_bDirty = FALSE;
	}

    void startTimer( milliseconds const );

	void deleteTimer( )
	{
	    if ( m_hTimer != nullptr )
        {
			HANDLE handle = m_hTimer;
            m_hTimer = nullptr;
            (void)DeleteTimerQueueTimer( nullptr, handle, 0 );
        }
	}

    HWND   m_hwnd;
	HWND   m_hwndApp;
    HANDLE m_hTimer;
    BOOL   m_bTimerActive;
    BOOL   m_bDirty;

    milliseconds m_msRefreshRate;
};

BOOL RootWinIsReady( RootWindow const * );
