// pixel.h
//

#pragma once

#include "util.h"
#include "NamedType.h"

using PIXEL = NamedType< long >;

constexpr PIXEL operator"" _PIXEL( unsigned long long ull )
{
	return PIXEL( CastToShort( ull ) );
}
