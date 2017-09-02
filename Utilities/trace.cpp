// trace.cpp
//

#include "stdafx.h"
#include "assert.h"
#include <fstream>
#include <iostream>
#include "trace.h"

using namespace std;

wofstream OpenTraceFile( wchar_t const * const wszTraceFileName )
{
    wofstream traceStream;
    traceStream.open( wszTraceFileName, ios::out );
    if ( traceStream.bad() )
    {
        cout << "Could not open trace file " << wszTraceFileName << endl;
        assert( false );
    }

    traceStream << L"OpenTraceFile " << L" \"" << wszTraceFileName << L"\"" << endl;

    return traceStream;
}