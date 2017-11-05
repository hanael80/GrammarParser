////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file	HyPch.h
///
/// @brief	HyPch 헤더 파일
///
/// @date	2012-11-5
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>


typedef bool				HyBool;
typedef char				HyInt8;
typedef unsigned char		HyUInt8;
typedef short				HyInt16;
typedef unsigned short		HyUInt16;
typedef int					HyInt32;
typedef unsigned int		HyUInt32;
typedef __int64				HyInt64;
typedef unsigned __int64	HyUInt64;
typedef float				HySingle;
typedef double				HyDouble;

typedef long				HyLong;
typedef unsigned long		HyULong;

typedef unsigned char		uchar;
typedef unsigned long		ulong;
typedef char                HyChar;

typedef void				HyVoid;

typedef std::string			HyString;


#include "HyObject.h"
#include "HyLog.h"
#include "HyMacros.h"
#include "HyStringUtil.h"
#include <windows.h>
