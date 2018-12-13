// PixelCore.h
//

#pragma once

#include "pixelPoint.h"

class PixelCoordinates;
class PixelRectSize;
class EvolutionCore;

class PixelCore
{
public:
	PixelCore
    ( 
        EvolutionCore *,
		PixelCoordinates *
    );

	~PixelCore( );

	void       SetSelection( PixelPoint const, PixelPoint const );
	PixelPoint GetPoiCenter() const;
	bool       CenterPoi( PixelPoint const ) const;
		
private:
    EvolutionCore    * m_pCore;
	PixelCoordinates * m_pPixelCoordinates;
};
