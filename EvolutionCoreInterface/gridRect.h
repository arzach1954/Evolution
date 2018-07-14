// gridRect.h : 
//

#pragma once

#include <iostream>
#include <functional>
#include "gridPoint.h"

class GridRect;

void Apply2Grid( const std::function<void( GridPoint const &)>& func );

class GridRect
{
public:
	GridRect( ) { Reset(); }
    GridRect( GRID_COORD const lLeft, GRID_COORD const lTop, GRID_COORD const lRight, GRID_COORD const lBottom ) : m_lLeft(lLeft), m_lTop(lTop), m_lRight(lRight), m_lBottom(lBottom) {};
    GridRect( GridPoint const & gpStart, GridPoint const & gpEnd ) : m_lLeft(gpStart.x), m_lTop(gpStart.y), m_lRight(gpEnd.x), m_lBottom(gpEnd.y) {};
    GridRect( GridPoint const & gpCenter, GRID_COORD const iSize ) : m_lLeft(gpCenter.x - iSize), m_lTop(gpCenter.y - iSize), m_lRight(gpCenter.x + iSize), m_lBottom(gpCenter.y + iSize) {};

    ~GridRect() { };
    
	void Apply2Rect( std::function<void( GridPoint const &, short const)> const &, short const = 0 ) const;
	void Apply2Rect( std::function<void( GridPoint const &             )> const &                  ) const;

    bool const operator== ( GridRect const &a ) const { return ( a.m_lLeft == m_lLeft ) && ( a.m_lTop == m_lTop ) && ( a.m_lRight == m_lRight ) && ( a.m_lBottom == m_lBottom ); };
    bool const operator!= ( GridRect const &a ) const { return ( a.m_lLeft != m_lLeft ) || ( a.m_lTop != m_lTop ) || ( a.m_lRight != m_lRight ) || ( a.m_lBottom != m_lBottom ); };

    GRID_COORD const GetLeft()         const { return m_lLeft;   };
    GRID_COORD const GetTop()          const { return m_lTop;    };
    GRID_COORD const GetRight()        const { return m_lRight;  };
    GRID_COORD const GetBottom()       const { return m_lBottom; };
    GRID_COORD const GetMinExtension() const { GridPoint gpSize = GetSize(); return (gpSize.x < gpSize.y) ? gpSize.x : gpSize.y; };

    GridPoint const GetStartPoint( ) const { return GridPoint( m_lLeft,  m_lTop    ); }
    GridPoint const GetEndPoint  ( ) const { return GridPoint( m_lRight, m_lBottom ); }
    GridPoint const GetSize      ( ) const { return GridPoint( m_lRight - m_lLeft, m_lBottom - m_lTop ); }
    GridPoint const GetCenter    ( ) const { return GridPoint( (m_lLeft + m_lRight) / 2, (m_lTop + m_lBottom) / 2 ); }

    bool const IsEmpty( )    const { return ( *this == GRID_RECT_EMPTY ); }
    bool const IsNotEmpty( ) const { return ( *this != GRID_RECT_EMPTY ); }

    void ClipToGrid( );
    void Move     ( GridPoint const & );
    void SetCenter( GridPoint const & );
    void SetSize  ( GridPoint const & );
	void Reset( ) { *this = GRID_RECT_EMPTY; }

	static GridRect GetFullRect( ) { return GRID_RECT_FULL; }

 private:
    static GridRect const GRID_RECT_EMPTY;
    static GridRect const GRID_RECT_FULL;

	GRID_COORD m_lLeft;
    GRID_COORD m_lTop;
    GRID_COORD m_lRight;
    GRID_COORD m_lBottom;
};

std::wostream & operator << ( std::wostream &, GridRect const & );
