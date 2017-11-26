#pragma once


#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <vector>


#define SIZEOF_BUFFER 1024 * 500
#define TYPEOF_ERROR_NORMAL                  1
#define TYPEOF_ERROR_UNEXPECTED_ENDOF_FILE   6


#include "HyGrammarParserConsts.h"
#include "HyCode.hpp"
#include "HyExecutiveAction.h"
#include "HyGrammar.h"
#include "HyStack.h"
#include "HyState.h"
#include "HySymbol.h"
#include "HyToken.h"
#include <iostream>


class ParserData
{
public:
	typedef std::list< HyExecutiveAction > ExecutiveActionList; ///< 실행 액션 목록
	typedef std::list< HyToken >           TokenList;           ///< 토큰 목록

public:
	ExecutiveActionList m_executiveActions; ///< 실행 액션 목록
	TokenList           m_tokens;           ///< 토큰 목록
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	문법 파서
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyGrammarParser
	:
	public HyObject
{
private:
	HyString                            m_name;               ///< 이름
	int                                 m_iCountOfErrors;
	int                                 m_iCountOfLines;
	int                                 m_CountOfWarnings;
	HyString                            m_strCompileString;
	std::list< HyGrammar >              m_inputGrammars;      ///< 입력 문법 목록
	std::vector< HyGrammar* >           m_inputGrammarVector; ///< 입력 문법 벡터
	std::map< HyUInt32, HySymbol >      m_symbols;            ///< 심볼 목록
	std::map< HyString, HySymbol* >     m_symbolsByName;      ///< 이름 별 심볼 목록
	std::map< int, HyState >            m_mapState;
	std::map< HyInt32, std::map< HyInt32, std::map< HyInt32, std::set< const HyGrammar* > > > > m_mapLLTable;
	HyBool                              m_debugging;          ///< 디버깅 여부

public:
	/// 생성자
	HyGrammarParser( const HyString& name );

	/// 디버깅 여부를 설정한다.
 	HyVoid SetDebugging( const HyBool& debugging ) { m_debugging = debugging; }

	/// 디버깅 여부를 반환한다.
	HyBool GetDebugging() const { return m_debugging; }

	bool Parse(const std::string& strFileString, ParserData& interpretData, void* const pParam, char* const InterpretingResult = NULL);
	bool ParseFile(const std::string& strFileName, ParserData& interpretData, void* const pParam, char* const pszInterpretingResult = NULL);

	/// 입력 문법을 추가한다.
	void AddInputGrammar( const HyChar* source, const HyChar* destination, GENERATION_CALLBACK callback = NULL );

protected:
	/// 초기화한다.
	HyVoid Init();

	/// 초기화할때를 처리한다.
	virtual void OnInit() = 0;

private:
	/// 심볼을 초기화한다.
	HyVoid _InitializeSymbols();

	/// 퍼스트를 초기화한다.
	HyUInt32 _InitializeFirsts();

	/// 팔로우를 초기화한다.
	HyUInt32 _InitializeFollows();

	/// 심볼을 추가한다.
	HySymbol* _AddSymbol( const HyString& name, HySymbol::Type type );

	/// 토큰을 생성한다.
	HyVoid _CreateToken( const HyChar* pszString, const TokenPos& tokenPos, ParserData& parserData );

	bool _LastTokenIsString(ParserData& interpretData) const;
	void _LexicalAnalysis(const std::string& strString, ParserData& interpretData);
	void _LogError(const HyToken& Tok, int Type, const HyToken* PrevToken);
	void _LogWarning(const HyToken& Tok, int Type);
	void _MakeAction();
	void _MakeLLTable();
	bool _ReadSourceFile(const std::string& strFileName, std::string& Source);
	void _CalFollowMap(const std::set<const HyGrammar*>& setGrammar, std::map<std::string, std::set<std::string> >& mapFollow) const;
	bool _SyntaxAnalysis(ParserData& interpretData);

	/// LL 파싱을 이용해 문법을 분석한다.
	HyBool _SyntaxAnalysis_LL( ParserData& parserData );

	/// 실행 액션 목록을 생성한다.
	HyBool _MakeExecutiveActions(
		ParserData& parserData,
		std::list< const HyGrammar* >& parsedGrammars,
		std::list< HyExecutiveAction >& executiveActions );

	/// 실행 액션 목록을 재구성한다.
	HyBool _RestructureExecutiveActions(
		std::list< const HyGrammar* >&  parsedGrammars,
		std::list< HyExecutiveAction >& tempExecutiveActions,
		ParserData&                     parserData );

private:
	void _ProcessDebugging(
		      int&                                 iNextDebugLine,
		      int                                  iTotalLine,
		      std::list< HyToken >::const_iterator itrToken,
		      std::list< HyToken >::const_iterator itrTokenEnd,
		const HySymbol*                            pSymbol,
		const HyToken&                             nextToken,
		const HyGrammar*&                          nextBreakGrammar,
		      HyBool                               debugBreak );

	void _GenerateDataFile( HyUInt64 hashCode );

	/// 데이터 파일을 읽는다.
	HyBool _ReadDataFile( HyUInt64 hashCode );

	int	_GetStringCount(const char* const Str1, const char* const Str2);

	/// 심볼을 반환한다.
	HySymbol* _GetSymbol( HyUInt32 id );

	/// 심볼을 반환한다.
	HySymbol* _GetSymbol( const HyString& name );

	/// calculate a hash code for input grammars
	HyUInt64 _calc_input_grammar_hash() const;
};

class CodeStackContent
{
public:
	std::string Type;
	int* Value;
};

class CodeStack : public std::list<CodeStackContent>
{
public:
	int Pop()
	{
		CodeStackContent& Content = back();
		int Value;
		if(Content.Type == "정수")
			Value = *Content.Value;
		else
			Value = (HyInt32)( (HyInt64)( Content.Value ) );
		pop_back();
		return Value;
	}

	void Push(const char* const Type, const int Value)
	{
		CodeStackContent Content;
		Content.Type = Type;
		Content.Value = (HyInt32*)(HyInt64)( Value );
		push_back(Content);
	}
};


/*
class SVariable
{
  public:
    int         m_Address;
    std::string	m_Name;
    int         m_Pos;
    std::string	m_Position;
    std::string	m_String;
    Token*     m_Tok;
    int         m_Type;
    BOOL        m_Used;

  public:
    SVariable*  AddNew(char*, char*, int, int);
    SVariable*  Get(char*, char*);
};*/

#ifndef _LIB
#pragma comment( lib, "HyGrammarParser.lib" )
#endif
