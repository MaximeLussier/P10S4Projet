// P10.h : fichier d'en-tête principal pour l'application P10
//
#pragma once

#ifndef __AFXWIN_H__
#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

 // symboles principaux
#include "resource.h"      
// Crane related imports
#include "craneexports.h"
#include "traitementImage.h"


#define GRUN 12000
#define GSTOP 13000
#define GRAZ 15000
#define PICTURE 18000

#define RED 1212
#define BLUE 1313
#define GREEN 1414
#define PINK 1515
#define UGLYGREEN 1616
// CP10App:
// Consultez P10.cpp pour l'implémentation de cette classe
//

class CP10App : public CWinApp
{
	/*********************
	*  Attributs
	**********************/
private:
	Crane* crane;
	bool executed;
public:
	// original picture
	CBitmap m_BMP_original;		// Concerve les éléments du bitmap
	CDC m_CDC_original;			// Conserve le contexte du bitmap utilisé
	double** M_Original;
	bool M_Original_loaded;
	// Picture with noise
	CBitmap m_BMP_bruite;		// Concerve les éléments du bitmap
	CDC m_CDC_bruite;			// Conserve le contexte du bitmap utilisé
	BITMAPFILEHEADER m_bmfh;	// Entête du fichier bitmap lue
	BITMAPINFOHEADER m_bmih;	// Description du bitmap lue
	double** M_Camera;
	bool M_Camera_loaded;
	// filtered
	CBitmap m_BMP_filtered;	// Concerve les éléments du bitmap
	CDC m_CDC_filtered;		// Conserve le contexte du bitmap utilisé
	// target
	BITMAPFILEHEADER m_bmfh_target;	// Entête du fichier bitmap lue
	BITMAPINFOHEADER m_bmih_target;	// Description du bitmap lue
	double** M_target;
	bool M_target_loaded;
	CBitmap m_BMP_target;  // Bitmap de la cible
	CDC m_CDC_target;		// Conserve le contexte du bitmap utilisé
	// reference
	BITMAPFILEHEADER m_bmfh_ref;	// Entête du fichier bitmap lue
	BITMAPINFOHEADER m_bmih_ref;	// Description du bitmap lue
	double** M_ref;
	bool M_ref_loaded;
	CBitmap m_BMP_ref;  // Bitmap de la cible
	CDC m_CDC_ref;		// Conserve le contexte du bitmap utilisé
	int tries;
	bool compLoaded;


	/***********************
	*  Fonctions
	***********************/

public:
	CP10App();
	~CP10App();
	virtual BOOL InitInstance();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	// Crane related Functions
	BOOL ConfigureCrane(char* compFilePath);
	Crane* getCraneInstance();
	float** GenTrajP10(double X3, double Y3, double Z3, long* TrajLength);
	void addCommandToStack(int command);
	void executeStack();
	void clearStack();
	void messages(int id);
	int commandStack[10];
	int currentCommand;
	void updateStack();
	bool isStable();
	bool CP10App::isAtDestination();

	// Picture related Functions
	void getPicture();
	BOOL loadImage(LPCTSTR lpszFileName, CStatic* ImageSquare, CBitmap* BMP, CDC* CDC, BITMAPFILEHEADER* m_bmfh, BITMAPINFOHEADER* bmih);
	double** loadMatrix(int M, int N, CDC* m_cdc);
	int	 LoadBitmapHeader(LPCTSTR lpszFileName, BITMAPFILEHEADER* bmfh, BITMAPINFOHEADER* m_bmih);
	void DisplayImage(CStatic* ImageSquare, CBitmap* bmp, CDC* cdc);
	void MatrixCalcul();
	void ChargerCible(char* targetPath);
	int  ValidePixel(int x);
	bool LireFilterAfficherImage(LPCTSTR lpszFileName, CStatic* ImageSquare);
	void updateFromMatrix(double** X, int M, int N, CDC* m_cdc);
	void sauvegarde();
	void ChargerRef();
	void pointeurCouleur(int couleur, int M, int N, int size, CDC* m_cdc);
	

};

extern CP10App theApp;