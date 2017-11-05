#pragma once


#include "HyStackContent.h"
#include <list>


class HyStack
	:
	public HyObject,
	public std::list<HyStackContent>
{
public:
	const char* GetString(char* const Buffer) const;

	void Pop();
	void Push(const HySymbol& symbol);
	void Push(const int iNumber, const HyToken* const pToken);
};
