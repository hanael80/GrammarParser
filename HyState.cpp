#include "PCH.h"
#include "HyState.h"


HyState::HyState() : m_id(0)
{
}


void HyState::AddGrammar(const HyState& state, const HySymbol& symbol)
{
	AddGrammar(state.m_grammars, symbol);
}


void HyState::AddGrammar(const std::list<HyGrammarPosition>& listGrammar, const HySymbol& symbol)
{
	for ( const HyGrammarPosition& eachGrammarPosition : listGrammar )
	{
		const HyGrammar& grammar = eachGrammarPosition.m_grammar;

		if ( &grammar.GetLeftSymbol() != &symbol )
			continue;

		if ( HasGrammar( eachGrammarPosition ) )
			continue;

		m_grammars.push_back( eachGrammarPosition );

		if ( !eachGrammarPosition.IsEndPosition() )
			AddGrammar( listGrammar, *eachGrammarPosition.GetCurSymbol() );
	}
}


void HyState::CreateGoto(const std::string& Str, const int iToNumber)
{
	m_gotos.insert(std::make_pair(Str, iToNumber)).second;
}


HyInt32 HyState::GetGoto( const HyString& string ) const
{
	const std::map< HyString, HyUInt32 >::const_iterator iter = m_gotos.find( string );
	if ( iter == m_gotos.end() )
		return -1;

	return iter->second;
}

HyBool HyState::HasGrammar( const HyGrammarPosition& grammarPosition ) const
{
	for ( const HyGrammarPosition& eachGrammarPosition : m_grammars )
		if ( eachGrammarPosition == grammarPosition )
			return true;

	return false;
}


bool HyState::HasGoto(const std::string& Str) const
{
	return GetGoto(Str) != -1;
}


void HyState::Process( std::map< int, HyState >& mapState, const bool bRecursive )
{
	for ( const HyGrammarPosition& gr : m_grammars )
	{
		const HySymbol* const pSymbol = gr.GetCurSymbol();
		if(pSymbol == NULL)
			continue;

		const bool bHasGoto = HasGoto( pSymbol->GetName() );
		//if(HasGoto(pSymbol->m_strName))
		//	continue;

		HyState state;

		for ( const HyGrammarPosition& gr2 : m_grammars )
		{
			const HySymbol* const pSymbol2 = gr2.GetCurSymbol();

			if(pSymbol2 != pSymbol)
				continue;

			HyGrammarPosition gr3(gr2.m_grammar, gr2.m_position + 1);
			state.m_grammars.push_back(gr3);

			if ( !gr3.IsEndPosition() )
				state.AddGrammar( HyGrammarPosition::GetInputGrammarPositionInstance(), *gr3.GetCurSymbol() );
		}

		const HyState* const pState = GetState(mapState, state);

		if(pState != NULL)
		{
			CreateGoto (pSymbol->GetName(), pState->m_id );
			continue;
		}

		static HyUInt32 s_id = 0;

		state.m_id = ++s_id;
		mapState[state.m_id] = state;

		CreateGoto( pSymbol->GetName(), state.m_id );

		if(bRecursive)
			mapState.rbegin()->second.Process(mapState, true);
	}
}


const HyState* HyState::GetState( const std::map< int, HyState >& mapState, const int iID )
{
	const std::map< int, HyState >::const_iterator itr = mapState.find( iID );
	if ( itr == mapState.end() ) return NULL;

	const HyState& state = itr->second;
	return &state;
}


const HyState* HyState::GetState( const std::map< int, HyState >& states, const HyState& state )
{
	HyUInt32 grammarCount = state.m_grammars.size();

	for ( auto& pair : states )
	{
		const HyState& state = pair.second;

		if ( grammarCount != state.m_grammars.size() )
			continue;

		HyBool same = true;
		for ( const HyGrammarPosition& grammarPosition : state.m_grammars )
		{
			if ( !state.HasGrammar( grammarPosition ) )
			{
				same = false;
				break;
			}
		}

		if ( same )
			return &state;
	}

	return NULL;
}


void HyState::AddAction(const HySymbol& symbol, const HyAction::Type::Enum eActionType, const int iNumber, const HyGrammar* const pGrammar)
{
	HyAction action(symbol);

	action.m_iStateNumber	= m_id;
	action.m_type			= eActionType;
	action.m_iNumber		= iNumber;
	action.m_grammar		= pGrammar;

	m_actions[ symbol.GetName() ].push_back( action );
}


const std::list< HyAction >* HyState::GetAction( const HyString& symbolName ) const
{
	const std::map< HyString, std::list< HyAction > >::const_iterator iter = m_actions.find( symbolName );
	if ( iter == m_actions.end() )
		return NULL;

	return &iter->second;
}
