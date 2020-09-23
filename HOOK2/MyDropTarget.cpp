#include "MyDropTarget.h"
#include "Shobjidl.h"


MyDropTarget::MyDropTarget()
{
}

MyDropTarget::~MyDropTarget()
{
}

BOOL MyDropTarget::RegisterDropTarget(HWND hTargetWnd/*, IDropTargetNotify *pDropTargetNotify*/)
{
	HRESULT hr = E_FAIL;

	if (IsWindow(hTargetWnd) && (m_hTargetWnd != hTargetWnd))
	{
		hr = ::RegisterDragDrop(hTargetWnd, this);
		if (SUCCEEDED(hr))
		{
			//m_pDropTargetNotify = pDropTargetNotify;
			m_hTargetWnd = hTargetWnd;
		}
		else
		{
			m_hTargetWnd = NULL;
		}
	}

	return SUCCEEDED(hr) ? TRUE : FALSE;
}

STDMETHODIMP MyDropTarget::DragEnter(
	IDataObject* pDataObj, DWORD grfKeyState,
	POINTL pt, DWORD* pdwEffect)
{
	/*
	AFX_MANAGE_STATE(AfxGetStaticModuleState());    // init MFC
	OutputDebugString(CString("DragEnter\n"));
	COleDataObject dataobj;
	TCHAR          szItem[MAX_PATH];
	UINT           uNumFiles;
	HGLOBAL        hglobal;
	HDROP          hdrop;

	dataobj.Attach(pDataObj, FALSE); // attach to the IDataObject, don't auto-release it
									 //AfxMessageBox("CSendToShlExt::DragEnter");
									 // Read the list of items from the data object.  They're stored in HDROP
									 // form, so just get the HDROP handle and then use the drag 'n' drop APIs
									 // on it.
	hglobal = dataobj.GetGlobalData(CF_HDROP);

	if (NULL != hglobal)
	{
		hdrop = (HDROP)GlobalLock(hglobal);

		uNumFiles = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

		for (UINT uFile = 0; uFile < uNumFiles; uFile++)
		{
			if (0 != DragQueryFile(hdrop, uFile, szItem, MAX_PATH))
				m_lsDroppedFiles.AddTail(szItem);
		}

		GlobalUnlock(hglobal);
	}

	if (m_lsDroppedFiles.GetCount() > 0)
	{
		*pdwEffect = DROPEFFECT_COPY;
		return S_OK;
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
		return E_INVALIDARG;
	}
	*/
	return E_INVALIDARG;
}

STDMETHODIMP MyDropTarget::Drop(
	IDataObject* pDataObj, DWORD grfKeyState,
	POINTL pt, DWORD* pdwEffect)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());    // init MFC
	//OutputDebugString(CString("Drop\n"));
	//CSendToCloneDlg dlg(&m_lsDroppedFiles);
	//CActCtx ctx;

	//dlg.DoModal();

	*pdwEffect = DROPEFFECT_COPY;
	return E_INVALIDARG;;
}


//--------------------------------------------------------------------------
STDMETHODIMP MyDropTarget::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(MyDropTarget, IDropTarget),
		{ 0 },
	};

	return QISearch(this, qit, riid, ppv);
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(ULONG) MyDropTarget::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(ULONG) MyDropTarget::Release()
{
	ULONG lRef = InterlockedDecrement(&m_lRefCount);
	if (0 == lRef)
	{
		delete this;
	}
	return m_lRefCount;
}