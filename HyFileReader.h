////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyFileReader.h
///
/// @brief	HyFileReader 클래스의 헤더 파일
///
/// @date	2012-12-14
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyReader.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyFileReader
///
/// @brief	HyFileReader의 설명
///
/// @remark	HyFileReader의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyFileReader
	:
	public HyReader
{
private:
	FILE*	m_file; ///< 파일

public:
	/// 생성자
	HyFileReader();

	/// 생성자
	HyFileReader( const HyString& filePath, const HyChar* mode );

	/// 소멸자
	virtual ~HyFileReader();

	HyVoid SetFile( FILE* file ) { m_file = file; }

	using HyReader::Read;
	HyBool Read( HyVoid* data, HyUInt32 size );

	HyBool ReadLine( HyString& lineStr );

	HyBool IsOpened() const { return m_file != NULL; }

	HyVoid Close();
};
