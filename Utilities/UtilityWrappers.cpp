// UtilityWrappers.cpp
//

#include "stdafx.h"
#include "assert.h"
#include "script.h"
#include "symtab.h"
#include "trace.h"
#include "BoolOp.h"
#include "UtilityWrappers.h"

PIXEL ScrReadPixel( Script & script )
{
    return PIXEL( script.ScrReadLong() );
}

PixelPoint ScrReadPixelPoint( Script & script )
{
    PIXEL const x( ScrReadPixel( script ) );
    PIXEL const y( ScrReadPixel( script ) );
    return PixelPoint( x, y );
}

PixelRectSize ScrReadPixelRectSize( Script & script )
{
    PixelPoint const pixPnt( ScrReadPixelPoint( script ) );
    return PixelRectSize( pixPnt.GetX(), pixPnt.GetY() );
}

PixelRect ScrReadPixelRect( Script & script )
{
    PixelPoint    const pixPos ( ScrReadPixelPoint   ( script ) );
    PixelRectSize const pixSize( ScrReadPixelRectSize( script ) );
    return PixelRect( pixPos, pixSize );
}

class WrapOpenTraceFile : public Script_Functor
{
public:
    virtual void operator() ( Script & script ) const
    {
		OpenTraceFile( script.ScrReadString( ) );
    }
};

void DefineUtilityWrapperFunctions( )
{
    DEF_FUNC( OpenTraceFile );

    DEF_ULONG_CONST( tBoolOp::opTrue );
    DEF_ULONG_CONST( tBoolOp::opFalse );
    DEF_ULONG_CONST( tBoolOp::opToggle );
    DEF_ULONG_CONST( tBoolOp::opNoChange );
}
