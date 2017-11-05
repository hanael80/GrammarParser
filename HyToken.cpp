#include "PCH.h"
#include "HyToken.h"
#include <string>


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	»ý¼ºÀÚ
////////////////////////////////////////////////////////////////////////////////////////////////////
HyToken::HyToken()
:
m_type ( typeNothing ),
m_line( 0 )
{
}

void HyToken::Init()
{
	AddTokenType(typeFinal, "$");
}

void HyToken::AddTokenType(const char* const pszString)
{
	const int iType = HyToken::typeFinal + (int)HyToken::IntStringTypes().size() + 1;
	AddTokenType(iType, pszString);
}

void HyToken::AddTokenType( int iType, const char* String )
{
	IntStringTypes().insert( std::make_pair( iType, String ) );
	StringIntTypes().insert( std::make_pair( String, iType ) );
}

const char* HyToken::GetInput( std::list< HyToken >::iterator it, const std::list< HyToken >::iterator end, char* pszBuffer, int iBufferSize )
{
	*pszBuffer = 0;

	for ( ; it != end; ++it )
	{
		const HyToken& Tok = *it;

		strcat_s( pszBuffer, iBufferSize, Tok.GetTypeString() );
		strcat_s( pszBuffer, iBufferSize, " " );
	}

	return pszBuffer;
}

HyToken::Type HyToken::ValueToType( const char* pszTypeString )
{
	const std::map< std::string, int >::const_iterator it = StringIntTypes().find( pszTypeString );
	if ( it != StringIntTypes().end() )
	{
		const Type eType = static_cast< Type >( it->second );
		return eType;
	}

	if ( _IsNumber( pszTypeString ) )
		return typeNum;

	return typeWord;
}

HyToken::Type HyToken::StringToType( const HyChar* str )
{
	const std::map< HyString, HyInt32 >::const_iterator iter = StringIntTypes().find( str );
	if ( iter != StringIntTypes().end() )
	{
		const Type type = static_cast< Type >( iter->second );
		return type;
	}

	if ( !strcmp( str, "number" ) )
		return typeNum;

	return typeWord;
}

const HyChar* HyToken::GetTypeString( Type type )
{
	const std::map< HyInt32, HyString >::const_iterator iter = IntStringTypes().find( type );
	if ( iter != IntStringTypes().end() )
	{
		const HyString& strType = iter->second;
		return strType.c_str();
	}

	switch ( type )
	{
		case 0:                         return "\n";
		case typeWord:                  return "word";
		case typeNum:                   return "number";
		case typeString:                return "word";
		default:
			return NULL;
	}
}
