#pragma once


#include <stdio.h>


class HyAction;
class HySymbol;
class HyToken;


class HyStackContent
	:
	public HyObject
{
public:
	enum Type
	{
		typeState = 1,
		typeSymbol
	};

public:
	const HyAction*	m_Act;
	int				m_Number;
	const HySymbol*	m_pSymbol;
	const HyToken*	m_Token;
	Type			m_Type;

public:
	HyStackContent() : m_Token(NULL) {}
};
