#pragma once


class HyGrammar;
class HySymbol;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyAction
///
/// @brief	HyAction의 설명
///
/// @remark	HyAction의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyAction
	:
	public HyObject
{
public:
	struct Type
	{
		enum Enum
		{
			Shift = 1,
			Reduce,
			Accept,
			Goto,
			Error
		};
	};

public:
	/// 생성자
	HyAction( const HySymbol& symbol );

	/// 문자열로 변환한다.
	const HyChar* ToString( HyChar* buffer ) const;

public:
	const HyGrammar* m_grammar;     
	      int        m_iNumber;      
	      int        m_iStateNumber; 
	const HySymbol&  m_symbol;       
	      Type::Enum m_type;        
};
