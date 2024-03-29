// BoolOp.cpp
//

#include "stdafx.h"
#include <unordered_map>
#include "BoolOp.h"

wchar_t const * const GetBoolOpName( tBoolOp const op )
{
    static std::unordered_map < tBoolOp, wchar_t const * const > mapNames =
    {
        { tBoolOp::opTrue,     L"tBoolOp::opTrue"     },  
        { tBoolOp::opFalse,    L"tBoolOp::opFalse"    },  
        { tBoolOp::opToggle,   L"tBoolOp::opToggle"   },  
        { tBoolOp::opNoChange, L"tBoolOp::opNoChange" }
    };

    return mapNames.at( op );
}
