#pragma once
#include "oleidl.h"
#include <atlstr.h>

typedef DWORD DROPEFFECT;

class /*CLASS_DECLSPEC*/ MyDropTarget : 
	public IDropTarget
	//,public IUnknown
{
public:

	MyDropTarget();
	~MyDropTarget();

	BOOL RegisterDropTarget(HWND hWnd);
	//void RevokeDropTarget();
	//void AllowDragDrop(BOOL bAllowDrop = TRUE);
	// IDropTarget
	STDMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState,
		POINTL pt, DWORD* pdwEffect);

	STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
	{
		OutputDebugString(CString("DragOver\n")); return E_NOTIMPL;
	}

	STDMETHODIMP DragLeave()
	{
		OutputDebugString(CString("DragLeave\n")); return S_OK;
	}

	STDMETHODIMP Drop(IDataObject* pDataObj, DWORD grfKeyState,
		POINTL pt, DWORD* pdwEffect);

	//STDMETHOD(Execute)(LPSHELLEXECUTEINFOA pei) { OutputDebugString(CString("Execute\n")); return S_OK; }

	IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
	IFACEMETHODIMP_(ULONG) AddRef(void);
	IFACEMETHODIMP_(ULONG) Release(void);

private:

	//!< The flag indicates whether allow drag or not
	BOOL                m_isAllowDrag;
	//!< The flag indicates the data available or not     
	BOOL                m_isDataAvailable;
	//!< The target window handle
	HWND                m_hTargetWnd;
	//!< The reference count      
	volatile LONG       m_lRefCount;
	//!< The pointer of IDropTargetHelper interface
	//IDropTargetHelper  *m_pDropTargetHelper;
};