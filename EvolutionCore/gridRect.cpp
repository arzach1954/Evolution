// gridRect.cpp :
//

#include "stdafx.h"
#include <algorithm>  // min/max templates
#include "gridRect.h"

using std::min;
using std::max;

GridRect const ClipToGrid( GridRect const & rect ) 
{ 
	GridPoint const startPoint
	(
		max( rect.GetLeft(), GRID_RECT_FULL().GetLeft() ),
		max( rect.GetTop(),  GRID_RECT_FULL().GetTop()  )
	);

	GridPoint const endPoint
	(
		min( rect.GetRight(),  GRID_RECT_FULL().GetRight() ),
		min( rect.GetBottom(), GRID_RECT_FULL().GetBottom() )
	);

	return GridRect( startPoint, endPoint );	
}

void Apply2Rect( GridPointFunc const & func, GridRect const & rect )
{
	GridRect rectClipped = ClipToGrid( rect );
	::Apply2Rect( func, rectClipped.GetStartPoint(), rectClipped.GetEndPoint() );
}

void Apply2Rect
( 
	GridPointFunc const & func,
	GridPoint             gpStart,
	GridPoint             gpEnd,
	bool          const   fWithBorders
)
{
	if (fWithBorders)
	{
		gpStart -= GridPoint( GRID_COORD(1_GRID_COORD), GRID_COORD(1_GRID_COORD) );
		gpEnd   += GridPoint( GRID_COORD(1_GRID_COORD), GRID_COORD(1_GRID_COORD) );
	}

    for ( short y = gpStart.GetYvalue(); y <= gpEnd.GetYvalue(); ++y )
    for ( short x = gpStart.GetXvalue(); x <= gpEnd.GetXvalue(); ++x )
	{
		func( GridPoint{ GRID_COORD(x), GRID_COORD(y) } );
	}
}

void Apply2Grid( GridPointFunc const & func, bool const fWithBorders )
{
	Apply2Rect( func, GRID_ORIGIN(), GRID_MAXIMUM(), fWithBorders );
}
