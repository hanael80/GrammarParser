#pragma once

#include <mutex>
#include <string>


#pragma warning(disable:4996)


class HyLog
{
public:
	class EventListener
	{
	protected:
		friend class HyLog;
		virtual void OnLog(const char* const pszContents) = 0;
	};

public:
	enum Type
	{
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};

public:
	static HyLog& Instance();
	static HyLog& Instance2();

private:
	HyLog();

public:
	void Log(const Type type, const char* pszFormat, ...);
	bool Log(const char* const pszFileName, const char* const pszString);
	HyLog& SetInfo(const char* const pszFunctionName, const int iLine);
	void SetEventListener(EventListener& eventListener) { m_pEventListener = &eventListener; }

	static void	Init(const char* const pszFileName, const bool bWithSeperateLog = false);
	static void	Init2(const char* const pszFileName, const bool bWithSeperateLog = false);

private:
	HyLog& HandleError(const char* const pszFunctionFullName);

	static char* FindFunctionBegin(char* pszFunctionName, const short shLength);
	static char* FindFunctionEnd(char* pszFunctionName, const short shLength);

private:
	std::string			m_strFileName;
	bool				m_bWithSeperateLog;

	char				m_szFunctionName[512];
	const char*			m_pszFunctionName;
	EventListener*		m_pEventListener;
	int					m_iLine;

	std::mutex			m_mutex;
};

#ifdef WIN32
#define LOG		HyLog::Instance().SetInfo(__FUNCTION__, __LINE__).Log
#define LOG2	HyLog::Instance2().SetInfo(__FUNCTION__, __LINE__).Log
#else
#define LOG		HyLog::Instance().SetInfo(__PRETTY_FUNCTION__, __LINE__).Log
#endif
