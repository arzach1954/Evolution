// interaction.cpp
//

#include "stdafx.h"
#include <iostream>
#include "config.h"
#include "EvolutionTypes.h"
#include "individual.h"
#include "interaction.h"

using namespace std;

static short R;  // Beide kooperieren und fahren das gute Ergebnis R (Reward) ein. 
static short S;  // Der Spieler wurde betrogen, er bekommt S (Suckers Payoff). 
static short T;  // Der Spieler hat den anderen ausgenutzt und erh�lt T (Temptation).
static short P;  // Gegenseitige Defektion f�hrt nur zu P (Penalty). 

void INTERACTION::InitClass( )
{
	R = Config::GetConfigValueShort( Config::tId::interactionPayOff_R );
	S = Config::GetConfigValueShort( Config::tId::interactionPayOff_S );
	T = Config::GetConfigValueShort( Config::tId::interactionPayOff_T );
	P = Config::GetConfigValueShort( Config::tId::interactionPayOff_P );
}

static pair <short,short> GetRewards( bool const resA, bool const resB )
{
	return resA ? (resB ? make_pair( R, R ) : make_pair( S, T ))
		        : (resB ? make_pair( T, S ) : make_pair( P, P ));
}

void INTERACTION::Interact( Individual &IndA, Individual &IndB )
{
	bool const resA = IndA.InteractWith( IndB.GetId() );
	bool const resB = IndB.InteractWith( IndA.GetId() );
    
	IndB.Remember( IndA.GetId(), resA );
	IndA.Remember( IndB.GetId(), resB );

	pair <short,short> const rewards = GetRewards( resA, resB );
	
	IndA.IncEnergy( rewards.first );
	IndB.IncEnergy( rewards.second );

	int const TRACE = 0;

	if /*lint -e774 */ ( TRACE )
	{
		cout << IndA.GetId().GetLong() << "[" << GetStrategyName( IndA.GetStrategyId() ) << "]/";
		cout << IndB.GetId().GetLong() << "[" << GetStrategyName( IndB.GetStrategyId() ) << "]  "; 
		cout << resA << "/" << resB << "  ";
		cout << rewards.first << "/" << rewards.second << "  ";
		cout << IndA.GetEnergy() << "/" << IndB.GetEnergy();
		cout << endl;
	}
}
