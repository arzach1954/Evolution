// win32_memorySlot.cpp
//

#include "stdafx.h"
#include <iomanip>
#include "win32_memorySlot.h"

GridPoint MemorySlot::GetReferencedGridPoint( GridPoint const gp ) const 
{ 
	return getReferencedGridPoint( getIndId( gp ) );
}

void MemorySlot::FillBuffer( GridPoint const gp )
{
	wostringstream & buffer = m_textDisplay.Buffer();
	IndId            indId  = getIndId( gp );

	buffer << setw( 10 );
	if ( getReferencedGridPoint( indId ) == GridPoint::GP_NULL ) 
		buffer << L"DEAD";
	else
		buffer << indId;
}

GridPoint MemorySlot::getReferencedGridPoint( IndId const indId ) const 
{ 
	return m_textDisplay.Core().FindGridPoint( indId );
}

IndId MemorySlot::getIndId( GridPoint const gp ) const 
{ 
	return m_textDisplay.Core().GetMemEntry( gp, m_index );
}