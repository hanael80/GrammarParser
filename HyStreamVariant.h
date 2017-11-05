/*
 * ------------------------------------------------------------------------------------------------
 *
 * Copyright(c) 2009, Hanael, Inc.
 *
 * ------------------------------------------------------------------------------------------------
 *
 * File:    HyStreamVariant.h
 * Date:    2011-5-19
 * Purpose: 
 *
 * ------------------------------------------------------------------------------------------------
 */


#pragma once


class HyStream;


class HyStreamVariant
	:
	public HyObject
{
private:
	HyStream&	m_stream;

public:
	HyStreamVariant( HyStream& stream )
		:
	m_stream( stream )
	{
	}

	template< typename Type >
	operator Type();
};
