// gridRect.h : 
//
// grid coordinates
// origin is bottom left

#pragma once

#include <iostream>
#include "gridPoint.h"

class GridRect;

void Apply2Rect( GridPointFunc const &, GridPoint const, GridPoint const, bool const = false );
void Apply2Grid( GridPointFunc const &,                                   bool const = false );

class GridRect
{
public:
	GridRect( ) 
	{ 
		Reset(); 
	}

    GridRect
	( 
		GridPoint const gpStart, 
		GridPoint const gpEnd 
	) : 
		m_lLeft  (gpStart.x), 
		m_lBottom(gpStart.y), 
		m_lRight (gpEnd.x), 
		m_lTop   (gpEnd.y) 
	{
		assert( m_lBottom <= m_lTop );
	};

	~GridRect() { };

	void Reset( ) { * this = GRID_RECT_EMPTY; }

	void Apply2Rect( GridPointFunc const & func ) const
	{
		::Apply2Rect( func, clipStartPoint( ), clipEndPoint( ) );
	}

    bool const operator== ( GridRect const &a ) const { return ( a.m_lLeft == m_lLeft ) && ( a.m_lTop == m_lTop ) && ( a.m_lRight == m_lRight ) && ( a.m_lBottom == m_lBottom ); };
    bool const operator!= ( GridRect const &a ) const { return ( a.m_lLeft != m_lLeft ) || ( a.m_lTop != m_lTop ) || ( a.m_lRight != m_lRight ) || ( a.m_lBottom != m_lBottom ); };

	GRID_COORD const GetLeft  () const { return m_lLeft;   };
    GRID_COORD const GetTop   () const { return m_lTop;    };
    GRID_COORD const GetRight () const { return m_lRight;  };
    GRID_COORD const GetBottom() const { return m_lBottom; };

    GridPoint const GetStartPoint( ) const { return GridPoint( GetLeft(),  GetBottom() ); }
    GridPoint const GetEndPoint  ( ) const { return GridPoint( GetRight(), GetTop()    ); }
    GridPoint const GetCenter    ( ) const { return GridPoint( (m_lLeft + m_lRight) / 2, (m_lTop + m_lBottom) / 2 ); }
    GridPoint const GetSize      ( ) const { return GridPoint( (m_lRight - m_lLeft),     (m_lTop - m_lBottom)     ); }
	
	GridRect  const ClipToGrid   ( ) const { return GridRect( clipStartPoint( ), clipEndPoint( ) );	}
 
    bool const IsEmpty( )    const { return ( *this == GRID_RECT_EMPTY ); }
    bool const IsNotEmpty( ) const { return ( *this != GRID_RECT_EMPTY ); }

	bool Includes( GridPoint const gp ) const
	{
		return (m_lLeft <= gp.x) && (gp.x <= m_lRight) && (m_lBottom <= gp.y) && (gp.y <= m_lTop);
	}

    static GridRect const GRID_RECT_FULL;
    static GridRect const GRID_RECT_EMPTY;

private:
	GridPoint clipStartPoint( ) const;
	GridPoint clipEndPoint  ( ) const;

	GRID_COORD m_lLeft;
    GRID_COORD m_lBottom;
    GRID_COORD m_lRight;
    GRID_COORD m_lTop;
};

std::wostream & operator << ( std::wostream &, GridRect const & );
