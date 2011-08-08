// P10View.h : interface de la classe CP10View
//


#pragma once
#include "afxwin.h"


class CP10View : public CFormView
{
protected: // création à partir de la sérialisation uniquement
	CP10View();
	DECLARE_DYNCREATE(CP10View)

public:
	enum{ IDD = IDD_P10_FORM };

	// Attributs
public:
	CP10Doc* GetDocument() const;

	// Opérations
public:
	static CP10View * GetView();
	void positionCharge(float* posSpheric, float* posCart);
	void actualiserSensorBoxes();

	// Substitutions
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	virtual void OnInitialUpdate(); // premier appel après la construction

	// Implémentation
public:
	virtual ~CP10View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
public:
	void AfficherMessageConsole(CString message);
	CListBox ConsoleBoxControl;
	afx_msg void OnBnClickedRazerobutton();
	afx_msg void OnBnClickedFullstopbutton();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedStatusbutton();
	afx_msg void OnOpentrajectory();
	afx_msg void OnOpentarget();
	CStatic m_Picture;
	void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnOpenCompensators();
	afx_msg void OnSaveSensors();
	CStatic targetImageControl;
	CStatic BetaControl;
	CStatic AlphaControl;
	CStatic XwControl;
	CStatic YwControl;
	CStatic RControl;
	CStatic XcControl;
	CStatic YcControl;
	CStatic ZcControl;
	CStatic ImageBruitControl;
	CButton ButtonFiltreeControl;
	CButton ButtonBruiteControl;
	afx_msg void OnBnClickedBtnfiltree();
	afx_msg void OnBnClickedBtnbruite();
	afx_msg void OnFichierEnregisterimagefiltre();
	afx_msg void OnBnClickedBtnoriginale();
	CButton ButtonOriginalControl;
	CButton StartButtonControl;
	afx_msg void OnBnClickedButtontakepic();

	ofstream* Ofile;
	CStatic PSNR_Avant_Control;
	CStatic PSNR_Apres_Control;
	CStatic destinationControl;
	CStatic destination2Control;
};

#ifndef _DEBUG  // version de débogage dans P10View.cpp
inline CP10Doc* CP10View::GetDocument() const
{ return reinterpret_cast<CP10Doc*>(m_pDocument); }
#endif

