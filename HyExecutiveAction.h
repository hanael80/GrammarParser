////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyExecutiveAction.h
///
/// @brief	HyExecutiveAction 클래스의 헤더 파일
///
/// @date	2012-11-3
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


class HyGrammar;
class HyToken;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyExecutiveAction
///
/// @brief	HyExecutiveAction의 설명
///
/// @remark	HyExecutiveAction의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyExecutiveAction
	:
	public HyObject
{
private:
	const HyGrammar&                      m_grammar; ///< 문법
	      std::list< HyExecutiveAction* > m_actions; ///< 액션 목록
	      std::vector< const HyToken* >   m_tokens;  ///< 토큰 목록

public:
	/// 생성자
	HyExecutiveAction( const HyGrammar& grammar );

	/// 문법을 반환한다.
	const HyGrammar& GetGrammar() const { return m_grammar; }

	/// 토큰 목록을 반환한다.
	std::vector< const HyToken* >& GetTokens() { return m_tokens; }

	/// 토큰 목록을 반환한다.
	const std::vector< const HyToken* >& GetTokens() const { return m_tokens; }

	/// 출력한다.
	HyVoid Print( const HyUInt32 depth ) const;

	/// 처리한다.
	HyBool Process() const;
};
