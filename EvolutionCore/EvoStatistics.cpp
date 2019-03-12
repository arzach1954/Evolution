// EvoStatistics.cpp :
//
// EvolutionCore

#include "stdafx.h"
#include "EvoStatistics.h"

EvoStatistics::EvoStatistics
( 
	EvolutionCore const * const pCore
) 
{
	m_pCore = pCore;
}

EvoStatistics::~EvoStatistics()
{
	m_pCore = nullptr;
	m_pTextBuf = nullptr;
}

void EvoStatistics::Initialize( TextBuffer * const pTextBuf )
{
	m_pTextBuf = pTextBuf;

	m_gsCounter.zero();         
    m_gsAverageAge.zero();      
	m_axaGenePoolStrategy.Apply2All( [&](XaFloatStat  & elem) { elem.zero(); } );
	m_aGeneStat          .Apply2All( [&](XaCounter    & elem) { elem.zero(); } );
	m_auiMemSize         .Apply2All( [&](unsigned int & elem) { elem = 0; } );

	Apply2Rect
	( 
		[&](GridPoint const gp)
		{
			if ( m_pCore->IsAlive( gp ) )
				aquireData( gp );
		},
		m_pCore->GetSelection( )
	);

	scale( );
};

void EvoStatistics::add2option( Strategy::Id const strategy, Action::Id const action, short const sValue )
{
    m_axaGenePoolStrategy[action].Add( strategy, static_cast<float>( sValue ) );
}

void EvoStatistics::add2Gene( Strategy::Id const strategy, GeneType::Id const geneType, long const lGenoType )
{
    assert( lGenoType >= 0 );
    m_aGeneStat[geneType].Add( strategy, CastToUnsignedInt( lGenoType ) );
}

void EvoStatistics::addMemSize( Strategy::Id const strategy, MEM_INDEX const memSize )
{
    m_auiMemSize[strategy] += memSize.GetValue();
}

void EvoStatistics::addAge( Strategy::Id const strategy, EVO_GENERATION genAge )
{
    m_gsAverageAge.Add( strategy, genAge.GetValue() );
}

void EvoStatistics::incCounter( Strategy::Id const strategy )
{
    m_gsCounter.Add( strategy, 1 );
}

void EvoStatistics::aquireData( GridPoint const & gp )
{
	Strategy::Id   const strategy { m_pCore->GetStrategyId( gp ) };
	EVO_GENERATION const age      { m_pCore->GetAge( gp ) };
	MEM_INDEX      const memSize  { m_pCore->GetMemSize( gp ) };

	if (  // can happen due to race conditions 
			( strategy != Strategy::Id::empty ) &&
			( age.IsNotNull()  )
		)
	{
		GeneType::Apply2All
		(
			[&]( GeneType::Id geneType )
			{
				Action::Id action { GetRelatedAction( geneType ) };
				if ( EvolutionCore::IsEnabled( action ) )
					add2option( strategy, action, m_pCore->GetDistr( gp, geneType ) );
			}
		);

		GeneType::Apply2All
		(
			[&]( auto geneType )
			{ add2Gene( strategy, geneType, m_pCore->GetGenotype( gp, geneType ) ); }
		);

		addMemSize( strategy, memSize );
		addAge    ( strategy, age );
		incCounter( strategy );
	}
}

void EvoStatistics::scale( )
{
	m_gsAverageAge.DivNonZero( m_gsCounter );

	GeneType::Apply2All
	(
		[&]( GeneType::Id geneType )
		{
			Strategy::Apply2All
			(
				[&]( Strategy::Id strategy )
				{
					DivNonZero( m_aGeneStat[geneType][strategy], m_gsCounter[strategy] );
				}
			);
			DivNonZero( m_aGeneStat[geneType].General(), m_gsCounter.General() );
		}
	);

	float fSum { 0.0 };
	m_axaGenePoolStrategy.Apply2All( [&](XaFloatStat & elem) { fSum += elem.General(); } );
	m_axaGenePoolStrategy.Apply2All( [&](XaFloatStat & elem) { Scale( elem.General(), fSum ); } );
	
	Strategy::Apply2All
	( 
		[&]( Strategy::Id strategy )
		{
			float fSum { 0.0 };
			m_axaGenePoolStrategy.Apply2All( [&](XaFloatStat & elem) { fSum += elem[strategy]; } );
			m_axaGenePoolStrategy.Apply2All( [&](XaFloatStat & elem) { Scale(  elem[strategy], fSum ); } );
		}
	);

	Strategy::Apply2All
	( 
		[&]( Strategy::Id strategy )
		{ DivNonZero( m_auiMemSize[strategy], m_gsCounter[strategy] ); }
	);
};

void EvoStatistics::printHeader( )
{
	Strategy::Apply2All
	( 
		[&]( Strategy::Id const & id ) 
		{ m_pTextBuf->printString( Strategy::GetName( id ) ); }
	);
	m_pTextBuf->printString( L"all" );                               
}

void EvoStatistics::printCounter( wchar_t const * const data )
{
    m_gsCounter.printGeneLine( m_pTextBuf, data );
}

void EvoStatistics::printAvAge( wchar_t const * const data )
{
    m_gsAverageAge.printGeneLine( m_pTextBuf, data );
}

void EvoStatistics::printCounters( Action::Id const action )
{
	ACTION_COUNT sum { 0 };
	m_pTextBuf->nextLine( Action::GetName( action ) );
	Strategy::Apply2All
	( 
		[&]( Strategy::Id strat )
		{
			ACTION_COUNT counter { m_pCore->GetActionCounter( strat, action ) };
			m_pTextBuf->printNumber( counter.GetValue() );
			sum += counter;
		}
	);
	m_pTextBuf->printNumber( sum.GetValue() );
}

void EvoStatistics::printIncidence( )
{
	Action::Apply2All
	(
		[&]( Action::Id action ) -> Action::Id
		{
			if ( EvolutionCore::IsEnabled( action ) )
			{
				printCounters( action );
			}
			return Action::Id::undefined;
		}
	);
}

void EvoStatistics::printProbabilities( )
{
	Action::Apply2All
	(
		[&]( Action::Id action ) -> Action::Id
		{
			if ( EvolutionCore::IsEnabled( action ) )
			{
				if ( GeneType::IsDefined( GetRelatedGeneType( action ) )  )
					m_axaGenePoolStrategy[action].printFloatLine( m_pTextBuf, Action::GetName( action ) );
			}
			return Action::Id::undefined;
		}
	);

}

void EvoStatistics::printGeneStat(  )
{
	GeneType::Apply2All
	(
		[&]( GeneType::Id geneType )
		{
			Action::Id action { GetRelatedAction( geneType ) };
			if ( EvolutionCore::IsEnabled( action ) )
				m_aGeneStat[geneType].printGeneLine( m_pTextBuf, GeneType::GetName( geneType ) );
		}
	);
}

void EvoStatistics::printAvFood(  wchar_t const * const data )
{
    XaFloatStat fsAvFood;

	Strategy::Apply2All
	( 
		[&]( Strategy::Id strategy )
		{ 
			fsAvFood[ strategy ] = m_aGeneStat[ GeneType::Id::appetite ][ strategy ] * 
				                    m_axaGenePoolStrategy[ Action::Id::eat ][ strategy ] / 100; 
		}
	);

    fsAvFood.General() = m_aGeneStat[ GeneType::Id::appetite ].General() * m_axaGenePoolStrategy[ Action::Id::eat ].General() / 100 ;

    fsAvFood.printFloatLine( m_pTextBuf, data );
}

void EvoStatistics::printMemory( wchar_t const * const data )
{
    m_pTextBuf->nextLine( data );

	Strategy::Apply2All([&](Strategy::Id strategy) { m_pTextBuf->printNumber(m_auiMemSize[strategy]); });
}