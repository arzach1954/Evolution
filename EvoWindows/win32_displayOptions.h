// win32_displayOptions.h 
//

#pragma once

#include "win32_baseDialog.h"
#include <functional>
#include "gridPoint.h"

class EvolutionCore;

class DspOptWindow : public BaseDialog
{
public:
    DspOptWindow( );
    ~DspOptWindow( );

    void Start( HWND const );
	void Stop( );

	int GetIntValue( EvolutionCore const * const pCore, GridPoint const gp ) 
	{ 
		return (m_IntValueLambda == nullptr) ? 0 : m_IntValueLambda( pCore, gp ); 
	}

	void UpdateDspOptionsControls( tBrushMode const );
	void SetDisplayMode( WORD );  // one of IDM_MUT_RATE/IDM_FERTILITY/IDM_FOOD_STOCK/IDM_FERTILIZER
    void SetIndividualsVisible( );
    BOOL AreIndividualsVisible( ) const;

private:
    DspOptWindow             ( DspOptWindow const & );  // noncopyable class 
    DspOptWindow & operator= ( DspOptWindow const & );  // noncopyable class 

	virtual INT_PTR UserProc( UINT const, WPARAM const, LPARAM const );

	std::function<int( EvolutionCore const * const, GridPoint const )> m_IntValueLambda;
};
