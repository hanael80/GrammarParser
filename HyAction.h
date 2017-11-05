#pragma once


class HyGrammar;
class HySymbol;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyAction
///
/// @brief	HyAction�� ����
///
/// @remark	HyAction�� ����
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
	/// ������
	HyAction( const HySymbol& symbol );

	/// ���ڿ��� ��ȯ�Ѵ�.
	const HyChar* ToString( HyChar* buffer ) const;

public:
	const HyGrammar* m_grammar;     
	      int        m_iNumber;      
	      int        m_iStateNumber; 
	const HySymbol&  m_symbol;       
	      Type::Enum m_type;        
};
