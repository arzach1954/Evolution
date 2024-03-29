// action.h
//
// EvolutionCoreInterface

#pragma once

#include <array>
#include <functional>

class Action
{
public:
	enum class Id : unsigned short
	{ 
		move, 
		clone,
		marry,
		interact, 
		eat, 
		fertilize,
		passOn,
		undefined,
		count
	};

	static int constexpr COUNT = static_cast<int>( Id::count );

	static void Apply2AllEnabledActions( std::function<void(Id const &)> const & );

	static bool IsDefined( Id const action )
	{
		return action != Id::undefined;
	}

	static bool IsUndefined( Id const action )
	{
		return action == Id::undefined;
	}

	static wchar_t const * const GetName( Id const );
};
