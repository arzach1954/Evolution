// gridField.cpp :
//

#include "stdafx.h"
#include <iomanip>
#include <iomanip>
#include "config.h"
#include "gridField.h"

int   GridField::m_iFertilizerYield;
int   GridField::m_iMaxFertilizer;
short GridField::m_sFoodReserve;
short GridField::m_sMaxFood;

void GridField::InitClass( )
{
    m_iFertilizerYield = Config::GetConfigValue( Config::tId::fertilizerYield );
    m_iMaxFertilizer   = Config::GetConfigValue( Config::tId::maxFertilizer );
    m_sFoodReserve     = Config::GetConfigValueShort( Config::tId::reserveFood );
    m_sMaxFood         = Config::GetConfigValueShort( Config::tId::maxFood );
}

void GridField::InitGridFieldStructure( GridPoint const & gp )
{
    m_gp = gp;
}

void GridField::ResetGridField( short const sFood )
{
    CutConnections( );
    m_sMutatRate  = 0;
    m_sFertility  = sFood;
    m_sFoodStock  = sFood;
    m_sFertilizer = 0;
    m_Individual.ResetIndividual( );
}

void GridField::SetFertilizer( short const sNewVal )
{
    assert( sNewVal >= 0 );
    m_sFertilizer = sNewVal;
}

void GridField::SetFoodStock( short const sNewVal )
{
    assert( sNewVal >= 0 );
    m_sFoodStock = ( sNewVal > m_sFertility ) ? m_sFertility : sNewVal;
}

void GridField::SetFertility( short const sFertility )
{ 
    m_sFertility = ( sFertility > m_sMaxFood ) ? m_sMaxFood : sFertility;
}

void GridField::SetMutationRate( short const sMutRate ) 
{ 
    static short const sMax = 100;        // mutation rate is a percent value
    m_sMutatRate = ( sMutRate > sMax ) ? sMax : sMutRate;
}

void GridField::IncFertilizer( short const sInc )
{
    assert( static_cast<long>( m_sFertilizer ) + static_cast<long>( sInc ) <= static_cast<long>( SHRT_MAX ) );
    assert( static_cast<long>( m_sFertilizer ) + static_cast<long>( sInc ) >= static_cast<long>( SHRT_MIN ) );
    short sNewVal = m_sFertilizer + sInc;
    if ( sNewVal < 0 )
        sNewVal = 0;
    SetFertilizer( sNewVal );
}

void GridField::IncFoodStock( short const sInc )
{
    assert( static_cast<long>( m_sFoodStock ) + static_cast<long>( sInc ) <= static_cast<long>( SHRT_MAX ) );
    assert( static_cast<long>( m_sFoodStock ) + static_cast<long>( sInc ) >= static_cast<long>( SHRT_MIN ) );
    short sNewVal = m_sFoodStock + sInc;
    if ( sNewVal < 0 )
        sNewVal = 0;
    SetFoodStock( sNewVal );
}

void GridField::DecFoodStock( short const sDec )
{ 
    assert( static_cast<long>(m_sFoodStock) <= static_cast<long>(SHRT_MAX) + static_cast<long>(sDec) );
    assert( static_cast<long>(m_sFoodStock) >= static_cast<long>(SHRT_MIN) + static_cast<long>(sDec) );
    short sNewVal = m_sFoodStock - sDec;
    if ( sNewVal < 0 )
        sNewVal = 0;
    SetFoodStock( sNewVal ); 
}

void GridField::IncFertility( short const sInc ) 
{ 
    assert( static_cast<long>(m_sFertility) + static_cast<long>(sInc) <= static_cast<long>(SHRT_MAX) );
    assert( static_cast<long>(m_sFertility) + static_cast<long>(sInc) >= static_cast<long>(SHRT_MIN) );
    short const sNewValue     = m_sFertility + sInc;
    short const sNewFertility = (sNewValue < 0) ? 0 : sNewValue;
    SetFertility( sNewFertility ); 
}

void GridField::IncMutationRate( short const sInc ) 
{
    assert( static_cast<long>(m_sMutatRate) + static_cast<long>(sInc) <= static_cast<long>(SHRT_MAX) );
    assert( static_cast<long>(m_sMutatRate) + static_cast<long>(sInc) >= static_cast<long>(SHRT_MIN) );
    short const sNewValue = m_sMutatRate + sInc;
    SetMutationRate( (sNewValue >= 0) ? sNewValue : 0 ); 
}

short GridField::GetConsumption( short const sWant ) const 
{
    short const sReserve = m_sFoodReserve;

    return ( m_sFoodStock < sReserve ) 
           ?  0 
           : ( m_sFoodStock >= sReserve + sWant) 
             ? sWant 
             : (m_sFoodStock - sReserve);
}

void GridField::CreateIndividual( IndId const id, EVO_GENERATION const genBirth, tStrategyId const s )
{
    m_Individual.Create( id, genBirth, s );
}

void GridField::Donate( GridField & gfDonator, short sDonation )
{
    gfDonator.DecEnergy( sDonation );
    IncEnergy( sDonation );
}

void GridField::Fertilize( short const sInvest )
{
    long const lYield = (sInvest * m_iFertilizerYield ) / 100;
    assert( sInvest > 0 );
    assert( m_iMaxFertilizer <= SHRT_MAX );
    long lNewValue = m_sFertilizer + lYield;
    if ( lNewValue > m_iMaxFertilizer )
        lNewValue = m_iMaxFertilizer;
    SetFertilizer( static_cast<short>( lNewValue ) );
}

void GridField::CloneIndividual( IndId const id, EVO_GENERATION const genBirth, Random & random, GridField & gfParent )
{
    m_Individual.Clone( id, genBirth, m_sMutatRate, random, gfParent.m_Individual );
    long lDonationRate = static_cast<long>( gfParent.GetAllele( tGeneType::cloneDonation ) );
    long lParentEnergy = static_cast<long>( gfParent.GetEnergy( ) );
    long lDonation = ( lDonationRate * lParentEnergy ) / SHRT_MAX;
    assert( lDonation <= SHRT_MAX );
    Donate( gfParent, static_cast<short>( lDonation ) );
}

void GridField::BreedIndividual( IndId const id, EVO_GENERATION const genBirth, Random & random, GridField & gfParentA, GridField & gfParentB )
{
    m_Individual.Breed( id, genBirth, m_sMutatRate, random, gfParentA.m_Individual, gfParentB.m_Individual );
    Donate( gfParentA, gfParentA.GetEnergy( ) / 3 );   //TODO:  Make variable, Gene?
    Donate( gfParentB, gfParentB.GetEnergy( ) / 3 );   //TODO:  Make variable, Gene?
}

void GridField::MoveIndividual( GridField & gfSrc )
{
    m_Individual = gfSrc.m_Individual;
    gfSrc.m_Individual.ResetIndividual( );
}

void GridField::CutConnections( )
{
    m_gpJunior.Set2Null( );
    m_gpSenior.Set2Null( );
}

void GridField::Interact( GridField & gfA, GridField & gfB )
{
    INTERACTION::Interact( gfA.m_Individual, gfB.m_Individual );
    gfA.SetLastAction( tAction::interact );
};

std::wostream & operator << ( std::wostream & out, GridField const & gf )
{
//lint -e747  Significant prototype coercion with setw
    out << gf.GetGridPoint( );
    out << L" en="           << setw(8) << gf.GetEnergy( );
//  out << L" last action: " << setw(9) << GetActionTypeName( gf.GetLastAction() );
//lint +e747 
    return out;
}

