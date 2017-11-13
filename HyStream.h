////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyStream.h
///
/// @brief	HyStream 클래스의 헤더 파일
///
/// @date	2012-12-14
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "HyStreamVariant.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HyStream
///
/// @brief	HyStream의 설명
///
/// @remark	HyStream의 각주
////////////////////////////////////////////////////////////////////////////////////////////////////
class HyStream
	:
	public HyObject
{
public:
	virtual HyBool IsInputStream() const = 0;

	virtual HyBool Serialize( HyVoid* data, HyInt32 size ) = 0;
	virtual HyBool Serialize( HyBool& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HyInt8& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HyUInt8& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HyInt16& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HyUInt16& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HyInt32& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HyInt64& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HyUInt64& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HySingle& value )		{ return Serialize( &value, sizeof( value ) ); }
	virtual HyBool Serialize( HyString& value ) = 0;

	template< typename Type >
	Type Serialize()
	{
		Type obj;
		Serialize( obj );
		return obj;
	}

	HyStreamVariant Serialize()
	{
		return HyStreamVariant( *this );
	}

	template< typename ObjType, typename ReturnType >
	ReturnType SerializeAndConvert()
	{
		ObjType obj;
		Serialize( obj );
		return (typename ReturnType)( obj );
	}

	template< typename Type >
	HyBool Serialize( Type& value )
	{
		return value.Serialize( *this );
	}
};


#include "HyStreamVariant.inl"
