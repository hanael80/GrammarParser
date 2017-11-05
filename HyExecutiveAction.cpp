////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyExecutiveAction.cpp
///
/// @brief	HyExecutiveAction 클래스의 소스 파일
///
/// @date	2012-11-3
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "PCH.h"
#include "HyExecutiveAction.h"
#include "HyGrammar.h"
#include "HySymbol.h"
#include "HyToken.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	생성자
///
/// @param	grammar	문법
////////////////////////////////////////////////////////////////////////////////////////////////////
HyExecutiveAction::HyExecutiveAction( const HyGrammar& grammar )
:
m_grammar( grammar )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	출력한다.
///
/// @param	depth	깊이
///
/// @return	반환 값 없음
////////////////////////////////////////////////////////////////////////////////////////////////////
HyVoid HyExecutiveAction::Print( HyUInt32 depth ) const
{
	HyChar buf[256];
	HyChar buf2[256];

	*buf = 0;
	for ( HyUInt32 i = 0; i < depth; ++i)
		strcat_s( buf, sizeof( buf ), "  " );

	*buf2 = 0;
	if ( !m_tokens.empty() )
	{
		strcpy_s( buf2, sizeof( buf2 ), " --- (" );
		for ( const HyToken* eachToken : m_tokens )
		{
			strcat_s( buf2, sizeof( buf2 ), eachToken->m_value.c_str() );
			if ( eachToken != m_tokens.back() )
				strcat_s( buf2, sizeof( buf2 ), " " );
		}
		strcat_s( buf2, sizeof( buf2 ), ")" );
	}

	LOG( HyLog::Info, "%s%s -> %s%s\n", buf, m_grammar.GetLeftSymbol().GetName().c_str(), m_grammar.GetRightString().c_str(), buf2 );

	for ( HyExecutiveAction* eachExecutiveAction : m_actions )
		eachExecutiveAction->Print( depth + 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	처리한다.
///
/// @return	작업을 성공적으로 수행했으면 true, 그렇지 않으면 false를 반환한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
HyBool HyExecutiveAction::Process() const
{
	for ( const HyExecutiveAction* eachExecutiveAction : m_actions )
		if ( !eachExecutiveAction->Process() )
			return false;

	if ( !m_grammar.GetCallBackFunction() )
		return true;

	const HyChar* tokenStrings[ 256 ];

	HyUInt32 index = 0;
	for ( const HyToken* eachToken : m_tokens )
		tokenStrings[ index++ ] = eachToken->m_value.c_str();

	tokenStrings[ index ] = NULL;
	HyBool result = true;// = m_Action.m_Grammar->m_Function(TokenString);

	return result;
}
