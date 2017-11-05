////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HySymbol.cpp
///
/// @brief	HySymbol Ŭ������ �ҽ� ����
///
/// @date	2012-11-3
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "PCH.h"
#include "HySymbol.h"


HyBool HySymbol::AddFirst( const HyString& string )
{
	return m_firsts.insert( string ).second;
}

HyBool HySymbol::AddFirst( const std::map< HyString, HySymbol* >& symbols, const HyString& string )
{
	if ( symbols.find( string ) == symbols.end() && string != "$" )
		return false;

	return m_firsts.insert( string ).second;
}

HyBool HySymbol::AddFollow( const HyString& string )
{
	return m_follows.insert( string ).second;
}

HyBool HySymbol::AddFollow( const std::map< HyString, HySymbol* >& symbols, const HyString& string )
{
	if ( symbols.find( string ) == symbols.end() && string != "$" )
		return false;

	return m_follows.insert( string ).second;
}

HyBool HySymbol::HasFirst( const HyString& string ) const
{
	return m_firsts.find( string ) != m_firsts.end();
}

HyBool HySymbol::HasFollow( const HyString& string ) const
{
	return m_follows.find( string ) != m_follows.end();
}
