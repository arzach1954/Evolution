// gridPoint.h : 
//
// library EvolutionCoreInterface

#pragma once

#include <stdlib.h>   // abs
#include <algorithm>  // min/max templates
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <functional>
#include "debug.h"
#include "util.h"
#include "Int24.h"
#include "gridCoord.h"
#include "PointType.h"

using std::min;
using std::max;

using GridPoint = PointType< GRID_COORD, struct GridPoint_Parameter >;

inline bool const Neighbors( GridPoint const a, GridPoint const b )
{ 
	GridPoint gpDiff = abs_value( a - b );
    return ( 
		      (( gpDiff.GetX() <= 1_GRID_COORD ) || ( gpDiff.GetX() == GRID_X_MAX )) && 
		      (( gpDiff.GetY() <= 1_GRID_COORD ) || ( gpDiff.GetY() == GRID_Y_MAX ))
		   );
}

inline static int const GRID_AREA() 
{ 
	static int res = GRID_WIDTH.GetValue() * GRID_HEIGHT.GetValue(); 
	return res;
};

inline static GridPoint const & GRID_ORIGIN() 
{ 
	static GridPoint res = GridPoint( GRID_X_MIN,  GRID_Y_MIN ); 
	return res;
};

inline static GridPoint const & GRID_MAXIMUM() 
{ 
	static GridPoint res = GridPoint( GRID_X_MAX, GRID_Y_MAX ); 
	return res;
};

inline static GridPoint const & GRID_SIZE() 
{ 
	static GridPoint res = GridPoint( GRID_WIDTH, GRID_HEIGHT ); 
	return res;
};

inline GridPoint const Wrap2Grid(GridPoint const gp) 
{ 
	return (gp + GRID_SIZE()) % GRID_SIZE(); 
}

inline GridPoint const ClipToGrid( GridPoint gp ) 
{ 
	gp = GridPoint
	(
		max( gp.GetX(), GRID_X_MIN ),
		max( gp.GetY(), GRID_Y_MIN  )
	);

	gp = GridPoint
	(
		min( gp.GetX(), GRID_X_MAX ),
		min( gp.GetY(), GRID_Y_MAX )
	);

	return gp;	
}

inline bool const IsInGrid( GridPoint const & gp ) 
{ 
	return ClipToGrid( gp ) == gp;
};

inline bool IsEvenColumn( GridPoint const & gp ) { return IsEven( gp.GetX() ); }
inline bool IsOddColumn ( GridPoint const & gp ) { return IsOdd ( gp.GetX() ); }

using GridPointFunc   = std::function<void (GridPoint const)>;
using ManipulatorFunc = std::function<short(short const, short const)>;
