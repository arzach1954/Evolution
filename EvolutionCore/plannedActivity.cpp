// plannedActivity.cpp
//
// EvolutionCore

#include "stdafx.h"
#include <fstream>
#include <iomanip>
#include "plannedActivity.h"

using namespace std;

wostream & operator << ( wostream & out, PlannedActivity const & plan )
{
//lint -e747  Significant prototype coercion with setw

   out << L" action=" << setw(8) << GetActionTypeName( plan.GetActionType( ) );

   if ( plan.GetTarget( ).IsNull( ) )
       out << setw( 15 ) << L" ";
   else
       out << L" target=" << setw( 3 ) << plan.GetTarget( );

   if ( plan.GetPartner( ).IsNull( ) )
       out << setw( 15 ) << L" ";
   else
       out << L" target=" << setw( 3 ) << plan.GetPartner( );

   return out;
//lint +e747 
}