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
	typedef std::list< HyExecutiveAction > ExecutiveActionList; ///< ���� �׼� ���
	typedef std::list< HyToken >           TokenList;           ///< ��ū ���

public:
	ExecutiveActionList m_executiveActions; ///< ���� �׼� ���
	TokenList           m_tokens;           ///< ��ū ���
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	���� �ļ�
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyGrammarParser
	:
	public HyObject
{
private:
	HyString                            m_name;               ///< �̸�
	int                                 m_iCountOfErrors;
	int                                 m_iCountOfLines;
	int                                 m_CountOfWarnings;
	HyString                            m_strCompileString;
	std::list< HyGrammar >              m_inputGrammars;      ///< �Է� ���� ���
	std::vector< HyGrammar* >           m_inputGrammarVector; ///< �Է� ���� ����
	std::map< HyUInt32, HySymbol >      m_symbols;            ///< �ɺ� ���
	std::map< HyString, HySymbol* >     m_symbolsByName;      ///< �̸� �� �ɺ� ���
	std::map< int, HyState >            m_mapState;
	std::map< HyInt32, std::map< HyInt32, std::map< HyInt32, std::set< const HyGrammar* > > > > m_mapLLTable;
	HyBool                              m_debugging;          ///< ����� ����

public:
	/// ������
	HyGrammarParser( const HyString& name );

	/// ����� ���θ� �����Ѵ�.
 	HyVoid SetDebugging( const HyBool& debugging ) { m_debugging = debugging; }

	/// ����� ���θ� ��ȯ�Ѵ�.
	HyBool GetDebugging() const { return m_debugging; }

	bool Parse(const std::string& strFileString, ParserData& interpretData, void* const pParam, char* const InterpretingResult = NULL);
	bool ParseFile(const std::string& strFileName, ParserData& interpretData, void* const pParam, char* const pszInterpretingResult = NULL);

	/// �Է� ������ �߰��Ѵ�.
	void AddInputGrammar( const HyChar* source, const HyChar* destination, GENERATION_CALLBACK callback = NULL );

protected:
	/// �ʱ�ȭ�Ѵ�.
	HyVoid Init();

	/// �ʱ�ȭ�Ҷ��� ó���Ѵ�.
	virtual void OnInit() = 0;

private:
	/// �ɺ��� �ʱ�ȭ�Ѵ�.
	HyVoid _InitializeSymbols();

	/// �۽�Ʈ�� �ʱ�ȭ�Ѵ�.
	HyUInt32 _InitializeFirsts();

	/// �ȷο츦 �ʱ�ȭ�Ѵ�.
	HyUInt32 _InitializeFollows();

	/// �ɺ��� �߰��Ѵ�.
	HySymbol* _AddSymbol( const HyString& name, HySymbol::Type type );

	/// ��ū�� �����Ѵ�.
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

	/// LL �Ľ��� �̿��� ������ �м��Ѵ�.
	HyBool _SyntaxAnalysis_LL( ParserData& parserData );

	/// ���� �׼� ����� �����Ѵ�.
	HyBool _MakeExecutiveActions(
		ParserData& parserData,
		std::list< const HyGrammar* >& parsedGrammars,
		std::list< HyExecutiveAction >& executiveActions );

	/// ���� �׼� ����� �籸���Ѵ�.
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

	/// ������ ������ �д´�.
	HyBool _ReadDataFile( HyUInt64 hashCode );

	int	_GetStringCount(const char* const Str1, const char* const Str2);

	/// �ɺ��� ��ȯ�Ѵ�.
	HySymbol* _GetSymbol( HyUInt32 id );

	/// �ɺ��� ��ȯ�Ѵ�.
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
		if(Content.Type == "����")
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
