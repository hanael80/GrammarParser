////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HySymbol.h
///
/// @brief	HySymbol 클래스의 헤더 파일
///
/// @date	2012-11-3
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyGrammar.h"
#include <set>


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HySymbol
///
/// @brief	HySymbol의 설명
///
/// @remark	HySymbol의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HySymbol
	:
	public HyObject
{
public:
	enum Type
	{
		typeTerminal,
		typeNonterminal
	};

private:
	HyUInt32             m_id;      ///< 식별자
	HyString             m_name;    ///< 이름
	Type                 m_type;    ///< 종류
	std::set< HyString > m_firsts;  ///< 퍼스트 목록
	std::set< HyString > m_follows; ///< 팔로우 목록

public:
	/// 식별자를 설정한다.
 	HyVoid SetId( HyUInt32 id ) { m_id = id; }

	/// 이름을 설정한다.
 	HyVoid SetName( const HyString& name ) { m_name = name; }

	/// 종류를 설정한다.
 	HyVoid SetType( Type type ) { m_type = type; }

	/// 식별자를 반환한다.
 	HyUInt32 GetId() const { return m_id; }

	/// 이름을 반환한다.
 	const HyString& GetName() const { return m_name; }

	/// 종류를 반환한다.
 	Type GetType() const { return m_type; }

	/// 퍼스트 목록을 반환한다.
 	const std::set< HyString >& GetFirsts() const { return m_firsts; }

	/// 팔로우 목록을 반환한다.
 	const std::set< HyString >& GetFollows() const { return m_follows; }

	HyBool AddFirst(  const HyString& string );
	HyBool AddFirst( const std::map< HyString, HySymbol* >& symbols, const HyString& string );
	HyBool AddFollow( const HyString& string );
	HyBool AddFollow( const std::map< HyString, HySymbol* >& symbols, const HyString& string );
	HyBool HasFirst( const HyString& string ) const;
	HyBool HasFollow( const HyString& string ) const;
};
