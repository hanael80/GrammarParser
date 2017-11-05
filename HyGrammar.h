////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyGrammar.h
///
/// @brief	HyGrammar 클래스의 헤더 파일
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
/// @brief	HyGrammar의 설명
///
/// @remark	HyGrammar의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyGrammar
	:
	public HyObject
{
private:
	HyUInt32                       m_id;               ///< 식별자
	GENERATION_CALLBACK            m_callBackFunction; ///< 콜백 함수
	HySymbol&                      m_leftSymbol;       ///< 왼쪽 심볼

public:
	HyString                       m_rightString;      ///< 오른쪽 문자열
	std::vector< const HySymbol* > m_rightSymbols;     ///< 오른쪽 심볼 목록
	HyUInt32                       m_endPosition;      ///< 끝 위치

public:
	friend class HyGrammarPosition;

	/// 생성자
	HyGrammar( HySymbol& headSymbol );

	/// 식별자를 설정한다.
	HyVoid SetId( HyUInt32 id ) { m_id = id; }

	/// 콜백 함수를 설정한다.
	HyVoid SetCallBackFunction( GENERATION_CALLBACK callBackFunction ) { m_callBackFunction = callBackFunction; }

	/// 식별자를 반환한다.
	HyUInt32 GetId() const { return m_id; }

	/// 콜백 함수를 반환한다.
	GENERATION_CALLBACK GetCallBackFunction() const { return m_callBackFunction; }

	/// 왼쪽 심볼을 반환한다.
	HySymbol& GetLeftSymbol() { return m_leftSymbol; }

	/// 왼쪽 심볼을 반환한다.
	const HySymbol& GetLeftSymbol() const { return m_leftSymbol; }

	/// 오른쪽 문자열을 반환한다.
	const HyString& GetRightString() const { return m_rightString; }

	/// 오른쪽 심볼 목록을 반환한다.
	std::vector< const HySymbol* >& GetRightSymbols() { return m_rightSymbols; }

	/// 오른쪽 심볼 목록을 반환한다.
	const std::vector< const HySymbol* >& GetRightSymbols() const { return m_rightSymbols; }

	/// 끝 위치를 반환한다.
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
/// @brief	HyGrammarPosition의 설명
///
/// @remark	HyGrammarPosition의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyGrammarPosition
{
public:
	/// 생성자
	HyGrammarPosition( const HyGrammar& grammar, HyUInt32 position )
	:
	m_grammar  ( grammar ),
	m_position ( position )
	{
	}

	/// 끝 위치인지의 여부를 반환한다.
	HyBool IsEndPosition() const { return m_grammar.GetEndPosition() == m_position; }

	/// 현재 심볼을 반환한다.
	const HySymbol* GetCurSymbol() const;

	static std::list< HyGrammarPosition >& GetInputGrammarPositionInstance();

	HyBool operator == ( const HyGrammarPosition& grammarPosition ) const { return m_grammar == grammarPosition.m_grammar && m_position == grammarPosition.m_position; }

public:
	const HyGrammar& m_grammar;  ///< 문법
	HyUInt32         m_position; ///< 위치
};
