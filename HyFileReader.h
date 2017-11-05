////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyFileReader.h
///
/// @brief	HyFileReader Ŭ������ ��� ����
///
/// @date	2012-12-14
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyReader.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyFileReader
///
/// @brief	HyFileReader�� ����
///
/// @remark	HyFileReader�� ����
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyFileReader
	:
	public HyReader
{
private:
	FILE*	m_file; ///< ����

public:
	/// ������
	HyFileReader();

	/// ������
	HyFileReader( const HyString& filePath, const HyChar* mode );

	/// �Ҹ���
	virtual ~HyFileReader();

	HyVoid SetFile( FILE* file ) { m_file = file; }

	using HyReader::Read;
	HyBool Read( HyVoid* data, HyUInt32 size );

	HyBool ReadLine( HyString& lineStr );

	HyBool IsOpened() const { return m_file != NULL; }

	HyVoid Close();
};
