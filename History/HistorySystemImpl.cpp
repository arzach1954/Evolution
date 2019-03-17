// HistorySystemImpl.cpp
//

#include "stdafx.h"
#include "assert.h"
#include "hist_slot.h"
#include "HistSlotNr.h"
#include "historyCache.h"
#include "historyIterator.h"
#include "HistoryGeneration.h"
#include "ObserverInterface.h"
#include "HistorySystemImpl.h"

// public member functions

HistorySystemImpl::HistorySystemImpl( ) :
    m_GenCmdList        ( ),
    m_pHistoryCache     ( nullptr ),
    m_pHistCacheItemWork( nullptr )
{ }

HistorySystemImpl::~HistorySystemImpl( )
{
    delete m_pHistoryCache;

    m_pHistoryCache      = nullptr;
    m_pHistCacheItemWork = nullptr;
}

void HistorySystemImpl::InitHistorySystem
(
    HIST_GENERATION     const genMaxNrOfGens,
    ModelData         * const pModelDataWork
)
{
    m_pHistoryCache      = new HistoryCache;
	m_pHistCacheItemWork = new HistCacheItem( pModelDataWork );
	m_GenCmdList.Resize( genMaxNrOfGens );
}

ModelData const * HistorySystemImpl::StartHistorySystem
(
    HistSlotNr          const nrOfSlots,
    ModelFactory      * const pModelFactory,
	ObserverInterface * const pObserver,
	GenerationCmd       const cmd
)
{
    m_pHistoryCache->InitHistoryCache( nrOfSlots, pModelFactory, pObserver );
	m_pHistCacheItemWork->SetGenerationCommand( cmd );
    return save2History( );
}

// ClearHistory
// Erase all generations starting with genFirst up to youngest generation
// from m_GenerationCmd and from history slots

void HistorySystemImpl::ClearHistory( HIST_GENERATION const genFirst )
{
	for ( HIST_GENERATION gen = GetYoungestGeneration(); gen > genFirst; --gen )
	{
		if  ( m_GenCmdList.IsCachedGeneration( gen ) )
			m_pHistoryCache->RemoveHistCacheSlot( m_GenCmdList[gen].GetSlotNr( ) );

		m_GenCmdList.ResetGenerationCmd( gen );
	}
}

bool HistorySystemImpl::AddHistorySlot( ) const 
{ 
	return m_pHistoryCache->AddCacheSlot( );
}

HistoryIterator * HistorySystemImpl::CreateHistoryIterator( ) const 
{ 
	return new HistoryIterator( m_pHistoryCache );     	 
}

ModelData const * HistorySystemImpl::CreateAppCommand( GenerationCmd const genCmd )
{
	step2NextGeneration( genCmd );
	m_pHistCacheItemWork->SetGenerationCommand( genCmd );
	return save2History( );
}

HistCacheItem const * HistorySystemImpl::getCachedItem( GenerationCmd cmd )
{
	HistSlotNr const sSlotNr = cmd.GetSlotNr( );
    return m_pHistoryCache->GetHistCacheItemC( sSlotNr );
}

// ApproachHistGen - Get closer to demanded HIST_GENERATION
//                 - If several steps are neccessary, function returns after one displayed generation
//                   to allow user interaction
//                 - But actual history generation as altered by at least 1

void HistorySystemImpl::ApproachHistGen( HIST_GENERATION const genDemanded )
{
    HIST_GENERATION genActual = m_pHistCacheItemWork->GetHistGenCounter( );

    assert( genDemanded != genActual );
	assert( genDemanded <= m_GenCmdList.GetMaxGeneration( ) );
	assert( m_GenCmdList[ 0 ].IsCachedGeneration( ) );      // at least initial generation is cached

    HIST_GENERATION genCached   = genDemanded;  // search backwards starting with genDemanded
    BOOL            bMicrosteps = TRUE;
        
    while ( m_GenCmdList[ genCached ].IsNotCachedGeneration( ) )
        --genCached;

    // now we have found a cached generation  

    if ( 
          (( genCached <= genActual ) && ( genActual < genDemanded )) || // cached generation is not better than actual generation
          (( genActual == genDemanded - 1 ) && bMicrosteps )
       )
    {
		step2NextGeneration( m_GenCmdList[ genActual + 1 ] );   // compute forwards
    }
    else  // get cached generation
    {
        m_pHistCacheItemWork->CopyCacheItem( getCachedItem( m_GenCmdList[ genCached ] ) );
    }
}

tGenCmd HistorySystemImpl::GetGenerationCmd( HIST_GENERATION const gen )
{
	GenerationCmd cmd = m_GenCmdList[ gen ];

	if ( cmd.IsCachedGeneration( ) )
	{
		cmd = getCachedItem( cmd )->GetGenCmd( );
	}

	return cmd.GetCommand( );
}

// private member functions

// save2History - Save Generation data (rule how to get to next generation), current Grid, etc. to new slot

ModelData const * HistorySystemImpl::save2History( )
{
    HistSlotNr    const   slotNr          = m_pHistoryCache->GetFreeCacheSlot( );
    HistCacheItem const * pHistCacheItem  = m_pHistoryCache->GetHistCacheItemC( slotNr );
    GenerationCmd const   genCmdFromCache = pHistCacheItem->GetGenCmd( );

    if ( genCmdFromCache.IsDefined( ) )       // Hist slot was in use before. Save GenCommand
    {
        HIST_GENERATION const genCached = pHistCacheItem->GetHistGenCounter( );
        assert( m_GenCmdList[ genCached ].IsCachedGeneration( ) );
        assert( m_GenCmdList[ genCached ].GetSlotNr( ) == slotNr );
        m_GenCmdList.SetGenerationCmd( genCached, genCmdFromCache );
        m_pHistoryCache->ResetHistCacheSlot( slotNr );
    }

    CHECK_HISTORY_STRUCTURE;
    ModelData const * pModelData = m_pHistoryCache->Save2CacheSlot( * m_pHistCacheItemWork, slotNr );
    m_GenCmdList.SetCachedGeneration( m_pHistCacheItemWork->GetHistGenCounter( ), slotNr );
    CHECK_HISTORY_STRUCTURE;

	return pModelData;
};

// step2NextGeneration - if cached generation: get GenerationCmd from cache
//                     - apply command and increment history generation counter
// throws exception if maximum number of generations reached

void HistorySystemImpl::step2NextGeneration( GenerationCmd genCmd )
{
    if ( genCmd.IsCachedGeneration( ) ) // can happen only in history mode
    {
        assert( IsInHistoryMode( ) );
        HistSlotNr    const   slotNr        = genCmd.GetSlotNr( );
        HistCacheItem const * pHistCacheItem = m_pHistoryCache->GetHistCacheItemC( slotNr );
        genCmd = pHistCacheItem->GetGenCmd( );
    }

    if ( m_pHistCacheItemWork->GetHistGenCounter() >= m_GenCmdList.GetMaxGeneration() )
		throw HistoryBufferException();

	m_pHistCacheItemWork->GetModelData()->OnAppCommand( genCmd );    // Apply application defined operation to step to next generation
	m_pHistCacheItemWork->IncHistGenCounter( );
}

HIST_GENERATION HistorySystemImpl::FindGenerationWithProperty
( 
	GenerationProperty const & property,
	bool               const   bReverse   // if true, search from youngest to oldest generation
) const
{
    HistoryIterator histIter( m_pHistoryCache );

    for ( 
			HistSlotNr slotNr = bReverse ? histIter.Set2Youngest( ) : histIter.Set2Oldest( ); 
			slotNr.IsNotNull(); 
			slotNr = bReverse ? histIter.Set2Senior( ) : histIter.Set2Junior( ) 
		)
    {
        if ( (property)( m_pHistoryCache->GetHistCacheItemC( slotNr )->GetModelDataC( ) ) )
        {
            return m_pHistoryCache->GetGridGen( slotNr );
        }
    }

    return HIST_GENERATION();   // NULL_VAL: no generation found with property
}

void HistorySystemImpl::checkHistoryStructure( )  // used only in debug mode
{
     // check generation number of all generations in history cache

//	wcout << L"**** checkHistoryStructure" << endl;
//	wcout << L"m_genCmdList" << endl;

    for ( HIST_GENERATION gen = 0; gen <= m_GenCmdList.GetMaxGeneration( ); ++gen )
    {
        GenerationCmd generationCmd = m_GenCmdList[ gen ];
//		if ( generationCmd.IsDefined( ) )
//			wcout << gen 
//			      << L" (" << generationCmd.GetCommand() 
//				  << L"," << generationCmd.GetParam()
//			      << L")";
        if ( generationCmd.IsCachedGeneration( ) )
        {
            HistSlotNr      const   slotNrFromList  = generationCmd.GetSlotNr( );
            HistCacheItem   const * pHistCacheItem  = m_pHistoryCache->GetHistCacheItemC( slotNrFromList );
            HIST_GENERATION const   genNrFromCache  = pHistCacheItem->GetHistGenCounter( );
	        GenerationCmd   const   genCmdFromCache = pHistCacheItem->GetGenCmd( );
            assert( genNrFromCache == gen );
//			wcout << L" cache: slot "
//				  << sSlotNrFromList
//	              << L" (" << genCmdFromCache.GetCommand() << L"," << genCmdFromCache.GetParam() << L") "
//				  << genNrFromCache << endl;
        }
        else if ( generationCmd.IsUndefined( ) )
            break;
//		else
//			wcout << endl;
    }

    // check slot numbers and according generation commands

//	wcout << L"slots" << L"(" << m_pHistoryCache->GetNrOfHistCacheSlots( ) << L")" << endl;

    for ( HistSlotNr slotNr = HistSlotNr(0); slotNr < m_pHistoryCache->GetNrOfHistCacheSlots( ); ++slotNr )
    {
        HistCacheItem   const * pHistCacheItem  = m_pHistoryCache->GetHistCacheItemC( slotNr );
        HIST_GENERATION const   genNrFromCache  = pHistCacheItem->GetHistGenCounter( );
        GenerationCmd   const   genCmdFromCache = pHistCacheItem->GetGenCmd( );
        if ( genCmdFromCache.IsDefined( ) )
        {
            GenerationCmd const generationCmd   = m_GenCmdList[ genNrFromCache ];
            HistSlotNr    const slotNrFromList = generationCmd.GetSlotNr( );
//			wcout << L"slot " << sSlotNr 
//				  << L" (" << genCmdFromCache.GetCommand() << L"," << genCmdFromCache.GetParam() << L") "
//				  << L"gen " << genNrFromCache
//				  << L" slotNrFromList= " << slotNrFromList 
//				  << endl;
            assert( generationCmd.IsCachedGeneration( ) );
            assert( slotNrFromList == slotNr );
        }
    }
}