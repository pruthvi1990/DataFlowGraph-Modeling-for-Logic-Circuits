// stdafx.h : include file for standard system include files,
//      or project specific include files that are used often,
//      but are changed infrequently

#pragma once

#define TEST_META_CONFORMANCE_INSIDE_BON

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS

#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

