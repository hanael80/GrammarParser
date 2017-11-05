////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyWriter.h
///
/// @brief	HyWriter 클래스의 헤더 파일
///
/// @date	2012-12-14
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyStream.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyWriter
///
/// @brief	HyWriter의 설명
///
/// @remark	HyWriter의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyWriter
	:
	public HyStream
{
public:
	/// 생성자
	HyWriter();

	/// 소멸자
	virtual ~HyWriter();

public: // Stream
	HyBool IsInputStream() const { return false; }

	using HyStream::Serialize;
	virtual HyBool Serialize( HyVoid* pData, HyInt32 iSize)	{ return Write( pData, iSize );	}
	virtual HyBool Serialize( HyString& strData)			{ return Write( strData );		}

public:
	virtual HyBool Write( HyVoid* data, HyUInt32 size ) = 0;

	HyBool Write( HyInt8 cData)		{ return Write( (HyVoid*)( &cData ), sizeof( cData ) );		}
	HyBool Write( HyUInt8 ucData)	{ return Write( (HyVoid*)( &ucData ), sizeof( ucData ) );	}
	HyBool Write( HyUInt16 ushData)	{ return Write( (HyVoid*)( &ushData ), sizeof( ushData ) );	}
	HyBool Write( HyInt32 iData)	{ return Write( (HyVoid*)( &iData ), sizeof( iData ) );		}
	HyBool Write( HyUInt32 uiData)	{ return Write( (HyVoid*)( &uiData ), sizeof( uiData ) );	}

	HyBool Write( const HyString& strData)
	{
		HyUInt16 ushLength = (HyUInt16)( strData.length() );
		if ( !Write( ushLength ) ) return false;
		if ( !Write( (HyVoid*)( strData.c_str() ), ushLength ) ) return false;
		return true;
	}

	template <class T>
	HyBool Write( const T& data)
	{
		return data.Write(*this);
	}
};