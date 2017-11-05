/*
 * ------------------------------------------------------------------------------------------------
 *
 * Copyright(c) 2009, Hanael, Inc.
 *
 * ------------------------------------------------------------------------------------------------
 *
 * File:    HyStreamVariant.inl
 * Date:    2011-5-19
 * Purpose: 
 *
 * ------------------------------------------------------------------------------------------------
 */


#pragma once


template< typename Type >
HyStreamVariant::operator Type()
{
	return m_stream.Serialize< Type >();
}