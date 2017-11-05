#pragma once


#include "HyAction.h"
#include "HySymbol.h"
#include <set>


class HyState
	:
	public HyObject
{
private:
	HyUInt32                                    m_id;       ///< 식별자
	std::list< HyGrammarPosition >              m_grammars; ///< 문법 목록

public:
	std::map< HyString, HyUInt32 >              m_gotos;    ///< GOTO 목록
	std::map< HyString, std::list< HyAction > > m_actions;  ///< 액션 목록

public:
	/// 생성자
	HyState();

	/// 문법 목록을 반환한다.
 	std::list< HyGrammarPosition >& GetGrammars() { return m_grammars; }

	void AddGrammar(const HyState& state, const HySymbol& symbol);
	void AddGrammar(const std::list<HyGrammarPosition>& listGrammar, const HySymbol& symbol);
	void CreateGoto(const std::string& Str, const int iToNumber);
	int GetGoto(const std::string& strStr) const;
	bool HasGrammar(const HyGrammarPosition& grammarPosition) const;
	bool HasGoto(const std::string& Str) const;
	void Process(std::map<int, HyState>& mapState, const bool bRecursive);

	static const HyState* GetState(const std::map<int, HyState>& mapState, const int iID);
	static const HyState* GetState(const std::map<int, HyState>& mapState, const HyState& St);

	void AddAction(const HySymbol& symbol, const HyAction::Type::Enum eActionType, const int iNumber, const HyGrammar* const pGrammar = NULL);
	const std::list<HyAction>* GetAction(const std::string& strSymbolName) const;
};
