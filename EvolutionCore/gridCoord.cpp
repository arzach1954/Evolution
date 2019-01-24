// gridCoord.cpp
//
// library: EvolutionCore

#include "stdafx.h"
#include "util.h"
#include "gridCoord.h"

using namespace std;

std::wostream & operator << ( std::wostream & out, GRID_COORD const c )
{
    out << c.get();
    return out;
}

constexpr GRID_COORD operator"" _GRID_COORD( unsigned long long ull )
{
	return GRID_COORD( CastToUnsignedShort( ull ) );
}