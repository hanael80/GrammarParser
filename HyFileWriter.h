////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyFileWriter.h
///
/// @brief	HyFileWriter 클래스의 헤더 파일
///
/// @date	2012-12-14
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyWriter.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyFileWriter
///
/// @brief	HyFileWriter의 설명
///
/// @remark	HyFileWriter의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyFileWriter
	:
	public HyWriter
{
private:
	FILE* m_file; ///< 파일

public:
	/// 생성자
	HyFileWriter();

	/// 생성자
	HyFileWriter( const HyString& filePath, const HyChar* mode );

	/// 소멸자
	virtual ~HyFileWriter();

	HyVoid SetFile( FILE* file ) { m_file = file; }

	/// 열려 있는지의 여부를 반환한다.
	HyBool IsOpened() const { return m_file != NULL; }

	/// 닫는다.
	HyVoid Close();

	using HyWriter::Write;

	/// 기록한다.
	virtual HyBool Write( HyVoid* data, HyUInt32 size ) override;

	/// 형식을 이용해서 기록한다.
	HyBool WriteFormat( const HyChar* format, ... );
};
