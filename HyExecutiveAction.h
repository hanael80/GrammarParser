////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyExecutiveAction.h
///
/// @brief	HyExecutiveAction Ŭ������ ��� ����
///
/// @date	2012-11-3
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


class HyGrammar;
class HyToken;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyExecutiveAction
///
/// @brief	HyExecutiveAction�� ����
///
/// @remark	HyExecutiveAction�� ����
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyExecutiveAction
	:
	public HyObject
{
private:
	const HyGrammar&                      m_grammar; ///< ����
	      std::list< HyExecutiveAction* > m_actions; ///< �׼� ���
	      std::vector< const HyToken* >   m_tokens;  ///< ��ū ���

public:
	/// ������
	HyExecutiveAction( const HyGrammar& grammar );

	/// ������ ��ȯ�Ѵ�.
	const HyGrammar& GetGrammar() const { return m_grammar; }

	/// ��ū ����� ��ȯ�Ѵ�.
	std::vector< const HyToken* >& GetTokens() { return m_tokens; }

	/// ��ū ����� ��ȯ�Ѵ�.
	const std::vector< const HyToken* >& GetTokens() const { return m_tokens; }

	/// ����Ѵ�.
	HyVoid Print( const HyUInt32 depth ) const;

	/// ó���Ѵ�.
	HyBool Process() const;
};
