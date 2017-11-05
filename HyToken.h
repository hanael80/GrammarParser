#pragma once


static bool _IsNumber(const char* pszNumber)
{
	if(strncmp(pszNumber, "0x", 2) == 0)
		pszNumber += 2;

	while(*pszNumber)
	{
		if(!(*pszNumber >= '0' && *pszNumber <= '9'))
			return false;

		++pszNumber;
	}

	return true;
}


struct TokenPos
{
	std::string		m_fileName;
	int				m_line;

	TokenPos()
	{
		Clear();
	}

	void Clear()
	{
		m_fileName.clear();
		m_line = 0;
	}
};


class HyToken
	:
	public HyObject
{
public:
	enum Type
	{
		typeNothing = 0,
		typeWord,
		typeNum,
		typeInt,
		typeChar,
		typeString,
		typeFinal
	};

public:
	HyString m_value; ///< 값
	Type     m_type;  ///< 종류
	TokenPos m_pos;   ///< 위치
	HyInt32  m_line;

public:
	static std::map< HyInt32, HyString >& IntStringTypes()
	{
		static std::map< HyInt32, HyString > instance;

		return instance;
	}

	static std::map< HyString, HyInt32 >& StringIntTypes()
	{
		static std::map< HyString, HyInt32 > instance;

		return instance;
	}

	/// 생성자
	HyToken();

public:
	static void Init();
	static void AddTokenType( const char* pszString );
	static void AddTokenType( int iType, const char* String );
	static const char* GetInput( std::list< HyToken >::iterator it, const std::list< HyToken >::iterator end, char* pszBuffer, int iBufferSize );
	static Type ValueToType( const char* pszTypeString );
	static Type StringToType( const char* str );

	const char* GetTypeString() const { return GetTypeString( m_type ); }

	static const char* GetTypeString( const Type eType );
};
