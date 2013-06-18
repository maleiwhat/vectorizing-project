// �o�� MFC �d�ҭ�l�{���X�|�ܽd�p��ϥ� MFC Microsoft Office Fluent �ϥΪ̤���
// (�uFluent UI�v)�A�åB�ȴ��Ѭ��ѦҸ�ơA����
// MFC �ѦҤ� MFC C++ �{���w�n��
// �H�������q�l��󪺸ɥR�C
// ���� Fluent UI ���ƻs�B�ϥΩδ��o�����v���ګh���O���ѡC
// �p�� Fluent UI ���v�p�����ԲӸ�T�A�гy�X
// http://go.microsoft.com/fwlink/?LinkId=238214�C
//
// Copyright (C) Microsoft Corporation
// �ۧ@�v�Ҧ��A�ëO�d�@���v�Q�C

// stdafx.h : �i�b�����Y�ɤ��]�t�зǪ��t�� Include �ɡA
// �άO�g�`�ϥΫo�ܤ��ܧ�
// �M�ױM�� Include �ɮ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �q Windows ���Y�ư����`�ϥΪ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ���T�w�q������ CString �غc�禡

// ���� MFC ���ä@�Ǳ`���Υi����ĵ�i�T�����\��
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �֤߻P�зǤ���
#include <afxext.h>         // MFC �X�R�\��


#include <afxdisp.h>        // MFC Automation ���O



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �䴩�� Internet Explorer 4 �q�α��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �䴩�� Windows �q�α��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �\��ϩM����C�� MFC �䴩
#include <d3d9.h>
#include <d3dx9.h>
#include <d3d11.h>
#include <windows.h>
#include <d3dx11.h>

#include <dxerr.h>
#include <cassert>
#include <string>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <vector>
#include <sstream>
#include <tchar.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vtkSphereSource.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkXYPlotActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkTesting.h>

#define VTK_SMART_POINTER(x) \
	typedef vtkSmartPointer< x >    x##_Sptr; \
	typedef std::vector< x##_Sptr > x##_Sptrs;
VTK_SMART_POINTER(vtkXYPlotActor);
VTK_SMART_POINTER(vtkRenderWindow);
VTK_SMART_POINTER(vtkRenderWindowInteractor);

template<typename T, typename U> class has_member_Initialize_tester
{
private:
	template<U> struct helper;
	template<typename T> static char check(helper < &T::Initialize >*);
	template<typename T> static char(&check(...))[2];
public:
	enum { value = (sizeof(check<T> (0)) == sizeof(char)) };
};
template<char Doit, class T> struct static_Check_To_Initialize
{
	static void Do(T& ic)   { ic; }
};
template<class T> struct static_Check_To_Initialize<1, T>
{
	static void Do(T& ic)   { ic->Initialize(); }
};
static struct
{
	template<class T> operator vtkSmartPointer<T> ()
	{
		vtkSmartPointer<T> ptr = vtkSmartPointer<T>::New();
		static_Check_To_Initialize<has_member_Initialize_tester<T, void(T::*)()>::value, vtkSmartPointer<T> >::Do(
		    ptr);
		return ptr;
	}
}
vtkSmartNew;
static struct
{
	template<class T> operator vtkSmartPointer<T> ()
	{
		return vtkSmartPointer<T>::New();
	}
}
vtkOnlyNew;

//���쥨��
#define ReleaseCOM(x) { if(x){ x->Release();x = 0; }}

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                      \
	{                                                  \
		HRESULT hr = x;                                \
		if(FAILED(hr))                                 \
		{                                              \
			DXTrace(__FILE__, __LINE__, hr, _T(#x), TRUE); \
		}                                              \
	}
#endif
#else
#ifndef HR
#define HR(x) x;
#endif
#endif

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


