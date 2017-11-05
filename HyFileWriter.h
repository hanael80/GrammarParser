////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyFileWriter.h
///
/// @brief	HyFileWriter Ŭ������ ��� ����
///
/// @date	2012-12-14
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyWriter.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyFileWriter
///
/// @brief	HyFileWriter�� ����
///
/// @remark	HyFileWriter�� ����
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyFileWriter
	:
	public HyWriter
{
private:
	FILE* m_file; ///< ����

public:
	/// ������
	HyFileWriter();

	/// ������
	HyFileWriter( const HyString& filePath, const HyChar* mode );

	/// �Ҹ���
	virtual ~HyFileWriter();

	HyVoid SetFile( FILE* file ) { m_file = file; }

	/// ���� �ִ����� ���θ� ��ȯ�Ѵ�.
	HyBool IsOpened() const { return m_file != NULL; }

	/// �ݴ´�.
	HyVoid Close();

	using HyWriter::Write;

	/// ����Ѵ�.
	virtual HyBool Write( HyVoid* data, HyUInt32 size ) override;

	/// ������ �̿��ؼ� ����Ѵ�.
	HyBool WriteFormat( const HyChar* format, ... );
};
