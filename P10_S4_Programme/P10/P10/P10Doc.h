// P10Doc.h : interface de la classe CP10Doc
//


#pragma once


class CP10Doc : public CDocument
{
protected: // cr�ation � partir de la s�rialisation uniquement
	CP10Doc();
	DECLARE_DYNCREATE(CP10Doc)

// Attributs
public:

// Op�rations
public:

// Substitutions
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Impl�mentation
public:
	virtual ~CP10Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions g�n�r�es de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
};


