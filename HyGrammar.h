////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyGrammar.h
///
/// @brief	HyGrammar Ŭ������ ��� ����
///
/// @date	2012-11-3
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyCode.hpp"
#include "HyToken.h"


typedef void ( *GENERATION_CALLBACK )( const std::vector<const HyToken*>& vecToken, void* const pParam );


class HySymbol;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyGrammar
///
/// @brief	HyGrammar�� ����
///
/// @remark	HyGrammar�� ����
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyGrammar
	:
	public HyObject
{
private:
	HyUInt32                       m_id;               ///< �ĺ���
	GENERATION_CALLBACK            m_callBackFunction; ///< �ݹ� �Լ�
	HySymbol&                      m_leftSymbol;       ///< ���� �ɺ�

public:
	HyString                       m_rightString;      ///< ������ ���ڿ�
	std::vector< const HySymbol* > m_rightSymbols;     ///< ������ �ɺ� ���
	HyUInt32                       m_endPosition;      ///< �� ��ġ

public:
	friend class HyGrammarPosition;

	/// ������
	HyGrammar( HySymbol& headSymbol );

	/// �ĺ��ڸ� �����Ѵ�.
	HyVoid SetId( HyUInt32 id ) { m_id = id; }

	/// �ݹ� �Լ��� �����Ѵ�.
	HyVoid SetCallBackFunction( GENERATION_CALLBACK callBackFunction ) { m_callBackFunction = callBackFunction; }

	/// �ĺ��ڸ� ��ȯ�Ѵ�.
	HyUInt32 GetId() const { return m_id; }

	/// �ݹ� �Լ��� ��ȯ�Ѵ�.
	GENERATION_CALLBACK GetCallBackFunction() const { return m_callBackFunction; }

	/// ���� �ɺ��� ��ȯ�Ѵ�.
	HySymbol& GetLeftSymbol() { return m_leftSymbol; }

	/// ���� �ɺ��� ��ȯ�Ѵ�.
	const HySymbol& GetLeftSymbol() const { return m_leftSymbol; }

	/// ������ ���ڿ��� ��ȯ�Ѵ�.
	const HyString& GetRightString() const { return m_rightString; }

	/// ������ �ɺ� ����� ��ȯ�Ѵ�.
	std::vector< const HySymbol* >& GetRightSymbols() { return m_rightSymbols; }

	/// ������ �ɺ� ����� ��ȯ�Ѵ�.
	const std::vector< const HySymbol* >& GetRightSymbols() const { return m_rightSymbols; }

	/// �� ��ġ�� ��ȯ�Ѵ�.
	HyUInt32 GetEndPosition() const { return m_endPosition; }

	HyInt32 GetNumber( const std::list< HyGrammar >& inputGrammars ) const;
	HyUInt32 GetRightSymbolCount() const;
	HyUInt32 GetRightTerminalSymbolCount() const;

	bool operator ==(const HyGrammar& grammar) const;

	bool HasTerminalDerivation() const;

	void GetSubGrammars(const std::list<HyGrammar>& listInputGrammar, std::set<const HyGrammar*>& setGrammar) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyGrammarPosition
///
/// @brief	HyGrammarPosition�� ����
///
/// @remark	HyGrammarPosition�� ����
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyGrammarPosition
{
public:
	/// ������
	HyGrammarPosition( const HyGrammar& grammar, HyUInt32 position )
	:
	m_grammar  ( grammar ),
	m_position ( position )
	{
	}

	/// �� ��ġ������ ���θ� ��ȯ�Ѵ�.
	HyBool IsEndPosition() const { return m_grammar.GetEndPosition() == m_position; }

	/// ���� �ɺ��� ��ȯ�Ѵ�.
	const HySymbol* GetCurSymbol() const;

	static std::list< HyGrammarPosition >& GetInputGrammarPositionInstance();

	HyBool operator == ( const HyGrammarPosition& grammarPosition ) const { return m_grammar == grammarPosition.m_grammar && m_position == grammarPosition.m_position; }

public:
	const HyGrammar& m_grammar;  ///< ����
	HyUInt32         m_position; ///< ��ġ
};
