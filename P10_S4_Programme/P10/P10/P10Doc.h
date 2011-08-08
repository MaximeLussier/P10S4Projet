// P10Doc.h : interface de la classe CP10Doc
//


#pragma once


class CP10Doc : public CDocument
{
protected: // création à partir de la sérialisation uniquement
	CP10Doc();
	DECLARE_DYNCREATE(CP10Doc)

// Attributs
public:

// Opérations
public:

// Substitutions
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implémentation
public:
	virtual ~CP10Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
};


