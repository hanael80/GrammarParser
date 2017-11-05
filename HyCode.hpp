#pragma once


class SCode
{
public:
	SCode( const int iID, const std::vector< const char *>& vecTokenString, const bool bFlag = false ) : m_iID( iID ), m_bFlag( bFlag )
	{
		for ( HyUInt32 i = 0; i < vecTokenString.size(); ++i )
			m_listString.push_back( vecTokenString[ i ] );
	}


	SCode( const int iID, const char* const pszTokenString[], const bool bFlag = false ) : m_iID( iID ), m_bFlag( bFlag )
	{
		for ( HyUInt32 i = 0; pszTokenString[ i ] != NULL; ++i )
			m_listString.push_back( pszTokenString[ i ] );
	}


	SCode& Copy() const
	{
		std::vector<const char*> vecTokenString;
		for(std::list<std::string>::const_iterator it = m_listString.begin(); it != m_listString.end(); ++it)
		{
			const std::string& strString = *it;
			vecTokenString.push_back(strString.c_str());
		}

		SCode& Code = *new SCode(m_iID, vecTokenString, m_bFlag);

		return Code;
	}

	void Delete()
	{
		delete this;
	}

	void SetFlag(const bool bFlag) { m_bFlag = bFlag; }

	int								GetID()			const { return m_iID; }
	const std::list<std::string>&	GetStringList()	const { return m_listString; }
	bool							GetFlag()		const { return m_bFlag; }

private:
	int						m_iID;
	std::list<std::string>	m_listString;
	bool					m_bFlag;
};
