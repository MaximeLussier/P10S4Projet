// P10Doc.cpp : implémentation de la classe CP10Doc
//

#include "stdafx.h"
#include "P10.h"

#include "P10Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CP10Doc

IMPLEMENT_DYNCREATE(CP10Doc, CDocument)

BEGIN_MESSAGE_MAP(CP10Doc, CDocument)
END_MESSAGE_MAP()


// construction ou destruction de CP10Doc

CP10Doc::CP10Doc()
{
	// TODO : ajoutez ici le code d'une construction unique

}

CP10Doc::~CP10Doc()
{
}

BOOL CP10Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO : ajoutez ici le code de réinitialisation
	// (les documents SDI réutiliseront ce document)

	return TRUE;
}




// sérialisation de CP10Doc

void CP10Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO : ajoutez ici le code de stockage
	}
	else
	{
		// TODO : ajoutez ici le code de chargement
	}
}


// diagnostics pour CP10Doc

#ifdef _DEBUG
void CP10Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CP10Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// commandes pour CP10Doc
