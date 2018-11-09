// win32_shape.h : 
//
// Shape is defined by the offset of the top left corner
// and the size of the rectangle 

#pragma once

#include <sstream> 
#include "vector"
#include "d3d_buffer.h"
#include "GridPoint.h"
#include "EvolutionCore.h"
#include "win32_textDisplay.h"
#include "pixelPoint.h"

using namespace std;

class Shape
{
public:
	static short const ZOOM_LEVEL_1 =  96;
	static short const ZOOM_LEVEL_2 = 256;

	Shape
	( 
		Shape * const pParent,
		TextDisplay & textDisplay 
	) :
		m_pParent( pParent ),
		m_rect( PixelRect( ) ),
		m_textDisplay( textDisplay )
	{}

	PixelRect GetAbsoluteCoordinates( ) const
	{
		PixelRect pRes( m_rect );
		for ( Shape * pParent = m_pParent; pParent; pParent = pParent->m_pParent )
			pRes += pParent->m_rect.GetStartPoint();
		return pRes;
	}

	void SetShapeRect( PixelRect const & rect )
	{
		m_rect = rect;
	}

	void SetShapeRect( PixelPoint const & pp, PixelRectSize const & size )
	{
		SetShapeRect( PixelRect( pp, size )	);
	}

	PixelRect const GetShapeRect( ) const 
	{
		return m_rect;
	}

	PixelRectSize const GetShapeSize( ) const 
	{
		return m_rect.GetSize();
	}

	long const GetShapeWidth( ) const 
	{
		return m_rect.GetWidth();
	}

	long const GetShapeHeight( ) const 
	{
		return m_rect.GetHeight();
	}

	virtual bool PointInShape( PixelPoint const pnt ) const
	{
		PixelRect rectAbsolute = GetAbsoluteCoordinates( );
		return rectAbsolute.Includes( pnt );
	}

	Shape const * FindShape
	( 
		PixelPoint const pnt,             
		GridPoint  const gp
	) const
	{
		return PointInShape( pnt ) ? this : nullptr;
	}

	virtual GridPoint GetReferencedGridPoint( GridPoint const gp ) const 
	{ 
		return GridPoint::GP_NULL; 
	}

	void Draw( GridPoint const gp )
	{
		m_textDisplay.Clear();
		FillBuffer( gp );
		PixelRect rectAbsolute = GetAbsoluteCoordinates( );
		m_textDisplay.DrawText( rectAbsolute );
	}

	virtual void PrepareShape( GridPoint const ) {};

protected:

	virtual void FillBuffer( GridPoint const ) = 0;

    TextDisplay & m_textDisplay;

private:
	PixelRect m_rect;
	Shape   * m_pParent;
};