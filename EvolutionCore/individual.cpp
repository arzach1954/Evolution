// individual.cpp : 
//

#include "stdafx.h"
#include <unordered_map>
#include "assert.h"
#include "debug.h"
#include "random.h"
#include "config.h"
#include "strategy.h"
#include "individual.h"

static DefectAlways    StratD;
static CooperateAlways StratC;
static Tit4Tat         StratT;
static EmptyStrategy   StratNull;

short Individual::m_sStdEnergyCapacity;
short Individual::m_sInitialEnergy;

const std::unordered_map< tStrategyId, Strategy * const > Individual::m_apStrat =
{ 
	{ tStrategyId::defect,    &StratD },
	{ tStrategyId::cooperate, &StratC },
	{ tStrategyId::tit4tat,   &StratT }
};

void Individual::InitClass( )
{
    m_sStdEnergyCapacity = Config::GetConfigValueShort( Config::tId::stdCapacity );
    m_sInitialEnergy     = Config::GetConfigValueShort( Config::tId::initialEnergy );
}

Individual::Individual( )
  : m_id( ),
    m_strat( ),
    m_genome( ),
	m_pStrategy( & StratNull )
{ 
    ResetIndividual( );
}

void Individual::ResetIndividual( )
{ 
    m_id.ResetIndId( ); 
    m_genBirth  = -1L;
    m_sEnergy   = 0;
    m_sCapacity = 0;
    m_strat.SetMemorySize( 0 );
    m_pStrategy = & StratNull;
    m_genome.InitGenome( );
};

void Individual::Create
( 
    IndId          const id,
    EVO_GENERATION const genBirth,
    tStrategyId    const strategyId
)
{
    m_pStrategy  = m_apStrat.at( strategyId );
    m_id         = id;
    m_genBirth   = genBirth;
    m_origin     = tOrigin::editor;
    m_genome.InitGenome( );
    m_strat.SetMemorySize( static_cast<MEM_INDEX>(m_genome.GetAllele(tGeneType::memSize)) );
    m_sCapacity = m_sStdEnergyCapacity;
    SetEnergy( m_sInitialEnergy ); // makes IsAlive() true. Last assignment to avoid race conditions  
}

// Clone - creates a mutated clone of this object
//         all member variables of new individual are initialized after this function

void Individual::Clone
(
    IndId          const   id,
    EVO_GENERATION const   genBirth,
    short          const   sMutationRate,
    Random               & random,
    Individual     const & indParent
)
{
    m_id        = id;
    m_genBirth  = genBirth;
    m_origin    = tOrigin::cloning;
    m_sCapacity = indParent.m_sCapacity;
    m_pStrategy = indParent.m_pStrategy;
    m_at        = tAction::undefined;
    m_strat.SetMemorySize( indParent.m_strat.GetMemSize( ) );  // clears memory. Experience not inheritable.
    m_genome.Mutate( sMutationRate, random );
}

static Individual const & selectParent
(
    Random           & random,
    Individual const & indParA,
    Individual const & indParB
)
{
    return random.NextBooleanValue( ) ? indParA : indParB;
}

// Breed - creates a child with a mix of genes of both parents
//         all member variables of new individual are initialized after this function

void Individual::Breed
(
    IndId          const   id,
    EVO_GENERATION const   genBirth,
    short          const   sMutationRate,
    Random               & random,
    Individual     const & indParentA,
    Individual     const & indParentB
)
{
    m_id        =          id;
    m_genBirth  =          genBirth;
    m_origin    =          tOrigin::marriage;
    m_at        =          tAction::undefined;
    m_sCapacity =          selectParent( random, indParentA, indParentB ).m_sCapacity;
    m_pStrategy =          selectParent( random, indParentA, indParentB ).m_pStrategy;
    m_strat.SetMemorySize( selectParent( random, indParentA, indParentB ).GetMemSize( ) );  // clears memory. Experience not inheritable.
    m_genome.Recombine( indParentA.m_genome, indParentB.m_genome, random );
    m_genome.Mutate( sMutationRate, random );
}
