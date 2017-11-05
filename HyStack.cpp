#include "PCH.h"
#include "HyStack.h"
#include "HySymbol.h"


const char* HyStack::GetString(char* const Buffer) const
{
	*Buffer = 0;

	for(std::list<HyStackContent>::const_iterator it = begin(); it != end(); ++it)
	{
		const HyStackContent& stackContent = *it;
		/*if(Con.m_Type == SStack::typeState)
			strcat(Buffer, itoa(Con.m_Number, Buf, 10));
		else
			strcat(Buffer, Con.m_Symbol->m_Name.c_str());
		strcat(Buffer, "$");
		*/
		if(stackContent.m_Type == HyStackContent::typeSymbol)
		{
			strcat(Buffer, stackContent.m_pSymbol->GetName().c_str());
			strcat(Buffer, " ");
		}
	}

	return Buffer;
}


void HyStack::Pop()
{
	pop_back();
}


void HyStack::Push(const HySymbol& symbol)
{
	HyStackContent content;

	content.m_Number = symbol.GetId();
	content.m_Type = HyStackContent::typeSymbol;
	content.m_pSymbol = &symbol;

	push_back(content);
}


void HyStack::Push(const int iNumber, const HyToken* const pToken)
{
	HyStackContent content;

	content.m_Number = iNumber;
	content.m_Type = HyStackContent::typeState;
	content.m_Token = pToken;

	push_back(content);
}
