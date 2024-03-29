// win32_focusPoint.h
//

#pragma once

#include "GridDimensions.h"
#include "GridPoint.h"
#include "EvolutionCore.h"
#include "HistoryGeneration.h"
#include "win32_viewCollection.h"

class RootWindow;
class EvoHistorySysGlue;
class EvolutionCore;

class FocusPoint
{
public:
    FocusPoint( );
	virtual ~FocusPoint( ) {};

    void Start( EvoHistorySysGlue * );

    void SetFocusPoint( GridPoint const );
	void AttachFocusPointObserver( RootWindow * pRootWin )
	{
		m_ViewCollection.AttachObserver( pRootWin );
	}
    
	GridPoint const GetGridPoint( ) const 
	{ 
		return m_gp; 
	}

	BOOL const IsInGrid( ) const 
	{ 
		return ::IsInGrid( m_gp ); 
	}

	BOOL const IsAlive( EvolutionCore const & core ) const 
	{ 
		return core.IsAlive( m_gp ); 
	}

	BOOL const IsDead( EvolutionCore const & core ) const 
	{ 
		return core.IsDead( m_gp ); 
	}

	BOOL const IsDefined( EvolutionCore const & core ) const 
	{ 
		return core.IsDefined( m_gp ); 
	}

private:
    ViewCollection      m_ViewCollection;
    EvoHistorySysGlue * m_pEvoHistGlue;
    GridPoint           m_gp;
};
