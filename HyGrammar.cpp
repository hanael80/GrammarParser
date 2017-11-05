#include "PCH.h"
#include "HyGrammar.h"
#include "HySymbol.h"


HyGrammar::HyGrammar( HySymbol& headSymbol )
:
m_callBackFunction   ( NULL ),
m_leftSymbol ( headSymbol )
{
}

HyInt32 HyGrammar::GetNumber( const std::list< HyGrammar >& inputGrammars ) const
{
	HyInt32 number = -1;

	for ( const HyGrammar& eachGrammar : inputGrammars )
	{
		++number;
		if ( &eachGrammar.m_leftSymbol == &m_leftSymbol && eachGrammar.m_rightString == m_rightString )
			return number;
	}

	return -1;
}


HyUInt32 HyGrammar::GetRightSymbolCount() const
{
	return m_rightSymbols.size();
}

HyUInt32 HyGrammar::GetRightTerminalSymbolCount() const
{
	HyUInt32 count = 0;

	for ( const HySymbol* eachSymbol : m_rightSymbols )
		if ( eachSymbol->GetType() == HySymbol::typeTerminal )
			++count;

	return count;
}

bool HyGrammar::operator ==(const HyGrammar& grammar) const
{
	if(&m_leftSymbol == &grammar.m_leftSymbol && m_rightString == grammar.m_rightString)
		return true;

	return false;
}


bool HyGrammar::HasTerminalDerivation() const
{
	if (
		m_rightString.find( HyToken::GetTypeString( HyToken::typeWord ) ) == HyString::npos &&
		m_rightString.find( HyToken::GetTypeString( HyToken::typeNum  ) ) == HyString::npos )
		return false;

	return true;
}

HyVoid HyGrammar::GetSubGrammars( const std::list<HyGrammar>& inputGrammars, std::set< const HyGrammar* >& grammars ) const
{
	if ( !grammars.insert( this ).second )
		return;

	HyUInt32 index = 0;
	HyUInt32 count = 2;

	for ( const HySymbol* eachSymbol : m_rightSymbols )
	{
		if ( index++ >= count )
			break;

		if ( eachSymbol->GetType() == HySymbol::typeTerminal )
			continue;

		if ( eachSymbol->HasFirst( "[e]" ) )
			++count;

		for ( const HyGrammar& eachGrammar : inputGrammars )
		{
			if ( eachSymbol != &eachGrammar.m_leftSymbol )
				continue;

			eachGrammar.GetSubGrammars( inputGrammars, grammars );
		}
	}
}


const HySymbol* HyGrammarPosition::GetCurSymbol() const
{
	if(m_position >= static_cast<int>(m_grammar.m_rightSymbols.size()))
		return NULL;

	return m_grammar.m_rightSymbols[m_position];
}


std::list<HyGrammarPosition>& HyGrammarPosition::GetInputGrammarPositionInstance()
{
	static std::list<HyGrammarPosition> instance;
	return instance;
}
