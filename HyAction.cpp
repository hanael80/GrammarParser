#include "PCH.h"
#include "HyAction.h"
#include "HyGrammar.h"
#include "HySymbol.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	생성자
///
/// @param	symbol	심볼
////////////////////////////////////////////////////////////////////////////////////////////////////
HyAction::HyAction( const HySymbol& symbol )
:
m_grammar ( NULL ),
m_symbol   ( symbol )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	문자열로 변환한다.
///
/// @param	buffer	버퍼
///
/// @return	문자열
////////////////////////////////////////////////////////////////////////////////////////////////////
const HyChar* HyAction::ToString( HyChar* buffer ) const
{
	switch( m_type )
	{
	case Type::Shift:
		strcpy( buffer, "shift" );
		break;
	case Type::Reduce:
		sprintf( buffer, "reduce %s →%s", m_grammar->GetLeftSymbol().GetName().c_str(), m_grammar->GetRightString().c_str());
		break;
	case Type::Goto:
		strcpy( buffer, "shift" );
		break;
	case Type::Accept:
		strcpy( buffer, "accept" );
		break;
	default:
		LOG( HyLog::Error, "Invalid Type(%d)", m_type );
		strcpy( buffer, "ERROR" );
		break;
	}

	return buffer;
}
