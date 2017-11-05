#include "PCH.h"
#include "HyLog.h"


HyLog& HyLog::Instance()
{
	static HyLog Log;

	return Log;
}

HyLog& HyLog::Instance2()
{
	static HyLog Log;

	return Log;
}

HyLog::HyLog()
	:
	m_bWithSeperateLog( false ),
	m_pEventListener( NULL )
{
}

void HyLog::Log( const Type type, const char* pszFormat, ... )
{
	char szBuf[ 1024 * 100 ];
	szBuf[ sizeof( szBuf ) - 1 ] = 0;

	va_list arg;

	va_start( arg, pszFormat );
	vsprintf_s( szBuf, sizeof( szBuf ) - 1, pszFormat, arg );
	va_end( arg );

	SYSTEMTIME localTime;
	GetLocalTime( &localTime );

	char szFileName[ 256 ];
	szFileName[ sizeof( szFileName ) - 1 ] = 0;

	sprintf_s(
		szFileName,
		sizeof( szFileName ) - 1,
		"%s%04d%02d%02d.log",
		m_strFileName.c_str(),
		localTime.wYear,
		localTime.wMonth,
		localTime.wDay );

	const char* pszTypeString;
	switch ( type )
	{
		case Debug:			pszTypeString = "Debug";	break;
		case Info:			pszTypeString = "Info";		break;
		case Warn:			pszTypeString = "Warn";		break;
		case Error:			pszTypeString = "Error";	break;
		case Fatal:			pszTypeString = "Fatal";	break;
		default:			pszTypeString = "Invalid";	break;
	}

	char szFinalLog[ sizeof( szBuf ) ];
	szFinalLog[ sizeof( szFinalLog ) - 1 ] = 0;

	sprintf_s(
		szFinalLog,
		sizeof( szFinalLog ) - 1,
		"%04d-%02d-%02d %02d:%02d:%02d.%03d|%s|%s|%d|%s",
		localTime.wYear,
		localTime.wMonth,
		localTime.wDay,
		localTime.wHour,
		localTime.wMinute,
		localTime.wSecond,
		localTime.wMilliseconds,
		pszTypeString,
		m_pszFunctionName,
		m_iLine,
		szBuf );

	if ( Log( szFileName, szFinalLog ) == false )
	{
		m_mutex.unlock();
		return;
	}

	if ( NULL != m_pEventListener )
		m_pEventListener->OnLog( szFinalLog );

	if ( m_bWithSeperateLog )
	{
		sprintf_s(
			szFileName,
			sizeof( szFileName ) - 1,
			"%s_%s_%04d%02d%02d.log",
			m_strFileName.c_str(),
			pszTypeString,
			localTime.wYear,
			localTime.wMonth,
			localTime.wDay );
		Log( szFileName, szFinalLog );
	}

	if ( type == Fatal )
	{
		__debugbreak();
	}

	m_mutex.unlock();
}

bool HyLog::Log( const char* const pszFileName, const char* const pszString )
{
	if ( pszFileName == NULL || pszString == NULL )
		return false;

	FILE* const pFile = fopen( pszFileName, "a" );
	if ( pFile == NULL )
		return false;

	fputs( pszString, pFile );
	fputs( "\n", pFile );
	fclose( pFile );

	return true;
}

HyLog& HyLog::SetInfo( const char* const pszFunctionName, const int iLine )
{
	m_mutex.lock();

	m_pszFunctionName = pszFunctionName;
	m_iLine = iLine;

	return *this;
}

void HyLog::Init( const char* const pszFileName, const bool bWithSeperateLog )
{
	Instance().m_strFileName = pszFileName;
	Instance().m_bWithSeperateLog = bWithSeperateLog;
}

void HyLog::Init2( const char* const pszFileName, const bool bWithSeperateLog )
{
	Instance2().m_strFileName = pszFileName;
	Instance2().m_bWithSeperateLog = bWithSeperateLog;
}

char* HyLog::FindFunctionBegin(char* pszFunctionName, const short shLength)
{
	int iCount = 0;
	char* p = pszFunctionName + shLength - 1;
	for(; p >= pszFunctionName; --p)
	{
		if(*p == ' ' && p - 8 >= pszFunctionName && strncmp(p - 8, "operator", 8))
			return p + 1;
		else if(!strncmp(p, "::", 2) && ++iCount == 2)
			return p + 2;
	}

	return NULL;
}

char* HyLog::FindFunctionEnd( char* pszFunctionName, const short shLength )
{
	char* p = pszFunctionName + shLength - 1;
	for ( ; p >= pszFunctionName; --p )
		if ( *p == '(' )
			return p;

	return NULL;
}
