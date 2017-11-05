#pragma once


#include "HyAction.h"
#include "HySymbol.h"
#include <set>


class HyState
	:
	public HyObject
{
private:
	HyUInt32                                    m_id;       ///< �ĺ���
	std::list< HyGrammarPosition >              m_grammars; ///< ���� ���

public:
	std::map< HyString, HyUInt32 >              m_gotos;    ///< GOTO ���
	std::map< HyString, std::list< HyAction > > m_actions;  ///< �׼� ���

public:
	/// ������
	HyState();

	/// ���� ����� ��ȯ�Ѵ�.
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
