#include "PCH.h"
#include "HyGrammarParser.h"
#include "HyFileReader.h"
#include "HyFileWriter.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	생성자
///
/// @param	name	이름
////////////////////////////////////////////////////////////////////////////////////////////////////
HyGrammarParser::HyGrammarParser( const HyString& name )
:
m_debugging ( false ),
m_name   ( name )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	문법을 돌면서 콜백 함수를 호출한다.
///
/// @param	parserData	파서 데이터
/// @param	param		파라미터
///
/// @return	작업을 성공적으로 수행했으면 true, 그렇지 않으면 false를 반환한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
static HyBool _RunCallBack( ParserData& parserData, HyVoid* param )
{
	for ( const HyExecutiveAction& executiveAction : parserData.m_executiveActions )
		if ( executiveAction.GetGrammar().GetCallBackFunction() )
			executiveAction.GetGrammar().GetCallBackFunction()( executiveAction.GetTokens(), param );

	return true;
}

bool HyGrammarParser::Parse(const std::string& strFileString, ParserData& interpretData, void* const pParam, char* const InterpretingResult)
{
	m_iCountOfLines = 1;
	m_strCompileString.clear();

	int iTick = GetTickCount();
	_LexicalAnalysis(strFileString, interpretData);
	LOG(HyLog::Info, "LexicalAnalysis() %d msec", GetTickCount() - iTick);
	if(interpretData.m_tokens.empty())
		return true;

	iTick = GetTickCount();
	//bool bResult = SyntaxAnalysis(interpretData);
	bool bResult = _SyntaxAnalysis_LL(interpretData);
	LOG(HyLog::Info, "SyntaxAnalysis() %d msec", GetTickCount() - iTick);

	if(bResult == false)
		LOG(HyLog::Error, "bResult == false");

	if(InterpretingResult)
		strcpy(InterpretingResult, m_strCompileString.c_str());

	if(bResult)
	{
		iTick = GetTickCount();
		bResult &= _RunCallBack( interpretData, pParam );
		LOG(HyLog::Info, "_RunCallBack() %d msec", GetTickCount() - iTick);

		if(bResult == false)
			LOG(HyLog::Error, "CodeGeneration() failed");
	}

	return bResult;
}


bool HyGrammarParser::ParseFile(const std::string& strFileName, ParserData& interpretData, void* const pParam, char* const pszInterpretingResult)
{
	LOG(HyLog::Info, "strFileName(%s)", strFileName.c_str());

	if(pszInterpretingResult != NULL)
		*pszInterpretingResult = 0;

	std::string strSource;
	if(_ReadSourceFile(strFileName, strSource) == false)
	{
		LOG(HyLog::Error, "ReadSourceFile() failed. strFileName(%s)", strFileName.c_str());
		return false;
	}

	return Parse(strSource.c_str(), interpretData, pParam, pszInterpretingResult);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	입력 문법을 추가한다.
///
/// @param	source		원본
/// @param	destination	목적
/// @param	callback	콜백 함수
///
/// @return	반환 값 없음
////////////////////////////////////////////////////////////////////////////////////////////////////
static HyVoid _Parse( const char* p, BOOL bInclude, std::list<std::string>& list, std::string& string )
{
	if(!bInclude && *p == '(' && p[1] != ' ' && p[1] != 0)
	{
		while(*p++ != ')');
		if(*p == ' ')
			++p;
	}

	for(; *p; ++p)
	{
		if(p[0] == '(' && p[1] != ' ' && p[1] != 0)
		{
			std::string str = string;
			_Parse(p + 1, true, list, string);
			list.push_back(str);
			_Parse(p, false, list, list.back());
			break;
		}
		else if(p[0] == ')' && p[-1] != ' ')
			;
		else
			string += *p;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	입력 문법을 추가한다.
///
/// @param	source		원본
/// @param	target		목적
/// @param	callback	콜백 함수
///
/// @return	반환 값 없음
////////////////////////////////////////////////////////////////////////////////////////////////////
void HyGrammarParser::AddInputGrammar( const HyChar* source, const HyChar* target, GENERATION_CALLBACK callback )
{
	HyChar srcText[ 256 ];
	strcpy( srcText, source );
	source = strtok( srcText, " " );
	HY_ENSURE( source, return );

	std::list< HyString > rightStrings;
	rightStrings.push_back( "" );
	_Parse( target, true, rightStrings, rightStrings.back() );

	HySymbol* srcSymbol = _AddSymbol( source, HySymbol::typeNonterminal );
	if ( !srcSymbol )
	{
		srcSymbol = _GetSymbol( source );
		if ( !srcSymbol )
			LOG( HyLog::Fatal, "pHeadSymbol == NULL. Head(%s)", source );

		srcSymbol->SetType( HySymbol::typeNonterminal );
	}

	for ( HyString& eachRightString : rightStrings )
	{
		HyBool exist = false;
		for ( const HyGrammar& grammar : m_inputGrammars )
		{
			if ( &grammar.GetLeftSymbol() == srcSymbol && grammar.GetRightString() == eachRightString )
			{
				LOG( HyLog::Error, "invalid grammar. Head(%s), To(%s)", source, eachRightString.c_str() );
				exist = true;
				break;
			}
		}

		if ( exist )
			continue;

		m_inputGrammars.push_back( HyGrammar( *srcSymbol ) );

		HyGrammar& grammar = m_inputGrammars.back();
		grammar.SetId( m_inputGrammars.size() - 1 );

		m_inputGrammarVector.push_back( &grammar );

		if ( eachRightString.length() > 0 && eachRightString[ 0 ] == ' ' )
			eachRightString.erase( 0, 1 );

		if ( eachRightString.length() > 0 && *eachRightString.rbegin() == ' ' )
			eachRightString.erase( eachRightString.length() - 1 );

		grammar.m_rightString = eachRightString;
		HyChar buf[256];
		strcpy_s( buf, sizeof( buf ) - 1, eachRightString.c_str());
		HyChar* symbolName = strtok( buf, " " );
		do
		{
			HySymbol* symbol = _AddSymbol( symbolName, HySymbol::typeTerminal );
			if ( !symbol )
				symbol = _GetSymbol( symbolName );

			if ( !symbol )
				LOG( HyLog::Fatal, "pSymbol == NULL. pszSymbolName(%s)", symbolName );

			grammar.m_rightSymbols.push_back( symbol );

		} while( symbolName = strtok( NULL, " " ) );
		grammar.m_endPosition = grammar.m_rightSymbols.size();

		grammar.SetCallBackFunction( callback );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	초기화한다.
///
/// @param	flag	플래그
///
/// @return	반환 값 없음
////////////////////////////////////////////////////////////////////////////////////////////////////
HyVoid HyGrammarParser::Init( HyUInt32 flag )
{
	HyToken::Init();

	HyUInt32 startTime = GetTickCount();

	_InitializeSymbols();

	startTime = GetTickCount();
	OnInit();

	for ( auto& pair : m_symbols )
	{
		HySymbol& symbol = pair.second;

		if ( symbol.GetType() == HySymbol::typeNonterminal )
			continue;

		if (
			symbol.GetName() == "word" ||
			symbol.GetName() == "number" ||
			symbol.GetName() == "string" ||
			symbol.GetName() == "[e]" ||
			symbol.GetName() == "$" )
			continue;

		HyToken::AddTokenType( symbol.GetName().c_str( ));
	}

	if ( !(flag & (HyUInt32)( InitFlag::UseDataFile ) ) )
	{
		while ( _InitializeFirsts() ) {}

		while ( true )
		{
			HyInt32 count = _InitializeFollows();
			if ( count == 0 )
				break;

			LOG( HyLog::Info, "Count(%d)", count );
		}

		_MakeLLTable();
	}
	else
	{
		startTime = GetTickCount();
		_ReadDataFile();
	}

	HyUInt32 endTime = GetTickCount();
	LOG( HyLog::Info, "Init() %d msec", endTime - startTime );

	if ( flag & (HyUInt32)( InitFlag::GenerateDataFile ) )
	{
		_GenerateDataFile();
// 		printf( "데이터 파일이 생성 되었습니다. 데이터 모드 사용 모드로 다시 실행해 주세요.\n" );
// 		exit( 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	심볼을 초기화한다.
///
/// @return	반환 값 없음
////////////////////////////////////////////////////////////////////////////////////////////////////
HyVoid HyGrammarParser::_InitializeSymbols()
{
	m_symbols.clear();
	m_symbolsByName.clear();

	_AddSymbol( "$", HySymbol::typeTerminal );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	퍼스트를 초기화한다.
///
/// @return	추가된 퍼스트 개수
////////////////////////////////////////////////////////////////////////////////////////////////////
HyUInt32 HyGrammarParser::_InitializeFirsts()
{
	HyUInt32 firstCount = 0;

	for ( auto& pair : m_symbols )
	{
		HySymbol& symbol = pair.second;

		if ( symbol.GetType() == HySymbol::typeNonterminal )
			continue;

		if ( symbol.AddFirst( m_symbolsByName, symbol.GetName() ) )
			++firstCount;
	}

	for ( HyGrammar& grammar : m_inputGrammars )
	{
		HySymbol& symbol = grammar.GetLeftSymbol();

		const std::vector< const HySymbol* >& rightSymbols = grammar.GetRightSymbols();
		HyUInt32 rightSymbolCount = rightSymbols.size();

		HyUInt32 i;
		for ( i = 0; i < rightSymbolCount; ++i )
		{
			const HySymbol* eachTargetSymbol = rightSymbols[ i ];
			HY_ENSURE( eachTargetSymbol, continue );

			for ( const HyString& eachFirst : eachTargetSymbol->GetFirsts() )
			{
				if ( eachFirst == "[e]" )
					continue;

				if ( symbol.AddFirst( m_symbolsByName, eachFirst ) )
					++firstCount;
			}

			if ( !eachTargetSymbol->HasFirst( "[e]" ) )
				break;
		}

		if( i >= rightSymbolCount )
		{
			if ( symbol.AddFirst( m_symbolsByName, "[e]" ) )
				++firstCount;
		}
	}

	return firstCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	팔로우를 초기화한다.
///
/// @return	추가된 팔로우 개수
////////////////////////////////////////////////////////////////////////////////////////////////////
HyUInt32 HyGrammarParser::_InitializeFollows()
{
	HyUInt32 followCount = 0;

	// 규칙1
	if ( m_inputGrammars.front().GetLeftSymbol().AddFollow( m_symbolsByName, "$" ) )
		++followCount;

	for ( const HyGrammar& grammar : m_inputGrammars )
	{
		const std::vector< const HySymbol* >& tos = grammar.GetRightSymbols();
		const HyUInt32 toSize = tos.size();

		HyUInt32 index = 0;
		while ( index < toSize )
		{
			HySymbol* const symbol = const_cast< HySymbol* >( tos[ index ] );
			++index;

			// 규칙2
			HyUInt32 index2;
			for ( index2 = index; index2 < toSize; ++index2 )
			{
				const HySymbol* const symbol2 = tos[ index2 ];

				HyBool hasEpsilon = false;
				for ( const HyString& first : symbol2->GetFirsts() )
				{
					if ( first == "[e]" )
					{
						hasEpsilon = true;
						continue;
					}

					if ( symbol->AddFollow( m_symbolsByName, first ) )
						++followCount;
				}

				if ( !hasEpsilon )
					break;
			}

			// 규칙 3
			if ( index2 >= toSize )
			{
				for ( const HyString& follow : grammar.GetLeftSymbol().GetFollows() )
					if ( symbol->AddFollow( m_symbolsByName, follow ) )
						++followCount;
			}
		}
	}

	return followCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	심볼을 추가한다.
///
/// @param	name	이름
/// @param	type	종류
///
/// @return	추가된 심볼
////////////////////////////////////////////////////////////////////////////////////////////////////
HySymbol* HyGrammarParser::_AddSymbol( const HyString& name, HySymbol::Type type )
{
	if ( name.empty() )
		return NULL;

	if ( _GetSymbol( name ) != NULL )
		return NULL;

	HySymbol symbol;

	static HyUInt32 symbolId = 0;

	symbol.SetId   ( ++symbolId );
	symbol.SetName ( name );
	symbol.SetType ( type );

	const std::pair< std::map< HyUInt32, HySymbol >::iterator, HyBool > result = m_symbols.insert( std::make_pair( symbol.GetId(), symbol ) );
	if ( !result.second )
		return NULL;

	HySymbol& insertedSymbol = result.first->second;

	m_symbolsByName[ symbol.GetName() ] = &insertedSymbol;

	return &insertedSymbol;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	토큰을 생성한다.
///
/// @param	pszString	문자열
/// @param	tokenPos	토큰 위치
/// @param	parserData	파서 데이터
///
/// @return	반환 값 없음
////////////////////////////////////////////////////////////////////////////////////////////////////
HyVoid HyGrammarParser::_CreateToken(
	const HyChar*     pszString,
	const TokenPos&   tokenPos,
	      ParserData& parserData )
{
	parserData.m_tokens.push_back( HyToken() );
	HyToken& token = parserData.m_tokens.back();

	token.m_value = pszString;
	token.m_pos   = tokenPos;
	token.m_line = m_iCountOfLines;
	token.m_type  = HyToken::ValueToType( pszString );
}

bool HyGrammarParser::_LastTokenIsString(ParserData& interpretData) const
{
	if(interpretData.m_tokens.empty())
		return false;

	if(interpretData.m_tokens.back().m_type != HyToken::typeString)
		return false;

	return true;
}

void HyGrammarParser::_LexicalAnalysis(const std::string& strString, ParserData& parserData)
{
	char			szBuf[ 1024 ];
	char			szBuf2[ 1024 ];
	std::string		str;
	int				iState = 0;
	const char*		pszStr = strString.c_str();
	TokenPos		tokenPos;

	*szBuf = 0;
	//Debug
	while ( *pszStr )
	{
		switch ( iState )
		{
			case 1:
				if ( *pszStr == '\"' )
				{
					char Buf2[ 1024 * 10 ];
					char* p2 = Buf2;
					for(const char* p = szBuf; *p; ++p, ++p2)
						if(p[0] == '\\' && p[1] == 'n')
						{
							++p;
							*p2 = '\n';
						}
						else
							*p2 = *p;
					*p2 = 0;

					if(_LastTokenIsString(parserData))
						parserData.m_tokens.back().m_value += Buf2;
					else
					{
						_CreateToken( Buf2, tokenPos, parserData );
						parserData.m_tokens.back().m_type = HyToken::typeString;
					}
					iState = 0;
					*szBuf = 0;
				}
				else
				{
					strncat( szBuf, pszStr, 1 );
					if ( *pszStr == '\\' )
					{
						strncat( szBuf, ++pszStr, 1 );
					}
				}
				break;
			case 0:
				while(*pszStr == ' ' || *pszStr == '\t' || *pszStr == '\r' || *pszStr == '\n')
				{
					if(*pszStr == '\n')
						++m_iCountOfLines;

					if(*szBuf)
					{
						_CreateToken( szBuf, tokenPos, parserData );
						*szBuf = 0;
					}
					++pszStr;
				}

				if(!*pszStr)
				{
					--pszStr;
					break;
				}
				if(*pszStr == '\"')
				{
					if(*szBuf)
						_CreateToken( szBuf, tokenPos, parserData );
					iState = 1;
					*szBuf = 0;
				}
				else if(!strncmp(pszStr, "//", 2))
				{
					if(strncmp(pszStr, "//##//", 6) == 0)
					{
						const char* p = pszStr;
						while(*p != '\n')
							++p;

						const int iLineLength = p - (pszStr + 7);

						pszStr += 7;
						const char* const pszFileName = strtok((char*)pszStr, "#\r\n");
						const int iLen = strlen(pszFileName);
						pszStr += strlen(pszFileName) + 1;
						tokenPos.m_fileName	= pszFileName;
						tokenPos.m_line		= atoi(pszStr);

						pszStr = p;
					}
					else
					{
						while(*pszStr && *pszStr != '\n')
						{
							++pszStr;
						}
					}

					++m_iCountOfLines;
				}
				else if(!strncmp(pszStr, "/*", 2))
				{
					while(strncmp(pszStr++, "*/", 2))
					{
						if(*pszStr == '\n')
							++m_iCountOfLines;
					}
				}
				else if(*pszStr == '{' || *pszStr == '}' || *pszStr == '(' || *pszStr == ')' || *pszStr == '[' || *pszStr == ']' || *pszStr == ';' || *pszStr == ',' || *pszStr == '<' || *pszStr == '>' ||
						*pszStr == '=' || *pszStr == '+' || *pszStr == '*' || *pszStr == '^' || *pszStr == '%' || *pszStr == '&' || *pszStr == '/' || *pszStr == '|' || *pszStr == '#' || *pszStr == ':' ||
						*pszStr == '-' || *pszStr == '.' || *pszStr == '~' || *pszStr == '!' || *pszStr == '\'')
				{
					if(*szBuf)
					{
						_CreateToken( szBuf, tokenPos, parserData );
						*szBuf = 0;
					}
					*szBuf2 = *pszStr;
					*(szBuf2 + 1) = 0;

					switch(*pszStr)
					{
						case '=':
							if(*(pszStr + 1) == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							break;
						case '+':
							if(*(pszStr + 1) == '+')
							{
								strcat(szBuf2, "+");
								++pszStr;
							}
							else if(*(pszStr + 1) == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							break;
						case '-':
							if(*(pszStr + 1) == '-')
							{
								strcat(szBuf2, "-");
								++pszStr;
							}
							else if(*(pszStr + 1) == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							else if(*(pszStr + 1) == '>')
							{
								strcat(szBuf2, ">");
								++pszStr;
							}
							break;
						case '*':
							if(*(pszStr + 1) == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							break;
						case '/':
							if(*(pszStr + 1) == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							break;
						case '<':
							if(*(pszStr + 1) == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							else if(*(pszStr + 1) == '<')
							{
								strcat(szBuf2, "<");
								++pszStr;
							}
							break;
						case '>':
							if(*(pszStr + 1) == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							else if(*(pszStr + 1) == '>')
							{
								strcat(szBuf2, ">");
								++pszStr;
							}
							break;
						case '|':
							if(*(pszStr + 1) == '|')
							{
								strcat(szBuf2, "|");
								++pszStr;
							}
							break;
						case '&':
							if(*(pszStr + 1) == '&')
							{
								strcat(szBuf2, "&");
								++pszStr;
							}
							else if(*(pszStr + 1) == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							break;
						case ':':
							if(*(pszStr + 1) == ':')
							{
								strcat(szBuf2, ":");
								++pszStr;
							}
							break;
						case '.':
							if(pszStr[1] == '.' && pszStr[2] == '.')
							{
								strcat(szBuf2, "..");
								pszStr += 2;
							}
							break;
						case '!':
							if(pszStr[1] == '=')
							{
								strcat(szBuf2, "=");
								++pszStr;
							}
							break;
						case '\'':
							if ( pszStr[ 1 ] == '\\' && pszStr[ 2 ] == '0' )
							{
								sprintf( szBuf2, "%d", 0 );
								pszStr += 3;
							}
							else if ( pszStr[ 1 ] == '\\' && pszStr[ 2 ] == '\'' )
							{
								sprintf( szBuf2, "%d", '\'' );
								pszStr += 3;
							}
							else if ( pszStr[ 1 ] == '\\' && pszStr[ 2 ] == '\"' )
							{
								sprintf( szBuf2, "%d", '\"' );
								pszStr += 3;
							}
							else if ( pszStr[ 1 ] == '\\' && pszStr[ 2 ] == '\\' )
							{
								sprintf( szBuf2, "%d", '\\' );
								pszStr += 3;
							}
							else if ( pszStr[ 1 ] == '\\' && pszStr[ 2 ] == 't' )
							{
								sprintf( szBuf2, "%d", '\t' );
								pszStr += 3;
							}
							else if ( pszStr[ 1 ] == '\\' && pszStr[ 2 ] == 'r' )
							{
								sprintf( szBuf2, "%d", '\r' );
								pszStr += 3;
							}
							else if ( pszStr[ 1 ] == '\\' && pszStr[ 2 ] == 'n' )
							{
								sprintf( szBuf2, "%d", '\n' );
								pszStr += 3;
							}
							else
							{
								sprintf( szBuf2, "%d", pszStr[ 1 ] );
								pszStr += 2;
							}

							HY_ENSURE( *pszStr == '\'', return );
							break;
					}
					_CreateToken( szBuf2, tokenPos, parserData );
					*szBuf = 0;
				}
				else
				{
					if ( !strncmp( pszStr, "0x", 2 ) )
					{
						if ( *szBuf )
							_CreateToken( szBuf, tokenPos, parserData );

						pszStr += 2;
						HyUInt32 number = 0;

						while ( true )
						{
							if ( *pszStr >= 'a' && *pszStr <= 'z' )
								number = number * 0x10 + *pszStr - 'a' + 10;
							else if ( *pszStr >= 'A' && *pszStr <= 'Z' )
								number = number * 0x10 + *pszStr - 'A' + 10;
							else if ( *pszStr >= '0' && *pszStr <= '9' )
								number = number * 0x10 + *pszStr - '0';
							else
								break;

							++pszStr;
						}

						sprintf( szBuf, "%d", number );
						_CreateToken( szBuf, tokenPos, parserData );
						*szBuf = 0;
						continue;
					}
					else if ( !(*szBuf == '0' && ((szBuf[1] == 0 && *pszStr == 'x') || szBuf[ 1 ] == 'x')) &&
						*szBuf >= '0' && *szBuf <= '9' && (*pszStr < '0' || *pszStr > '9'))
					{
						_CreateToken( szBuf, tokenPos, parserData );
						*szBuf = 0;
					}
					strncat(szBuf, pszStr, 1);
				}
				break;
		}
		++pszStr;
	}

	if ( (pszStr = strtok( szBuf, " \t\r\n" )) )
		_CreateToken( pszStr, tokenPos, parserData );

	_CreateToken( "$", tokenPos, parserData );
}


void HyGrammarParser::_LogError(const HyToken& Tok, int Type, const HyToken* PrevToken)
{
	char  Buf[256];

	++m_iCountOfErrors;
	if(PrevToken)
	{
		sprintf(Buf, "%d번째 줄의 '%s' 뒤에 있는 '%s'에서 문법 에러\r\n"
				   , Tok.m_line
				   , PrevToken->m_value.c_str()
				   , Tok.m_value.c_str());
		m_strCompileString += Buf;
	}
	else
	{
		sprintf(Buf, "%d번째 줄의 맨 앞의 '%s'에서 문법 에러\r\n"
				   , Tok.m_line
				   , Tok.m_value.c_str());
		m_strCompileString += Buf;
	}
}


void HyGrammarParser::_LogWarning(const HyToken& Tok, int Type)
{
  ++m_CountOfWarnings;
  return;
/*
  printf("%s.txt(%d) : warning N%d: "
		 , SourceFileName
		 , Tok->Line
		 , Type);

  switch(Type)
  {
	case TYPEOF_WARNING_GLOBALVARIABLE_UNREFERENCING:
	  printf("'%s' : unreferenced global variable.\n"
			 , Tok->String);
	  break;
	case TYPEOF_WARNING_LOCALVARIABLE_UNREFERENCING:
	  printf("'%s' : unreferenced local variable.\n"
			 , Tok->String);
	  break;
  }
*/
}


void HyGrammarParser::_MakeAction()
{
	for ( auto& pair : m_mapState )
	{
		HyState& eachState = pair.second;

		for ( auto& pair2 : m_symbols )
		{
			const HySymbol& eachSymbol = pair2.second;

			HyInt32 iToNumber = eachState.GetGoto( eachSymbol.GetName() );
			if ( eachSymbol.GetType() == HySymbol::typeNonterminal )
			{
				if ( iToNumber == -1 )
					continue;

				eachState.AddAction( eachSymbol, HyAction::Type::Goto, iToNumber );
				continue;
			}

			if ( iToNumber != -1 )
			{
				eachState.AddAction( eachSymbol, HyAction::Type::Shift, iToNumber );
				continue;
			}

			for ( const HyGrammarPosition& eachGrammar : eachState.GetGrammars() )
			{
				if ( eachGrammar.GetCurSymbol() )
					continue;

				if ( !eachGrammar.m_grammar.GetLeftSymbol().HasFollow( eachSymbol.GetName() ) )
					continue;

				HyGrammar& grammar = m_inputGrammars.front();
				if ( eachGrammar.m_grammar == grammar )
					eachState.AddAction( eachSymbol, HyAction::Type::Accept, 0 );
				else
					eachState.AddAction( eachSymbol, HyAction::Type::Reduce, eachGrammar.m_grammar.GetNumber( m_inputGrammars ), &eachGrammar.m_grammar );
			}
		}
	}
}


void HyGrammarParser::_MakeLLTable()
{
//	m_mapLLTable.assign( m_symbols.size() + 1, std::map< HyInt32, std::map< HyInt32, std::set< const HyGrammar* > > >() );

	for ( const HyGrammar& eachGrammar : m_inputGrammars )
	{
		HyUInt32 rightSymbolCount = eachGrammar.m_rightSymbols.size();

		std::set< HyString > firsts;
		HyUInt32 i;
		for ( i = 0; i < rightSymbolCount; ++i )
		{
			const HySymbol* const targetSymbol = eachGrammar.m_rightSymbols[ i ];
			HY_ENSURE( targetSymbol, continue );

			HyBool hasEpsilon = false;
			for ( const HyString& first : targetSymbol->GetFirsts() )
			{
				if ( first == "[e]" )
				{
					hasEpsilon = true;
					continue;
				}

				firsts.insert( first );
			}

			if ( !hasEpsilon )
				break;
		}

		if ( i >= rightSymbolCount )
			firsts.insert("[e]");

		std::set< const HyGrammar* > subGrammars;
		eachGrammar.GetSubGrammars( m_inputGrammars, subGrammars );

// 		HyFileWriter file( "SubGrammar.txt", "w" );
// 		FOR_EACH( const HyGrammar* eachGrammar, subGrammars )
// 			file.WriteFormat( "%s -> %s\n", eachGrammar->m_leftSymbol.GetName().c_str(), eachGrammar->m_rightString.c_str() );
// 		file.Close();

		std::map< HyString, std::set< HyString > > followsByName;
		_CalFollowMap( subGrammars, followsByName );

		std::map< HyInt32, std::map< HyInt32, std::set< const HyGrammar* > > >& grammarsByTwoTokenTypes =
			m_mapLLTable[ eachGrammar.GetLeftSymbol().GetId() ];

		for ( const HyString& eachFirst : firsts )
		{
			if ( eachFirst == "[e]" )
				continue;

			HyToken::Type firstType = HyToken::StringToType( eachFirst.c_str() );

			std::map< HyInt32, std::set< const HyGrammar* > >& grammarsBySecondTokenType =
				grammarsByTwoTokenTypes[ firstType ];

			if ( eachFirst == "$")
			{
				grammarsBySecondTokenType[ firstType ].insert( &eachGrammar );
				continue;
			}

			const std::set< HyString >& follows = followsByName[ eachFirst ];

			for ( const HyString& eachFollow : follows )
			{
				HyToken::Type followType = HyToken::StringToType( eachFollow.c_str() );

				grammarsBySecondTokenType[ followType ].insert( &eachGrammar );
			}
		}

		// 규칙 3
		if ( firsts.find( "[e]" ) == firsts.end() )
			continue;

		for ( const HyString& eachFollow : eachGrammar.GetLeftSymbol().GetFollows() )
		{
			HyToken::Type followType = HyToken::StringToType( eachFollow.c_str() );

			std::map< HyInt32, std::set< const HyGrammar* > >& grammarsBySecondTokenType =
				grammarsByTwoTokenTypes[ followType ];

			if ( eachFollow == "$" )
			{
				grammarsBySecondTokenType[ followType ].insert( &eachGrammar );
				continue;
			}

			const HySymbol* const eachFollowSymbol = _GetSymbol( eachFollow );
			HY_ENSURE( eachFollowSymbol, continue );

			for ( const HyString& eachNextFollow : eachFollowSymbol->GetFollows() )
			{
				HyToken::Type nextFollowType = HyToken::StringToType( eachNextFollow.c_str() );

				grammarsBySecondTokenType[ nextFollowType ].insert( &eachGrammar );
			}
		}
	}
}


bool HyGrammarParser::_ReadSourceFile(const std::string& strFileName, std::string& Source)
{
	FILE* const pFile = fopen(strFileName.c_str(), "r");
	if(pFile == NULL)
	{
		LOG(HyLog::Error, "pFile == NULL. strFileName(%s)", strFileName.c_str());
		return false;
	}

	char szBuf[1024];
	while(fgets(szBuf, sizeof(szBuf), pFile))
		Source += szBuf;
	fclose(pFile);
	return true;
}


void HyGrammarParser::_CalFollowMap(const std::set<const HyGrammar*>& setGrammar, std::map<std::string, std::set<std::string> >& mapFollow) const
{
	int FollowCount;

	//DeleteFile("FollowMap.txt");

	std::map<std::string, std::map<std::string, int> > mapFollowCount;

	do
	{
		FollowCount = 0;
		for ( const HyGrammar* const pGrammar : setGrammar )
		{
			const std::vector<const HySymbol*>& vecTo = pGrammar->GetRightSymbols();
			const int iToSize = static_cast<int>(vecTo.size());

			int iCurIdx = 0;
			while(iCurIdx < iToSize)
			{
				HySymbol* const pSymbol = const_cast<HySymbol*>(vecTo[iCurIdx]);
				++iCurIdx;

				// 규칙2
				int iIdx;
				for(iIdx = iCurIdx; iIdx < iToSize; ++iIdx)
				{
					const HySymbol* const Symbol2 = vecTo[iIdx];
					bool bHaveEpsilon = false;
					for ( const HyString& first : Symbol2->GetFirsts() )
					{
						if(first == "[e]")
						{
							bHaveEpsilon = true;
							continue;
						}

						++mapFollowCount[ pSymbol->GetName() ][ first ];

						if ( mapFollow[ pSymbol->GetName() ].insert( first ).second )
							++FollowCount;
					}

					if(bHaveEpsilon == false)
						break;
				}

				 // 규칙 3
				if(iIdx >= iToSize)
				{
					for ( const HyString& follow : pGrammar->GetLeftSymbol().GetFollows() )
					{
						++mapFollowCount[ pSymbol->GetName() ][ follow ];
						if ( mapFollow[ pSymbol->GetName() ].insert( follow ).second )
							++FollowCount;
					}
				}
			}
		}
	} while(FollowCount > 0);
/*
	FILE* const pFile = fopen("FollowMap.txt", "a");
	for(std::map<std::string, std::map<std::string, int> >::const_iterator itr = mapFollowCount.begin(); itr != mapFollowCount.end(); ++itr)
	{
		const std::string& strSymbolName = itr->first;
		fprintf(pFile, "Symbol(%s)\n", strSymbolName.c_str());

		const std::map<std::string, int>& mapCount = itr->second;
		for(std::map<std::string, int>::const_iterator itrCount = mapCount.begin(); itrCount != mapCount.end(); ++itrCount)
		{
			const std::string& strFollow = itrCount->first;
			const int iCount = itrCount->second;

			fprintf(pFile, "- %s (%d)\n", strFollow.c_str(), iCount);
		}
	}
	fclose(pFile);
	*/
}


bool HyGrammarParser::_SyntaxAnalysis(ParserData& interpretData)
{
	static char		Buf[SIZEOF_BUFFER];
	int				Num2;
	int				Num3;
	HyStack			stack;
	static char		StackBuffer[SIZEOF_BUFFER];

	if(interpretData.m_tokens.empty())
		return true;

	std::list<HyToken>::iterator TokenIterator = interpretData.m_tokens.begin();
	const HyToken* NowToken = &(*TokenIterator);
	const HyToken* prevToken = NULL;

	stack.Push(0, NULL);

	std::list<std::list<const HyAction*> > listActions;
	std::list<HyStack> listStack;
	std::list<std::list<HyToken>::iterator> listTokenIterator;
	bool bRetry = false;

	while(1)
	{
		const HyState* const pState = HyState::GetState(m_mapState, stack.back().m_Number);
		if ( pState == NULL )
			LOG( HyLog::Fatal, "pState == NULL" );

		const HyAction* pAction;

		if(bRetry == false)
		{
			const std::list<HyAction>* const pListAction = pState->GetAction(NowToken->GetTypeString());
			if(pListAction == NULL)
				pAction = NULL;
			else
			{
				if(pListAction->empty())
					LOG(HyLog::Fatal, "pListAction->empty()");

				pAction = &pListAction->front();

				if(pListAction->size() > 1)
				{
					listStack.push_back(stack);
					std::list<const HyAction*> listTempAction;
					std::list<HyAction>::const_iterator itrAction = pListAction->begin();
					++itrAction;
					for(; itrAction != pListAction->end(); ++itrAction)
					{
						const HyAction& action = *itrAction;
						listTempAction.push_back(&action);
					}

					listActions.push_back(listTempAction);
					listTokenIterator.push_back(TokenIterator);
				}
			}
		}
		else
		{
			bRetry = false;

			if(listActions.empty())
				LOG(HyLog::Fatal, "listAction.empty()");

			std::list<const HyAction*>& listAction = listActions.back();
			pAction = listAction.front();
			listAction.pop_front();
			if(listAction.empty())
			{
				listStack.pop_back();
				listActions.pop_back();
				listTokenIterator.pop_back();
			}
		}

		if ( !pAction )
		{
			if ( !listStack.empty() )
			{
				HyFileWriter file( "SyntaxAnalysis.txt", "a" );
				if ( file.IsOpened() )
				{
					file.WriteFormat( "Error Retry\n" );
					file.Close();
				}

				stack = listStack.back();
				TokenIterator = listTokenIterator.back();
				NowToken = &*TokenIterator;
				bRetry = true;
				continue;
			}

			m_strCompileString.clear();
			wsprintf(Buf, "%d■%s■",
						stack.back().m_Number,
						stack.GetString(StackBuffer));
			m_strCompileString += Buf;
			m_strCompileString += NowToken->GetInput( TokenIterator, interpretData.m_tokens.end(), Buf, sizeof( Buf ) );
			m_strCompileString += "■";
			if ( !strcmp( NowToken->GetTypeString(), "$" ) )
				_LogError( *NowToken, TYPEOF_ERROR_UNEXPECTED_ENDOF_FILE, prevToken );
			else
				_LogError( *NowToken, TYPEOF_ERROR_NORMAL, prevToken );
			HyFileWriter file( "SyntaxAnalysis.txt", "a" );
			if ( file.IsOpened() )
			{
				file.WriteFormat(
					"%s\n----------------------------------------------------------------------------\n",
					m_strCompileString );
				file.Close();
			}

			return false;
		}
/*
		FILE* const pFile = fopen("SyntaxAnalysis.txt", "a");
		if(pFile == NULL)
			LOG(SLog::Fatal, "pFile == NULL");
		fprintf(pFile, "%d■%s■%s\n",
						stack.back().m_Number,
						stack.GetString(StackBuffer),
						NowToken->GetInput(TokenIterator,
						interpretData.m_listToken.end(), Buf));
		fclose(pFile);
*/
		switch(pAction->m_type)
		{
			case HyAction::Type::Accept:
				return true;
			case HyAction::Type::Shift:
				stack.Push(pAction->m_symbol);
				stack.Push(pAction->m_iNumber, NowToken);
				prevToken = NowToken;
				NowToken = &(*(++TokenIterator));
				break;
			case HyAction::Type::Reduce:
			{
				const int Count = pAction->m_grammar->GetRightSymbolCount();
				const int Count2 = pAction->m_grammar->GetRightTerminalSymbolCount();
				interpretData.m_executiveActions.push_back(HyExecutiveAction(*pAction->m_grammar));
				HyExecutiveAction& HyExecutiveAction = interpretData.m_executiveActions.back();

				if ( HyExecutiveAction.GetGrammar().HasTerminalDerivation() )
				{
					std::list<HyStackContent>::reverse_iterator it = stack.rbegin();
					for(Num2 = 1, Num3 = 1; Num2 <= 2 * Count; ++Num2, ++it)
					{
						const HyStackContent& Con = *it;
						if(Con.m_Token == NULL)
							continue;

						if(
							!strcmp(Con.m_Token->GetTypeString(), HyToken::GetTypeString(HyToken::typeWord)) ||
							!strcmp(Con.m_Token->GetTypeString(), HyToken::GetTypeString(HyToken::typeNum)) ||
							Con.m_Token->m_type > HyToken::typeFinal)
						{
							HyExecutiveAction.GetTokens().insert( HyExecutiveAction.GetTokens().begin(), Con.m_Token );
							if(++Num3 > Count2)
								break;
						}
					}
				}
				for(Num2 = 1; Num2 <= Count * 2; ++Num2)
					stack.Pop();

				const HyState* const pState = HyState::GetState( m_mapState, stack.back().m_Number );
				if ( pState == NULL )
					LOG( HyLog::Fatal, "pState == NULL" );

				const std::list< HyAction >* const pListAction2 = pState->GetAction( pAction->m_grammar->GetLeftSymbol().GetName() );
				if(pListAction2 == NULL)
				{
					if(!strcmp(NowToken->GetTypeString(), "$"))
						_LogError(*NowToken, TYPEOF_ERROR_UNEXPECTED_ENDOF_FILE, prevToken);
					else
						_LogError(*NowToken, TYPEOF_ERROR_NORMAL, prevToken);
					return false;
				}

				if(pListAction2->empty())
					LOG(HyLog::Fatal, "pListAction2->empty()");

				const HyAction* const pAction2 = &pListAction2->front();

				stack.Push( pAction->m_grammar->GetLeftSymbol() );
				stack.Push( pAction2->m_iNumber, NULL );
				break;
			}
			case HyAction::Type::Goto:
				HY_ENSURE( false, return false );
				break;
		}
		stack.back().m_Act = pAction;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	컨테이너의 요소들을 이동시킨다.
///
/// @param	sources	원본 목록
/// @param	targets	대상 목록
///
/// @return	반환 값 없음
////////////////////////////////////////////////////////////////////////////////////////////////////
template< typename T >
void _Move( std::list< T >& sources, std::list< T >& targets )
{
	for ( T& element : sources )
		targets.push_back( element );

	sources.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	액션 목록을 실행한다.
///
/// @param	executiveActions	실행 액션 목록
////////////////////////////////////////////////////////////////////////////////////////////////////
HyVoid _PrintActions( const std::list< HyExecutiveAction >& executiveActions )
{
	printf("[Action]\n");
	for ( const HyExecutiveAction& action : executiveActions )
	{
		printf( "%s -> %s : ", action.GetGrammar().GetLeftSymbol().GetName().c_str(), action.GetGrammar().m_rightString.c_str() );
		HyUInt32 index = 0;
		for ( const HyToken* token : action.GetTokens() )
		{
			if ( token == NULL )
				LOG( HyLog::Fatal, "pToken == NULL" );

			if ( index++ > 0 )
				printf( ", " );
			printf( "%s", token->m_value.c_str() );
		}
		printf( "\n" );
	}
}

struct LLStack
{
	std::list< const HySymbol* >         m_symbols;
	std::list< HyToken >::const_iterator m_tokenIter;
	std::list< const HyGrammar* >        m_derivationGrammars;
	std::list< const HyToken* >          m_tokens;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	LL 파싱을 이용해 문법을 분석한다.
///
/// @param	parserData	파서 데이터
///
/// @return	작업을 성공적으로 수행했으면 true, 그렇지 않으면 false를 반환한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
HyBool HyGrammarParser::_SyntaxAnalysis_LL( ParserData& parserData )
{
	if ( m_inputGrammars.empty() )
		return false;

	std::list< const HyGrammar* > parsedGrammars;
	std::list< const HyToken* > listToken;

	std::list< std::set< const HyGrammar* > > listGrammars;

	std::list< LLStack > stacks;
	LLStack stack;

	stack.m_symbols.push_back( &m_inputGrammars.front().GetLeftSymbol() );
	stack.m_tokenIter = parserData.m_tokens.begin();

	const HyUInt32 totalLine = parserData.m_tokens.rbegin()->m_line;

	HyBool backTracking = false;
	HyInt32 nextDebugLine;

	if ( m_debugging )
		nextDebugLine = -1;
	else
		nextDebugLine = -2;

	HyUInt32 loopCount = 0;
	HyUInt32 startTick = GetTickCount();

	while ( true )
	{
		++loopCount;

		const HyToken& token = *stack.m_tokenIter;

		std::list< HyToken >::const_iterator nextTokenIter = stack.m_tokenIter;
		++nextTokenIter;
		if ( nextTokenIter == parserData.m_tokens.end() )
			--nextTokenIter;

		const HyToken& nextToken = *nextTokenIter;

		if ( stack.m_symbols.empty() )
		{
			if ( token.m_type == HyToken::typeFinal )
				break;

			if ( stacks.empty() )
				return false;

			if ( m_debugging )
				printf( "##### BackTracking\n" );

			stack			= stacks.back();
			backTracking	= true;
			continue;
		}

		const HySymbol* symbol = stack.m_symbols.back();
		HY_ENSURE( symbol, return false );

		_ProcessDebugging( nextDebugLine, totalLine, stack.m_tokenIter, parserData.m_tokens.end(), symbol, nextToken );

		if ( symbol->GetType() == HySymbol::typeTerminal )
		{
			if ( m_debugging )
				printf( "\n" );
			if ( symbol->GetName() == token.GetTypeString() )
			{
				if ( symbol->GetName() == ";" )
				{
					_Move( stack.m_derivationGrammars, parsedGrammars );
					_Move( stack.m_tokens, listToken );

					stacks.clear();
				}

				stack.m_symbols.pop_back();
				stack.m_tokens.push_back( &token );
				++stack.m_tokenIter;
				continue;
			}

			if ( stacks.empty() )
				return false;

			if ( m_debugging )
				printf( "##### BackTracking\n" );

			stack			= stacks.back();
			backTracking	= true;
			continue;
		}

		const HyGrammar* grammar;
		if ( !backTracking )
		{
			HyToken::Type tokenType     = token.m_type     == HyToken::typeString ? HyToken::typeWord : token.m_type;
			HyToken::Type nextTokenType = nextToken.m_type == HyToken::typeString ? HyToken::typeWord : nextToken.m_type;

			const std::set< const HyGrammar* >& grammars = m_mapLLTable[ symbol->GetId() ][ tokenType ][ nextTokenType ];
			if ( grammars.size() != 1 )
			{
				if ( grammars.empty() )
				{
					if ( stacks.empty() )
						return false;

					if ( m_debugging )
						printf( "##### BackTracking\n" );

					stack			= stacks.back();
					backTracking	= true;
					continue;
				}

				std::set< const HyGrammar* > tempGrammars = grammars;
				tempGrammars.erase( tempGrammars.begin() );
				if ( m_debugging )
				{
					for ( const HyGrammar* grammar : tempGrammars )
					{
						printf(
							"##### Reserve : %s -> %s\n",
							grammar->GetLeftSymbol().GetName().c_str(),
							grammar->m_rightString.c_str() );
					}
				}

				listGrammars.push_back( tempGrammars );
				stacks.push_back( stack );
			}

			grammar = *grammars.begin();
			if ( !grammar )
				LOG( HyLog::Fatal, "pGrammar == NULL" );
		}
		else
		{
			backTracking = false;
			grammar = *listGrammars.back().begin();
			listGrammars.back().erase( listGrammars.back().begin() );
			if ( listGrammars.back().empty() )
			{
				listGrammars.pop_back();
				stacks.pop_back();
			}
		}

		stack.m_symbols.pop_back();

		stack.m_derivationGrammars.push_back( grammar );

		if ( m_debugging )
			printf( "%s -> %s\n", grammar->GetLeftSymbol().GetName().c_str(), grammar->m_rightString.c_str() );

		for ( auto iter = grammar->m_rightSymbols.rbegin(); iter != grammar->m_rightSymbols.rend(); iter++ )
		{
			const HySymbol* toSymbol = *iter;

			if ( toSymbol->GetName() == "[e]" )
				continue;

			stack.m_symbols.push_back( toSymbol );
		}
	}

	_Move( stack.m_derivationGrammars, parsedGrammars );
	_Move( stack.m_tokens, listToken );

	if ( !m_debugging )
		printf( "\n" );

	//printf( "LoopCount : %d\n", loopCount );
	//printf( "Duration  : %d msec\n", GetTickCount() - startTick );

	std::list< HyExecutiveAction > executiveActions;
	if ( !_MakeExecutiveActions( parserData, parsedGrammars, executiveActions ) )
		return false;

	if ( !_RestructureExecutiveActions( parsedGrammars, executiveActions, parserData) )
		return false;

	if ( m_debugging )
		_PrintActions( parserData.m_executiveActions );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	실행 액션 목록을 생성한다.
///
/// @param	parserData			파서 데이터
/// @param	parsedGrammars		파싱된 문법 목록
/// @param	executiveActions	실행 액션 목록
///
/// @return	작업을 성공적으로 수행했으면 true, 그렇지 않으면 false를 반환한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
HyBool HyGrammarParser::_MakeExecutiveActions(
	ParserData&                     parserData,
	std::list< const HyGrammar* >&  parsedGrammars,
	std::list< HyExecutiveAction >& executiveActions ) 
{
	std::list< std::pair< HyExecutiveAction*, const HySymbol* > > symbols;
	symbols.push_back( std::make_pair( (HyExecutiveAction*)( NULL ), &m_inputGrammars.front().GetLeftSymbol() ) );

	std::list< HyToken >::const_iterator tokenIter = parserData.m_tokens.begin();

	for ( const HyGrammar* grammar : parsedGrammars )
	{
		executiveActions.push_back( HyExecutiveAction( *grammar ) );

		const HySymbol* symbol = symbols.back().second;
		HY_ENSURE( symbol, return false );

		symbols.pop_back();

		for ( auto iter = grammar->m_rightSymbols.rbegin(); iter != grammar->m_rightSymbols.rend(); iter++ )
		{
			const HySymbol* eachSymbol = *iter;

			if ( eachSymbol->GetName() == "[e]" )
				continue;

			symbols.push_back( std::make_pair( &executiveActions.back(), eachSymbol ) );
		}

		if ( symbols.empty() )
			break;

		while ( !symbols.empty() )
		{
			const HySymbol* symbol = symbols.back().second;
			if ( symbol->GetType() == HySymbol::typeNonterminal )
				break;

			const HyToken& token = *tokenIter++;
			if (
				token.m_type != HyToken::typeWord &&
				token.m_type != HyToken::typeNum  &&
				token.m_type != HyToken::typeString )
			{
				HY_ENSURE( symbol->GetName() == token.m_value, return false );
			}

			HyExecutiveAction* executiveAction = symbols.back().first;
			executiveAction->GetTokens().push_back( &token );

			symbols.pop_back();
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	실행 액션 목록을 재구성한다.
///
/// @param	parsedGrammars		파싱된 문법 목록
/// @param	executiveActions	실행 액션 목록
/// @param	parserData			파서 데이터
///
/// @return	작업을 성공적으로 수행했으면 true, 그렇지 않으면 false를 반환한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
HyBool HyGrammarParser::_RestructureExecutiveActions(
	std::list< const HyGrammar* >&  parsedGrammars,
	std::list< HyExecutiveAction >& executiveActions,
	ParserData&                     parserData )
{
	std::list< std::pair< const HyGrammar*, int > > nonTerminalSymbolCountsByGrammar;

	std::list< HyExecutiveAction > tempExecutiveActions;

	int iIdx = 0;
	for ( const HyGrammar* grammar : parsedGrammars )
	{
		if ( m_debugging )
			printf( "%d) %s -> %s : \n", ++iIdx, grammar->GetLeftSymbol().GetName().c_str(), grammar->m_rightString.c_str() );

		HyUInt32 terminalSymbolCount    = grammar->GetRightTerminalSymbolCount();
		HyUInt32 nonTerminalSymbolCount = grammar->GetRightSymbolCount() - terminalSymbolCount;

		if ( nonTerminalSymbolCount > 0 )
		{
			tempExecutiveActions.push_back( executiveActions.front() );
			executiveActions.pop_front();
			HyExecutiveAction& executiveAction = tempExecutiveActions.back();

			nonTerminalSymbolCountsByGrammar.push_back( std::make_pair( grammar, nonTerminalSymbolCount ) );
			if ( m_debugging )
				printf( "Push(%d)\n", nonTerminalSymbolCount );
		}
		else
		{
			if ( m_debugging )
				printf( "\n" );

			parserData.m_executiveActions.push_back( executiveActions.front() );
			executiveActions.pop_front();
			HyExecutiveAction& executiveAction = parserData.m_executiveActions.back();

			while ( --nonTerminalSymbolCountsByGrammar.back().second == 0 )
			{
				const HyGrammar* pGrammar = nonTerminalSymbolCountsByGrammar.back().first;
				HY_ENSURE( pGrammar, return false );

				nonTerminalSymbolCountsByGrammar.pop_back();

				HyExecutiveAction& executiveAction = tempExecutiveActions.back();
				parserData.m_executiveActions.push_back( executiveAction );
				tempExecutiveActions.pop_back();

				if ( pGrammar == &m_inputGrammars.front() )
					break;

				if ( nonTerminalSymbolCountsByGrammar.empty() )
					break;
			}
		}
	}

	return true;
}

void HyGrammarParser::_ProcessDebugging(
	int& iNextDebugLine,
	const int iTotalLine,
	std::list<HyToken>::const_iterator itrToken,
	std::list<HyToken>::const_iterator itrTokenEnd,
	const HySymbol* const pSymbol,
	const HyToken& nextToken)
{
	const HyToken& token = *itrToken;

	if(m_debugging && iNextDebugLine - token.m_line <= 1)
		printf("Symbol : %-30s, Token : %-20s, %-20s (%d)\n", pSymbol->GetName().c_str(), token.m_value.c_str(), nextToken.m_value.c_str(), token.m_line );
// 	else
// 		printf("\r%d/%d (%.2f%%)", token.m_iLine, iTotalLine, token.m_iLine * 100.0f / iTotalLine);
	if(iNextDebugLine != -2 && (iNextDebugLine == -1 || iNextDebugLine <= token.m_line))
	{
		if(iNextDebugLine > 0)
		{
			iNextDebugLine = token.m_line;
			printf("소스 --------------------------------------------------------------------------------------------------------------\n");
			for(std::list<HyToken>::const_iterator itrCurToken = itrToken; itrCurToken != itrTokenEnd; ++itrCurToken)
			{
				const HyToken& token = *itrCurToken;

				if(iNextDebugLine < token.m_line)
					break;

				printf("%s ", token.m_value.c_str());
			}
			printf("\n");
			printf("-------------------------------------------------------------------------------------------------------------------\n");
		}

		iNextDebugLine = -1;

		char szInput[64];
		std::cin.getline(szInput, sizeof(szInput) - 1);

		const char* const pszCommand = strtok(szInput, " ");
		if(pszCommand != NULL)
		{
			if(strcmp(pszCommand, "n") == 0)
			{
				const char* const pszLine = strtok(NULL, " ");
				if(pszLine == NULL)
					iNextDebugLine = token.m_line + 1;
				else
					iNextDebugLine = atoi(pszLine);
			}
			else if(strcmp(pszCommand, "c") == 0)
				iNextDebugLine = -2;
		}
	}
}

void HyGrammarParser::_GenerateDataFile()
{
	HyString fileName = HyStringUtil::Format( "%s.dat", m_name.c_str() );
	HyFileWriter file( fileName, "wb" );
	HY_ENSURE( file.IsOpened(), return );

	for ( auto& pair : m_symbols )
	{
		const HySymbol& symbol = pair.second;

		file.Write( (HyUInt8)( 1 ) );
		file.Write( symbol.GetName() );

		for ( const HyString& first : symbol.GetFirsts() )
		{
			file.Write( (HyUInt8)( 2 ) );
			file.Write( first );
		}
		for ( const HyString& follow : symbol.GetFollows() )
		{
			file.Write( (HyUInt8)( 3 ) );
			file.Write( follow );
		}
	}

	for ( auto& pair : m_symbols )
	{
		HyUInt32  symbolId = pair.first;
		HySymbol& symbol   = pair.second;

		file.Write( (HyUInt8)( 4 ) );
		file.Write( symbol.GetName() );

		std::map< HyInt32, std::map< HyInt32, std::set< const HyGrammar* > > >& mapSymbolGrammar = m_mapLLTable[ symbolId ];

		std::map< HyInt32, std::map< HyInt32, std::set< const HyGrammar* > > >::const_iterator itrSymbolGrammar;
		for ( itrSymbolGrammar = mapSymbolGrammar.begin(); itrSymbolGrammar != mapSymbolGrammar.end(); ++itrSymbolGrammar )
		{
			file.Write( (HyUInt8)( 5 ) );
			file.Write( (HyUInt8)( itrSymbolGrammar->first ) );

			const std::map< HyInt32, std::set<const HyGrammar*> >& mapSymbolGrammar2 = itrSymbolGrammar->second;

			std::map< HyInt32, std::set<const HyGrammar*> >::const_iterator itrSymbolGrammar2;
			for ( itrSymbolGrammar2 = mapSymbolGrammar2.begin(); itrSymbolGrammar2 != mapSymbolGrammar2.end(); ++itrSymbolGrammar2 )
			{
				file.Write( (HyUInt8)( 6 ) );
				file.Write( (HyUInt8)( itrSymbolGrammar2->first ) );

				const std::set<const HyGrammar*>& setGrammar = itrSymbolGrammar2->second;
				for ( const HyGrammar* eachGrammar : setGrammar )
				{
					file.Write( (HyUInt8)( 7 ) );
					file.Write( (HyUInt16)( eachGrammar->GetId() ) );
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	데이터 파일을 읽는다.
///
/// @return	반환 값 없음
////////////////////////////////////////////////////////////////////////////////////////////////////
HyVoid HyGrammarParser::_ReadDataFile()
{
	char szFileName[ 256 ];
	sprintf_s( szFileName, sizeof( szFileName ), "%s.dat", m_name.c_str() );

	HyFileReader file( szFileName, "rb" );
	if ( !file.IsOpened() )
		LOG( HyLog::Fatal, "pFile == NULL" );

//	m_mapLLTable.assign( m_symbols.size() + 1, std::map< HyInt32, std::map< HyInt32, std::set< const HyGrammar* > > >() );

	HySymbol* pSymbol = NULL;

	std::map< HyInt32, std::map< HyInt32, std::set< const HyGrammar* > > >*	p1;
	std::map< HyInt32, std::set< const HyGrammar* > >*						p2;
	std::set< const HyGrammar* >*											p3;

	HyInt32 index = 0;

	HyUInt8 type;
	while ( file.Read( type ) )
	{
		++index;

		switch ( type )
		{
		case 1:
			{
				HyString symbolName;
				file.Read( symbolName );

				pSymbol = _GetSymbol( symbolName );
			}
			break;
		case 2:
			{
				HyString first;
				file.Read( first );
				pSymbol->AddFirst( first );
			}
			break;
		case 3:
			{
				HyString follow;
				file.Read( follow );
				pSymbol->AddFollow( follow );
			}
			break;
		case 4:
			{
				HyString symbolName;
				file.Read( symbolName );

				HySymbol* symbol = _GetSymbol( symbolName );
				p1 = &m_mapLLTable[ symbol->GetId() ];
			}
			break;
		case 5:
			{
				HyUInt8 tokenType;
				file.Read( tokenType );

				p2 = &(*p1)[ tokenType ];
			}
			break;
		case 6:
			{
				HyUInt8 tokenType;
				file.Read( tokenType );

				p3 = &(*p2)[ tokenType ];
			}
			break;
		case 7:
			{
				HyUInt16 grammarId;
				file.Read( grammarId );

				p3->insert( m_inputGrammarVector[ grammarId ] );
			}
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	심볼을 반환한다.
///
/// @param	id	식별자
///
/// @return	심볼
////////////////////////////////////////////////////////////////////////////////////////////////////
HySymbol* HyGrammarParser::_GetSymbol( HyUInt32 id )
{
	std::map< HyUInt32, HySymbol >::iterator iter = m_symbols.find( id );
	if ( iter == m_symbols.end() )
		return NULL;

	return &iter->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	심볼을 반환한다.
///
/// @param	name	이름
///
/// @return	심볼
////////////////////////////////////////////////////////////////////////////////////////////////////
HySymbol* HyGrammarParser::_GetSymbol( const HyString& name )
{
	std::map< HyString, HySymbol* >::const_iterator iter = m_symbolsByName.find( name );
	if ( iter == m_symbolsByName.end() )
		return NULL;

	return iter->second;
}


int HyGrammarParser::_GetStringCount(const char* const Str1, const char* const Str2)
{
	char		Buf[1024];
	int			Count = 0;
	const char* Str;

	strcpy(Buf, Str1);
	Str = strtok(Buf, " ");
	do
	{
		if(!strcmp(Str, Str2))
			++Count;
	}
	while((Str = strtok(NULL, " ")) != NULL);

	return Count;
}
