// win32_leftColumn.cpp
//

#pragma once

#include "win32_identifierShape.h"
#include "win32_infoShape.h"
#include "win32_shape.h"

class LeftColumn : public Shape
{
public:
	LeftColumn( TextDisplay & t ) :
		Shape( t ),
		m_idShape  ( t ),
		m_infoShape( t )
	{ }

	virtual PixelRectSize MinimalSize( );
	virtual void          PrepareShape( PixelPoint const, PixelRectSize const );
	virtual void          Draw        ( GridPoint  const, PixelPoint    const );
	virtual Shape const * FindShape   ( PixelPoint const, GridPoint     const ) const;

	IdentifierShape const & GetIdentifierShape() const { return m_idShape; }

private:
	IdentifierShape m_idShape;
	InfoShape       m_infoShape;
};