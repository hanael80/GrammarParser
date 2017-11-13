////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyReader.h
///
/// @brief	HyReader Ŭ������ ��� ����
///
/// @date	2012-12-14
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyStream.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyReader
///
/// @brief	HyReader�� ����
///
/// @remark	HyReader�� ����
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyReader
	:
	public HyStream
{
public:
	/// ������
	HyReader();

	/// �Ҹ���
	virtual ~HyReader();

public: // Stream
	bool IsInputStream() const { return true; }

	using HyStream::Serialize;
	virtual HyBool Serialize( HyVoid* pData, HyInt32 iSize )	{ return Read( pData, iSize );	}
	virtual HyBool Serialize( HyString& strData )				{ return Read( strData );		}

public:
	virtual HyBool Read( HyVoid* data, HyUInt32 size ) = 0;

	HyBool Read( HyUInt8& data)		{ return Serialize( data );	}
	HyBool Read( HyUInt16& data)	{ return Serialize( data );	}
	HyBool Read( HyInt32& data)		{ return Serialize( data );	}
	HyBool Read( HyUInt64& data)	{ return Serialize( data );	}
	HyBool Read( HySingle& data)	{ return Serialize( data );	}

	HyBool Read( HyString& data )
	{
		char szBuf[1024];

		unsigned short ushLength;
		if(!Read(ushLength))
			return false;

		if(sizeof(szBuf) <= ushLength)
			return false;

		if(!Read(szBuf, ushLength))
			return false;

		szBuf[ushLength] = 0;
		data = szBuf;

		return true;
	}

	template <class T>
	HyBool Read(T& data)
	{
		return data.Read(*this);
	}
};
