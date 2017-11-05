////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HySymbol.h
///
/// @brief	HySymbol Ŭ������ ��� ����
///
/// @date	2012-11-3
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyGrammar.h"
#include <set>


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HySymbol
///
/// @brief	HySymbol�� ����
///
/// @remark	HySymbol�� ����
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
	HyUInt32             m_id;      ///< �ĺ���
	HyString             m_name;    ///< �̸�
	Type                 m_type;    ///< ����
	std::set< HyString > m_firsts;  ///< �۽�Ʈ ���
	std::set< HyString > m_follows; ///< �ȷο� ���

public:
	/// �ĺ��ڸ� �����Ѵ�.
 	HyVoid SetId( HyUInt32 id ) { m_id = id; }

	/// �̸��� �����Ѵ�.
 	HyVoid SetName( const HyString& name ) { m_name = name; }

	/// ������ �����Ѵ�.
 	HyVoid SetType( Type type ) { m_type = type; }

	/// �ĺ��ڸ� ��ȯ�Ѵ�.
 	HyUInt32 GetId() const { return m_id; }

	/// �̸��� ��ȯ�Ѵ�.
 	const HyString& GetName() const { return m_name; }

	/// ������ ��ȯ�Ѵ�.
 	Type GetType() const { return m_type; }

	/// �۽�Ʈ ����� ��ȯ�Ѵ�.
 	const std::set< HyString >& GetFirsts() const { return m_firsts; }

	/// �ȷο� ����� ��ȯ�Ѵ�.
 	const std::set< HyString >& GetFollows() const { return m_follows; }

	HyBool AddFirst(  const HyString& string );
	HyBool AddFirst( const std::map< HyString, HySymbol* >& symbols, const HyString& string );
	HyBool AddFollow( const HyString& string );
	HyBool AddFollow( const std::map< HyString, HySymbol* >& symbols, const HyString& string );
	HyBool HasFirst( const HyString& string ) const;
	HyBool HasFollow( const HyString& string ) const;
};
