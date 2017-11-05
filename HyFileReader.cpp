////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyFileReader.cpp
///
/// @brief	HyFileReader Ŭ������ �ҽ� ����
///
/// @date	2012-12-14
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "PCH.h"
#include "HyFileReader.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	������
////////////////////////////////////////////////////////////////////////////////////////////////////
HyFileReader::HyFileReader()
:
m_file( NULL )
{
}

HyFileReader::HyFileReader( const HyString& filePath, const HyChar* mode )
{
	m_file = fopen( filePath.c_str(), mode );
	if ( !m_file )
		return;

	rewind( m_file );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	�Ҹ���
////////////////////////////////////////////////////////////////////////////////////////////////////
HyFileReader::~HyFileReader()
{
	Close();
}

HyBool HyFileReader::Read( HyVoid* data, HyUInt32 size )
{
	if ( !m_file )
		return false;

	HyUInt32 totalSize = 0;
	while ( totalSize < size )
	{
		HyInt32 curSize = fread( (HyChar*)( data ) + totalSize, 1, size - totalSize, m_file );
		if ( curSize <= 0 )
			return false;

		totalSize += curSize;
	}

	return true;
}

HyBool HyFileReader::ReadLine( HyString& lineStr )
{
	HyChar buf[ 1024 ];
	if ( !fgets( buf, sizeof( buf ), m_file) )
		return false;

	lineStr = buf;
	return true;
}

HyVoid HyFileReader::Close()
{
	if ( !m_file )
		return;

	fclose( m_file );
	m_file = NULL;
}
