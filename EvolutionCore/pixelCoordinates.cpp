// pixelCoordinates.cpp
//

#include "stdafx.h"
#include <algorithm>  // min/max templates
#include "gridPoint.h"
#include "GridDimensions.h"
#include "PixelTypes.h"
#include "pixelCoordinates.h"

static double const SQRT3_DIV2 = sqrt(3.) / 2.;
static double const SQRT3_DIV3 = sqrt(3.) / 3.;

static long const MILLI_GRID_FACTOR = 1024;

PixelCoordinates::PixelCoordinates
( 
    PIXEL const fs, 
	bool  const bHexagon
)
  : m_pixOffset( 0_PIXEL_X, 0_PIXEL_Y ),
    m_pixFieldSize( fs ),
    m_smoothMove(  ),
	m_bMoving   ( false ),
	m_bHexagon  ( bHexagon )
{ }

bool PixelCoordinates::isValidFieldSize( PIXEL const pixNewFieldSize ) const 
{ 
    return (MINIMUM_FIELD_SIZE <= pixNewFieldSize) && (pixNewFieldSize <= MAXIMUM_FIELD_SIZE); 
};

void PixelCoordinates::MoveGrid( PixelPoint const pntDelta )
{
    m_pixOffset -= pntDelta;
}

PixelPoint PixelCoordinates::calcCenterOffset  // calculate new pixel offset,
(                                              // which moves gridRect to center of window.
	GridPoint  const gpCenter,                 // do not yet set m_pixOffset to this value!
	PixelPoint const pixCenter 
)  
{
	assert( IsInGrid( gpCenter ) );
	PixelPoint const pixPnt( Grid2PixelSize(gpCenter) + m_pixFieldSize / 2 );
	PixelPoint const pixOffset( pixPnt - pixCenter );
	return pixOffset;
}

bool PixelCoordinates::CenterPoi( PixelPoint const pixCenter, GridPoint const gpPoi ) // returns TRUE, if POI was already centered, or if no POI defined
{
    if ( gpPoi.IsNull( ) )
        return true;

    PixelPoint pixOffsetDesired( calcCenterOffset( gpPoi, pixCenter ) );
    bool       bCentered( m_pixOffset == pixOffsetDesired );

    if ( ! bCentered )
        m_pixOffset = m_smoothMove.Step( m_pixOffset, pixOffsetDesired );

    return bCentered;
}

void PixelCoordinates::CenterGrid
( 
	GridPoint     const gpCenter,   
	PixelRectSize const pntPixSize  // available size 
)
{
    m_pixOffset = calcCenterOffset( gpCenter, PixelPoint( pntPixSize.GetX(), pntPixSize.GetY() ) / 2 );
}

PIXEL PixelCoordinates::CalcMaximumFieldSize
( 
	GridRectSize  const & gpGridRectSize,   // Grid size to fit into window
	PixelRectSize const & pntPixSize        // available pixel size 
) const
{
	PIXEL_X xSize = pntPixSize.GetX() / gpGridRectSize.GetXvalue();
	PIXEL_Y ySize = pntPixSize.GetY() / gpGridRectSize.GetYvalue(); 
	return std::min( xSize.GetValue(), ySize.GetValue() );
}

bool PixelCoordinates::SetGridFieldSize( PIXEL const pixNewFieldSize )
{
    bool bValid = isValidFieldSize( pixNewFieldSize );
	if ( bValid )
	    m_pixFieldSize = pixNewFieldSize;
	return bValid;
}

PIXEL PixelCoordinates::ComputeNewFieldSize( bool const bZoomIn ) const
{
    PIXEL pixNewFieldSize( m_pixFieldSize );
    if ( bZoomIn )
    {
        PIXEL const pixDelta
		( 
			( m_pixFieldSize < 16_PIXEL ) 
			? 1_PIXEL
			: (
				( m_pixFieldSize < 64_PIXEL ) 
				? 16_PIXEL
				: 32_PIXEL
			   ) 
		);
        pixNewFieldSize += pixDelta;
    }
    else
    {
        PIXEL const pixDelta( ( m_pixFieldSize <= 16_PIXEL ) ? 1_PIXEL : (( m_pixFieldSize <= 64_PIXEL ) ? 16_PIXEL : 32_PIXEL ) );
        pixNewFieldSize -= pixDelta;
    }
	return pixNewFieldSize;
}

PixelPoint PixelCoordinates::Pixel2PixelSize( PixelPoint const ptSizeIn, PixelCoordinates const & fTarget ) const 
{
    static long const FACTOR( 1024 ); // to avoid zero when dividing small ptSizeIn by m_pixFieldSize 

    return (((ptSizeIn * FACTOR) / m_pixFieldSize.GetValue()) * fTarget.m_pixFieldSize.GetValue() ) / FACTOR;
}

PixelPoint PixelCoordinates::Pixel2PixelPos( PixelPoint const ptPosIn, PixelCoordinates const & fTarget ) const 
{
    return Pixel2PixelSize( ptPosIn + m_pixOffset, fTarget ) - fTarget.m_pixOffset;
}

MilliGridPoint PixelCoordinates::pixel2MilliGridPoint( PixelPoint const ptSize ) const
{
	PixelPoint size { ptSize * MILLI_GRID_FACTOR / m_pixFieldSize.GetValue() };
    return MilliGridPoint( MilliGrid_X(MilliGrid(size.GetXvalue())), MilliGrid_Y(MilliGrid(size.GetYvalue())) );
}

MilliGridRect PixelCoordinates::Pixel2MilliGridRect( PixelRect const & rect ) const 
{
	MilliGridPoint pntStartPos { pixel2MilliGridPoint( rect.GetStartPoint() + m_pixOffset ) };
	MilliGridPoint pntEndPos   { pixel2MilliGridPoint( rect.GetEndPoint()   + m_pixOffset ) };
    return MilliGridRect( pntStartPos, pntEndPos );
}

PixelPoint PixelCoordinates::Grid2PixelSize( GridPoint const gp ) const 
{ 
	PIXEL pixX { m_pixFieldSize * gp.GetXvalue() };
	PIXEL pixY { m_pixFieldSize * gp.GetYvalue() };

	if ( m_bHexagon )
	{
		pixX = PIXEL( CastToLong( static_cast<double>(pixX.GetValue()) * SQRT3_DIV2 + 0.5 ) );
	}

	return PixelPoint( PIXEL_X(pixX), PIXEL_Y(pixY) );
}

PixelPoint PixelCoordinates::Grid2PixelPos( GridPoint const gp ) const 
{ 
	PixelPoint ppRes { Grid2PixelSize( gp ) - m_pixOffset };

	if ( m_bHexagon && IsOddColumn( gp ) )
		ppRes -= PixelPoint( 0_PIXEL_X, PIXEL_Y(m_pixFieldSize / 2) );

	return ppRes;
}

PixelPoint PixelCoordinates::Grid2PixelPosCenter( GridPoint const gp ) const 
{ 
	PixelPoint pxResult( Grid2PixelPos( gp ) );
	if (m_bHexagon)
	{
		pxResult += PixelPoint( 
			                     PIXEL_X(PIXEL(static_cast<long>(SQRT3_DIV3 * m_pixFieldSize.GetValue()))),
		                         PIXEL_Y(m_pixFieldSize / 2) 
			                  );
	}
	else
	{
		pxResult += m_pixFieldSize / 2; 
	}
	return pxResult;
}

PixelRect PixelCoordinates::GridPoint2PixelRect( GridPoint const gp ) const
{
	return PixelRect
	(
		Grid2PixelPos( gp ),  // PixelPoint 
		PixelRectSize( PIXEL_X(m_pixFieldSize), PIXEL_Y(m_pixFieldSize) ) 
	);
}

GridPoint PixelCoordinates::Pixel2GridPos( PixelPoint const pp ) const 
{ 
	PixelPoint pixPoint( pp + m_pixOffset );

	if ( m_bHexagon ) // adapted from http://blog.ruslans.com/2011/02/hexagonal-grid-math.html
	{
		double const dFieldSize = static_cast<double>(m_pixFieldSize.GetValue());
		double const dRadius    = dFieldSize * SQRT3_DIV3;
		double const dSide      = dFieldSize * SQRT3_DIV2;

		double const dPixPointX = static_cast<double>(pixPoint.GetXvalue());
		double const dCi        = floor(dPixPointX/dSide);
		double const dCx        = dPixPointX - dSide * dCi;

		GRID_COORD const gCi    = GRID_COORD( CastToShort(dCi) );
		bool       const bOdd   = IsOdd(gCi);

		double const dPixPointY = static_cast<double>(pixPoint.GetYvalue());
		double const dTy        = dPixPointY + (bOdd ? (dFieldSize * 0.5) : 0);
		double const dCj        = floor(dTy/dFieldSize);
		double const dCy        = dTy - dFieldSize * dCj;
		double const dCrit      = 0.5 - dCy / dFieldSize;

		GridPoint gpResult( GRID_X(gCi), GRID_Y(GRID_COORD( CastToShort(dCj) )) );

		if (dCx <= dRadius * abs(dCrit))
		{
			gpResult -= GridPoint( 1_GRID_X, 0_GRID_Y );
			if (bOdd)
				gpResult -= GridPoint( 0_GRID_X, 1_GRID_Y );
			if (dCrit < 0)
				gpResult += GridPoint( 0_GRID_X, 1_GRID_Y );
		}

		return gpResult;
	}
	else 
	{
		if ( pixPoint.GetX() < 0_PIXEL_X ) 
		{
			pixPoint -= PixelPoint( PIXEL_X(m_pixFieldSize - 1_PIXEL), 0_PIXEL_Y );
		}
		if ( pixPoint.GetY() < 0_PIXEL_Y ) 
		{
			pixPoint -= ( 0_PIXEL, m_pixFieldSize - 1_PIXEL );
		}

		GridPoint gp = GridPoint
		( 
			GRID_X( GRID_COORD(CastToShort(pixPoint.GetXvalue() / m_pixFieldSize.GetValue() )) ), 
			GRID_Y( GRID_COORD(CastToShort(pixPoint.GetYvalue() / m_pixFieldSize.GetValue() )) ) 
		); 

		return gp;
	}
}

PixelPoint PixelCoordinates::milliGrid2PixelSize( MilliGridRectSize const kgpSize ) const 
{
	MilliGridRectSize const size { (kgpSize * m_pixFieldSize.GetValue()) / MILLI_GRID_FACTOR };
    return PixelPoint( PIXEL_X(PIXEL(size.GetXvalue())), PIXEL_Y(PIXEL(size.GetYvalue())) );
}

PixelRect PixelCoordinates::MilliGrid2PixelRect( MilliGridRect const & kgrIn ) const 
{
	MilliGridRectSize const kpSize{ kgrIn.GetStartPoint().GetX(), kgrIn.GetStartPoint().GetY() };
	PixelPoint    const ptPos { milliGrid2PixelSize( kpSize ) - m_pixOffset };
	PixelPoint    const ptSize{ milliGrid2PixelSize( kgrIn.GetSize() ) };
    return PixelRect( ptPos, ptPos + ptSize );
}

GridRect PixelCoordinates::Pixel2GridRect(PixelRect const & rect ) const 
{
    GridRect gridRect
    ( 
        ClipToGrid( Pixel2GridPos( rect.GetStartPoint() ) ), 
        ClipToGrid( Pixel2GridPos( rect.GetEndPoint  () ) )
    );
	return gridRect;
}

PixelRect PixelCoordinates::Grid2PixelRect( GridRect const & rcGrid ) const 
{
    return PixelRect
    ( 
        Grid2PixelPos( rcGrid.GetStartPoint( ) ),
        Grid2PixelPos( rcGrid.GetEndPoint  ( ) ) + ( m_pixFieldSize - 1_PIXEL ) 
    );
}
