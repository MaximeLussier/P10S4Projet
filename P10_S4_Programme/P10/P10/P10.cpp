// P10.cpp : Définit les comportements de classe pour l'application.
//

#include "stdafx.h"
#include "P10.h"
#include "MainFrm.h"

#include "P10Doc.h"
#include "P10View.h"
#include <stdlib.h>

/*****************************************************************************
*                                                    Crane related Includes *
******************************************************************************/
#include <math.h>
#include <time.h>
#include "fftw3.h"


/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
*/
/*****************************************************************************
*                                                    Crane related Constants *
******************************************************************************/

#define MEANITER    10                  // Nombre d'itérations utilises pour 
// calculer la moyenne des données des capteurs
#define PI          3.141592653589793   // Constante pi
#define TOLANG      (5.0/180.0*PI)      // Tolérance sur l'angle pour considérer que
// la grue est stabilisée
#define TOLPOSX     0.05               // Tolérance sur la position pour considérer que
#define TOLPOSY     0.05               // la grue est stabilisée
#define TOLPOSZ     0.005

#define FS			1000                // Fréquence d'échantillonnages

// CP10App

BEGIN_MESSAGE_MAP(CP10App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CP10App::OnAppAbout)
	// Commandes de fichier standard
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// construction CP10App

CP10App::CP10App()
{
#ifdef _DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	_CrtSetDbgFlag(flag);
	// _CrtSetBreakAlloc(622026); // Comment or un-comment on need basis to id mem leaks
#endif
	// TODO : ajoutez ici du code de construction,
	// Placez toutes les initialisations significatives dans InitInstance
	M_Camera_loaded = false;
	M_target_loaded = false;
	M_Original_loaded = false;
	M_ref_loaded = false;
	executed = false;
	compLoaded = false;
	currentCommand = 1;
	tries = 0;

}

CP10App::~CP10App()
{
	if(M_Camera_loaded)
		MVOps::deleteMatrix(M_Camera, m_bmih.biHeight);
	if(M_target_loaded)
		MVOps::deleteMatrix(M_target, m_bmih_target.biHeight);
	if(M_ref_loaded)
		MVOps::deleteMatrix(M_ref, m_bmih_ref.biHeight);
	if(M_Original_loaded)
		MVOps::deleteMatrix(M_Original,  m_bmih.biHeight);
}

// Seul et unique objet CP10App

CP10App theApp;


// initialisation de CP10App

BOOL CP10App::InitInstance()
{
	// InitCommonControlsEx() est requis sur Windows XP si le manifeste de l'application
	// spécifie l'utilisation de ComCtl32.dll version 6 ou ultérieure pour activer les
	// styles visuels.  Dans le cas contraire, la création de fenêtres échouera.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// À définir pour inclure toutes les classes de contrôles communs à utiliser
	// dans votre application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialiser les bibliothèques OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Initialisation standard
	// Si vous n'utilisez pas ces fonctionnalités et que vous souhaitez réduire la taille
	// de votre exécutable final, vous devez supprimer ci-dessous
	// les routines d'initialisation spécifiques dont vous n'avez pas besoin.
	// Changez la clé de Registre sous laquelle nos paramètres sont enregistrés
	// TODO : modifiez cette chaîne avec des informations appropriées,
	// telles que le nom de votre société ou organisation
	SetRegistryKey(_T("Applications locales générées par AppWizard"));
	LoadStdProfileSettings(4);  // Charge les options de fichier INI standard (y compris les derniers fichiers utilisés)
	// Inscrire les modèles de document de l'application. Ces modèles
	//  lient les documents, fenêtres frame et vues entre eux
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CP10Doc),
		RUNTIME_CLASS(CMainFrame),       // fenêtre frame SDI principale
		RUNTIME_CLASS(CP10View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Activer les ouvertures d'exécution DDE
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Analyser la ligne de commande pour les commandes shell standard, DDE, ouverture de fichiers
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Commandes de dispatch spécifiées sur la ligne de commande. Retournent FALSE si
	// l'application a été lancée avec /RegServer, /Register, /Unregserver ou /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// La seule fenêtre a été initialisée et peut donc être affichée et mise à jour
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// appelle DragAcceptFiles uniquement s'il y a un suffixe
	//  Dans une application SDI, cet appel doit avoir lieu juste après ProcessShellCommand
	// Activer les ouvertures via glisser-déplacer
	m_pMainWnd->DragAcceptFiles();

	// *************** Code P10 non Autogénéré *********************
	// Création de l'objet grue.
	crane = new Crane();
	// Configuration de la grue;
	if (!ConfigureCrane("controller.ini")){
		CP10View::GetView()->AfficherMessageConsole(CString("Impossible de configurer la grue\n"));
	}
	ChargerRef();
	//	getPicture(); // TODO: Comment this when final
	return TRUE;
}

// Fonction de configuration de la grue
BOOL CP10App::ConfigureCrane(char* compFilePath)
{
	// Configurer les compensateurs de la grue avec un fichier
	int ret = crane->LoadCompFile(compFilePath);
	if(ret == 0){
		CP10View::GetView()->AfficherMessageConsole(CString("Fichier de configuration chargé avec succès."));
		return TRUE;
		compLoaded = true;
	}
	messages(ret);
	return FALSE;
}

//------------------------------------------
//	Fonction pour obtenir l'instance courrante de la grue
//	Retourne: pointeur vers l'instance courante de la grue
//	Auteur: Maxime Lussier
//------------------------------------------
Crane* CP10App::getCraneInstance()
{
	return crane;
}

// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Données de boîte de dialogue
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	// Implémentation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// Commande App pour exécuter la boîte de dialogue
void CP10App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

//-------------------------------------------------
// Lecture de l'entête du fichier image pour s'assurer du contenu, 
// du format et des dimensions

//------------------------------------------
//
// Cette méthode est utilisée par la méthode " OnAfficher " pour 
//	permettre de lire les en-têtes du bitmap et aussi vérifier
//	l'existence du fichier (reçu en paramêtre) et qu'il est bien un 
//	bitmap.
//
// En sortie de cette méthode les membres m_bmfh et m_bmih contiennent
//	 les informations
//
// Retourne 0 si le fichier est valide, 1 si on a une erreur, 
//	2 si ce n'est pas un BMP
//
// À noter une correction à m_bmih.biSizeImage si elle contient une 
//	valeur de 0. Certains logiciels ne fournissent pas cette valeur 
//	puisqu'elle peut être calculée avec les autres paramètres de 
//	l'en-tête. Par contre, pour le présent logiciel, cette valeur 
//	est utilisée pour l'écriture de l'image et doit être valide.
//
//------------------------------------------
int CP10App::LoadBitmapHeader(LPCTSTR lpszFileName, BITMAPFILEHEADER* bmfh, BITMAPINFOHEADER* bmih)
{
	CFile file;

	if (!file.Open(lpszFileName, CFile::modeRead))
		return 1;  	//Erreur d'ouverture

	//------------------------------------------
	// Lecture des en-têtes
	file.Read ((char*)bmfh,sizeof (BITMAPFILEHEADER));
	file.Read ((char*)bmih,sizeof (BITMAPINFOHEADER));

	file.Close();


	//------------------------------------------
	// Valide la signature du fichier
	if (bmfh->bfType!='MB') return 2;		// Le fichier n'est pas un BMP

	if (bmih->biSizeImage == 0)
	{
		bmih->biSizeImage=(DWORD)bmih->biWidth * bmih->biBitCount; // octets par lignes
		bmih->biSizeImage=(((bmih->biSizeImage) + 31) / 32) * 4;    // Alignement DWORD
		bmih->biSizeImage*=bmih->biHeight;                          // Espace requis pour l'ensemble de l'image (en octets)
	}

	return 0;
}


//-------------------------------------------------
//
// Fonction relié à la demande de prise d'image par la caméra. Une erreur est
// affichée sur la console s'il n'est pas possible de créer le fichier de demande.
//
// Notez que la fonction attendra indéfiniement pour obtenir l'image.
//
//-------------------------------------------------
void CP10App::getPicture()
{
	CFile file;

	// Création du fichier de demande
	if (!file.Open(CString("c:/TransImage/GetPict.txt"), CFile::modeCreate | CFile::modeWrite))
	{
		CP10View::GetView()->AfficherMessageConsole(CString("Erreur génération du fichier de demande"));
		return;
	}

	file.Close();

	// On affiche ce que l'on est en train de faire dans la boite des messages.
	CP10View::GetView()->AfficherMessageConsole(CString("Lecture de Image_Recu.bmp et Image_Orig.bmp"));

	/* Permet d'attendre sans compromettre les autres applications */

	while (!loadImage(CString("c:/TransImage/Image_Orig.bmp"), &CP10View::GetView()->m_Picture, &m_BMP_original, &m_CDC_original, &m_bmfh, &m_bmih)) 
	{ 
		MSG msg;
		( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) ;
	}

	if(M_Original_loaded)
		MVOps::deleteMatrix(M_Original, m_bmih.biHeight);
	M_Original = loadMatrix(m_bmih.biHeight, m_bmih.biWidth, &m_CDC_original);
	M_Original_loaded = true;


	while (!LireFilterAfficherImage(CString("c:/TransImage/Image_Recu.bmp"), &CP10View::GetView()->m_Picture)) 
	{ 
		MSG msg;
		( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) ;
	}

	// On efface le bitmap une fois qu'il est lu
	remove("c:/TransImage/Image_Recu.bmp");
	remove("c:/TransImage/Image_Orig.bmp");
}

//------------------------------------------
//	Fonction pour charger, filter une image et ensuite l'afficher dans un cadre
//	Entrées: Le path du fichier en String
//	Retourne: un booléen relié au succès du chargement de l'image
//	Auteur: Maxime Lussier
//------------------------------------------
bool CP10App::LireFilterAfficherImage(LPCTSTR lpszFileName, CStatic* ImageSquare){

	if(!loadImage(lpszFileName, &CP10View::GetView()->m_Picture, &m_BMP_bruite, &m_CDC_bruite, &m_bmfh, &m_bmih))
		return false;

	loadImage(lpszFileName, &CP10View::GetView()->m_Picture, &m_BMP_filtered, &m_CDC_filtered, &m_bmfh, &m_bmih);

	int M = m_bmih.biHeight;
	int N = m_bmih.biWidth;

	if(M_Camera_loaded){
		MVOps::deleteMatrix(M_Camera, M);
	}
	CP10View::GetView()->AfficherMessageConsole(CString("Chargement de l'image en matrice."));
	M_Camera = loadMatrix(M,N, &m_CDC_bruite);
	M_Camera_loaded = true;

	CString temp;
	if(M_Camera_loaded && M_Original_loaded){ 
		// on afficher le psnr en ignorant les dernières 20 lignes et colonnes pour ignorer les barres ajoutées par les filtres
		temp.Format("%f", traitementImages::PSNR(M_Camera, M_Original, m_bmih.biHeight-20,  m_bmih.biWidth-20));
		CP10View::GetView()->PSNR_Avant_Control.SetWindowText(temp);
	}

	DisplayImage(ImageSquare, &m_BMP_bruite, &m_CDC_bruite);

	CP10View::GetView()->AfficherMessageConsole(CString("Filtrage du bruit aléatoire."));
	traitementImages::selEtPoivre(M_Camera, M, N);
	CP10View::GetView()->AfficherMessageConsole(CString("Filtrage de l'interférence."));
	double** Y1 = traitementImages::interference(M_Camera, M, N);
	CP10View::GetView()->AfficherMessageConsole(CString("Filtrage de flou."));
	double** Y2 = traitementImages::filtrerFlou(Y1, M, N);
	MVOps::deleteMatrix(Y1, M);


	if(!M_ref_loaded){
		CP10View::GetView()->AfficherMessageConsole(CString("Pas de référence de chargée donc corrélation impossible."));
		CP10View::GetView()->AfficherMessageConsole(CString("Veuillez fournir c:/TransImage/Reference.bmp ."));
		CP10View::GetView()->AfficherMessageConsole(CString("Mise à jour de l'image."));
		updateFromMatrix(Y2, M, N, &m_CDC_filtered);
	} else if (!M_target_loaded){
		CP10View::GetView()->AfficherMessageConsole(CString("Pas de cible de chargée donc corrélation impossible."));
		CP10View::GetView()->AfficherMessageConsole(CString("Veuillez en charger une et appuyez sur acquisition d'image."));
		CP10View::GetView()->AfficherMessageConsole(CString("Mise à jour de l'image."));
		updateFromMatrix(Y2, M, N, &m_CDC_filtered);
	} else {
		CP10View::GetView()->AfficherMessageConsole(CString("Détection de la destination."));
		double** CO = traitementImages::detectImage(Y2, M, N, M_target, m_bmih_target.biHeight,  m_bmih_target.biWidth, M_ref, m_bmih_ref.biHeight,  m_bmih_ref.biWidth);


		temp.Format("%f pixels en X, %f pixels en Y", CO[3][0], CO[3][1]);
		CP10View::GetView()->destinationControl.SetWindowText(temp);
		temp.Format("%f m en X, %f m en Y, %f pixels/m", CO[4][0]+1, CO[4][1]+0.1, CO[5][0]);
		CP10View::GetView()->destination2Control.SetWindowText(temp);

		if((CO[4][0] < 0 || CO[4][1] < 0 || CO[4][0]+1 > 2.2 || CO[4][1]+0.1 > 1) && tries < 3){
			tries++;
			CP10View::GetView()->AfficherMessageConsole(CString("Échec de la détection de la destination, on essaie de nouveau."));
			getPicture();
			return true;
		} else if (tries == 3) {
			CO[4][0] = 1.7-1;
			CO[4][1] = 0.2-0.1;
		}

		tries = 0;
		// Configure Trajectory
		long TrajLength = 0;

		float** gTraj = GenTrajP10(CO[4][0]+1, CO[4][1]+0.1, 0.4, &TrajLength);
		int res = getCraneInstance()->SetTrajBuffer(gTraj, TrajLength);
		if(res != 0)
			messages(res);


		// add To CommandStack (GRUN)
		addCommandToStack(GRUN);

		updateFromMatrix(Y2, M, N, &m_CDC_filtered);
	


		// Reconnaissance de l'image
		// PSNR après/avant traitement


		// ajouter pointeurs couleur
		CP10View::GetView()->AfficherMessageConsole(CString("Affichage des pointeurs."));
		pointeurCouleur(RED, CO[0][1], CO[0][0], 5, &m_CDC_filtered);
		pointeurCouleur(RED, CO[1][1], CO[1][0], 5, &m_CDC_filtered);
		pointeurCouleur(RED, CO[2][1], CO[2][0], 5, &m_CDC_filtered);
		pointeurCouleur(BLUE, CO[3][1], CO[3][0], 5, &m_CDC_filtered);

		MVOps::deleteMatrix(CO, 6);
	}
	
	if(M_Camera_loaded && M_Original_loaded){
		// on afficher le psnr en ignorant les dernières 20 lignes et colonnes pour ignorer les barres ajoutées par les filtres
		temp.Format("%f", traitementImages::PSNR(Y2, M_Original, m_bmih.biHeight-20,  m_bmih.biWidth-20));
		CP10View::GetView()->PSNR_Apres_Control.SetWindowText(temp);
	}
	MVOps::deleteMatrix(Y2, M);
	DisplayImage(ImageSquare, &m_BMP_filtered, &m_CDC_filtered);
	return true;
}


//-------------------------------------------------
// lire une image selon le nom de fichier demandé. 
BOOL CP10App::loadImage(LPCTSTR lpszFileName, CStatic* ImageSquare, CBitmap* BMP, CDC* CDC, BITMAPFILEHEADER* bmfh, BITMAPINFOHEADER* bmih)
{
	HBITMAP BitmapHandle;


	if (LoadBitmapHeader(lpszFileName, bmfh, bmih) != 0)
	{
		return FALSE;   // Retourne FALSE si l'image n'est pas lue
	}

	CDC->DeleteDC();			// Efface l'ancien contexte du bitmap

	CDC->CreateCompatibleDC(ImageSquare->GetDC());   // Crée un nouvel espace pour la nouvelle image

	//------------------------------------------
	// Cette fonction de la librairie standard lie directement le bitmap d'un
	//	fichier et retourne une référence sur l'image en mémoire si il y a succès
	//------------------------------------------
	CRect rect;
	ImageSquare->GetClientRect(&rect);

	//BitmapHandle = (HBITMAP)::LoadImage(NULL,LPCTSTR(lpszFileName),IMAGE_BITMAP,rect.Width(),rect.Height(),LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	BitmapHandle = (HBITMAP)::LoadImage(NULL,LPCTSTR(lpszFileName),IMAGE_BITMAP,0 ,0,LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (BitmapHandle !=0)  					// Non NULL si la lecture a réussie
	{
		BMP->Detach();					// Efface l'ancien bitmap
		BMP->Attach(BitmapHandle);		// Conserve la nouvelle référence
		CDC->SelectObject(BMP); // Assigne l'image au rectangle
	}

	return TRUE;  // retourne TRUE si la lecture est un succès
}

//------------------------------------------
//	Fonction pour charger une image dans une matrice
//	Entrées: les tailles en Y et en X de l'image, Le CDC de l'image
//	Retourne: un array contenant l'image en valeurs de 0-255
//	Auteur: Maxime Lussier
//------------------------------------------
double** CP10App::loadMatrix(int M, int N, CDC* m_cdc){
	double** Y = MVOps::newMatrix(M, N);

	for (int j=0; j < N;j++){
		for (int i = 0 ; i < M;i++){
			// Lecture d'un pixel		
			Y[i][j] = GetBValue(m_cdc->GetPixel(j,i)); // Les RGB sont égaux (merci!!!)
		}	
	}
	return Y;
}

//------------------------------------------
//	Fonction pour charger une matrice dans une image
//	Entrées: la matrice, les tailles en Y et en X de l'image, Le CDC de l'image
//	Retourne: rien
//	Auteur: Maxime Lussier
//------------------------------------------
void CP10App::updateFromMatrix(double** X, int M, int N, CDC* m_cdc){
	COLORREF pixel;			// Pour permettre l'écriture d'un pixel 
	int grey = 0;				

	for (int j = 0; j < N; j++){
		for (int i = 0; i <M; i++){		
			// Modification avec protection
			grey = ValidePixel(X[i][j]);

			// Écriture d'un pixel
			m_cdc->SetPixel(j,i,RGB(grey,grey,grey));
		}
	}
}

//------------------------------------------
//
// Fonction qui s'assurer que les couleurs sont conservé sur un octet même
// 	si les calculs s'effectue avec un " int "
//
//------------------------------------------

int CP10App::ValidePixel(int x)
{
	if (x > 255) return 255;
	if (x < 0) return 0;
	return x;
}

//------------------------------------------
// Fonction d'affichade de l'image
//	Entrées: le cadre de l'image, le CBitmap, le cdc de l'image
//	Retourne: rien
//	Auteur: 
//------------------------------------------
void CP10App::DisplayImage(CStatic* ImageSquare, CBitmap* bmp, CDC* cdc) 
{
	//-------------------------------------------------
	// Ajout d'une fonction d'affichage de l'image
	BITMAP bm;
	bmp->GetBitmap(&bm);
	CRect rect;
	ImageSquare->GetClientRect(&rect);
	CDC *DC = ImageSquare->GetDC();
	// On redimensionne l'image en l'affichant.
	DC->StretchBlt(0,0,rect.Width(),rect.Height(),cdc,0,0,bm.bmWidth,bm.bmHeight ,SRCCOPY);


	//------------------------------------------------


}

//------------------------------------------
//	Fonction pour suvegarder l'image filtrée en bmp
//	Entrées: rien
//	Retourne: rien
//------------------------------------------
void CP10App::sauvegarde() 
{
	CString OpenFilter, ActualName;

	//------------------------------------------
	// Création d'un filtre appel et création du dialogue. 

	OpenFilter = "Bit map (*.bmp)|*.bmp||";

	if (ActualName.Right(1) == "\\")
		ActualName = ActualName + "*.bmp";

	if (ActualName.Right(1) == ":")
		ActualName = ActualName + "\\*.bmp";


	CFileDialog FileOpenDialog(
		FALSE, 					// Ceci demande l'utilisation d'un 
		//	dialogue d'écriture (notez le
		//	OFN_OVERWRITEPROMPT plus loin)

		NULL,
		ActualName,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,
		OpenFilter,   
		AfxGetMainWnd());  

	if(FileOpenDialog.DoModal()!=IDCANCEL)
	{
		CFile file;
		BITMAPINFO bi;
		BYTE *buffer;

		if (!file.Open(FileOpenDialog.GetPathName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			return; 		//Erreur d'ouverture du fichier


		//------------------------------------------
		// Évaluation de la dimension du tableau des assignations des couleurs des
		//	" pixels " de l'image. Notez le cas	particulier des couleurs 24 bits. 

		bi.bmiHeader = m_bmih;

		int biColorSize=0;

		if (m_bmih.biBitCount!=24) biColorSize=(1<<m_bmih.biBitCount);

		biColorSize*=sizeof(RGBQUAD);

		//------------------------------------------
		// Création d'un tampon pour recevoir l'image. 
		//
		// Ce tampon va aussi contenir le deuxième en-tête.

		buffer = new BYTE[m_bmfh.bfSize];

		//------------------------------------------
		// Ajout du deuxième en-tête au début du tampon.


		*(BITMAPINFOHEADER *)buffer = m_bmih;

		GetDIBits(m_CDC_filtered,(HBITMAP)m_BMP_filtered.GetSafeHandle(),0L,
			m_bmih.biHeight,

			// Ici on a une astuce pour permettre de lire
			// 	l'image et de conserver les positions des 
			//	octets ainsi que l'information sur les 
			//	couleurs en même temps.
			//
			// L'écriture de l'image va commencer après
			// 	l'information d'en-tête et l'espace 
			//	réservée pour la table des couleurs.

			(LPSTR)buffer+m_bmih.biSize + biColorSize, 

			// Le début du tampon contient l'en-tête de 
			//	structure BITMAPINFOHEADER. Cet 
			//	information est un élément de la 
			//	structure BITMAPINFO qui ajoute le 
			//	tableau des couleurs à utiliser pour 
			//	l'affichage de l'image. Cette section
			//	sera réécite par " GetDIBits " qui va 
			//	permettre d'inscrire les informations 
			//	dans notre fichier.

			(BITMAPINFO *)buffer,		

			(DWORD) DIB_RGB_COLORS);

		//------------------------------------------
		// Écriture du premier en-tête (qui est le même que l'image d'origine)
		//	ainsi que le tampons contenant le deuxième en-tête, le tableau des couleurs
		// 	et l'image.

		file.Write ((char*)&m_bmfh,sizeof (BITMAPFILEHEADER));

		// Dans la situation présente, le buffer contient aussi la palette (voir (LPSTR) dans le GetDIBIts)

		file.Write (buffer,m_bmfh.bfSize);

		file.Close();		// Fermeture du fichier

		delete buffer;		// Destruction du tampon

	}
	else
		AfxMessageBox("Échec : " + ActualName);

}

//------------------------------------------
//	Fonction pour charger l'image de la cible cherchée
//	Entrées: le path de la target
//	Retourne: rien
//	Auteur: Maxime Lussier
//------------------------------------------
void CP10App::ChargerCible(char* targetPath){
	//CPaintDC cdc(this);
	HBITMAP BitmapHandle;

	if(M_target_loaded)
		MVOps::deleteMatrix(M_target,m_bmih_target.biHeight);

	if (LoadBitmapHeader(targetPath, &m_bmfh_target, &m_bmih_target) != 0)
	{
		CP10View::GetView()->AfficherMessageConsole(CString("Erreur d'ouverture de la cible"));
		return;
	}

	m_CDC_target.DeleteDC();			// Efface l'ancien contexte du bitmap

	m_CDC_target.CreateCompatibleDC(CP10View::GetView()->targetImageControl.GetDC());   // Crée un nouvel espace 
	//   pour la nouvelle image

	//------------------------------------------
	// Cette fonction de la librairie standard lie directement le bitmap d'un
	//	fichier et retourne une référence sur l'image en mémoire si il y a succès
	//------------------------------------------
	CRect rect;
	CP10View::GetView()->targetImageControl.GetClientRect(&rect);
	BitmapHandle = (HBITMAP)::LoadImage(NULL,LPCTSTR(targetPath),IMAGE_BITMAP,0,0,LR_LOADFROMFILE | LR_CREATEDIBSECTION);



	if (BitmapHandle !=0)  					// Non NULL si la lecture a réussie
	{
		m_BMP_target.Detach();					// Efface l'ancien bitmap
		m_BMP_target.Attach(BitmapHandle);		// Conserve la nouvelle référence
		m_CDC_target.SelectObject(m_BMP_target); // Assigne l'image au rectangle
	}

	BITMAP bm;
	m_BMP_target.GetBitmap(&bm);
	CDC *DC = CP10View::GetView()->targetImageControl.GetDC();
	// On redimensionne l'image en l'affichant.
	DC->StretchBlt(0,0,rect.Width(),rect.Height(),&m_CDC_target,0,0,bm.bmWidth,bm.bmHeight ,SRCCOPY);

	M_target = loadMatrix(m_bmih_target.biHeight, m_bmih_target.biWidth, &m_CDC_target);
	M_target_loaded = true;
}


//------------------------------------------
//	Fonction pour charger l'image des références
//	Entrées: rien
//	Retourne: rien
//	Auteur: Maxime Lussier
//------------------------------------------
void CP10App::ChargerRef(){
	//CPaintDC cdc(this);
	HBITMAP BitmapHandle;
	CString path("c:/TransImage/Reference.bmp");

	if (LoadBitmapHeader(path, &m_bmfh_ref, &m_bmih_ref) != 0)
	{
		CP10View::GetView()->AfficherMessageConsole(CString("Erreur d'ouverture de la référence"));
		return;
	}

	m_CDC_ref.DeleteDC();			// Efface l'ancien contexte du bitmap

	m_CDC_ref.CreateCompatibleDC(CP10View::GetView()->targetImageControl.GetDC());   // Crée un nouvel espace 
	//   pour la nouvelle image

	//------------------------------------------
	// Cette fonction de la librairie standard lie directement le bitmap d'un
	//	fichier et retourne une référence sur l'image en mémoire si il y a succès
	//------------------------------------------
	CRect rect;
	CP10View::GetView()->targetImageControl.GetClientRect(&rect);
	BitmapHandle = (HBITMAP)::LoadImage(NULL,LPCTSTR(path),IMAGE_BITMAP,0 ,0,LR_LOADFROMFILE | LR_CREATEDIBSECTION);



	if (BitmapHandle !=0)  					// Non NULL si la lecture a réussie
	{
		m_BMP_ref.Detach();					// Efface l'ancien bitmap
		m_BMP_ref.Attach(BitmapHandle);		// Conserve la nouvelle référence
		m_CDC_ref.SelectObject(m_BMP_ref); // Assigne l'image au rectangle
	}

	//BITMAP bm;
	//m_BMP_target.GetBitmap(&bm);
	//CDC *DC = CP10View::GetView()->targetImageControl.GetDC();
	//// On redimensionne l'image en l'affichant.
	//DC->StretchBlt(0,0,rect.Width(),rect.Height(),&m_CDC_target,0,0,bm.bmWidth,bm.bmHeight ,SRCCOPY);

	if(M_ref_loaded)
		MVOps::deleteMatrix(M_ref,m_bmih_ref.biHeight);
	M_ref = loadMatrix(m_bmih_ref.biHeight, m_bmih_ref.biWidth, &m_CDC_ref);
	M_ref_loaded = true;
}

//------------------------------------------
//	Fonction ajouter des cercles de couleur dans l'image
//	Entrées: la couleur, la position M, N, le rayon du cercle et le cdc de l'image
//	Retourne: rien
//	Auteur: Maxime Lussier
//------------------------------------------
void CP10App::pointeurCouleur(int couleur, int M, int N, int size, CDC* m_cdc){

	int R, G, B;
	switch(couleur){
	case PINK:
		R = 255;
		G = 105;
		B = 180;
		break;
	case RED:
		R = 255;
		G = 0;
		B = 0;
		break;
	case BLUE:
		R = 0;
		G = 0;
		B = 255;
		break;
	case GREEN:
		R = 0;
		G = 255;
		B = 0;
		break;
	case UGLYGREEN:
		R = 124;
		G = 252;
		B = 0;
		break;
	default:
		R = 10;
		G = 205;
		B = 46;
		break;
	}
	COLORREF pixel;			// Pour permettre l'écriture d'un pixel 
	double t = 0;
	while(t < 2*PI)
	{
		t = t+(1/((double)size))*2;
		int x = size*cos(t);
		int y = size*sin(t);
		m_cdc->SetPixel(N+x,M+y,RGB(R,G,B));
		m_cdc->SetPixel(N+x+1,M+y-1,RGB(R,G,B));
		m_cdc->SetPixel(N+x-1,M+y+1,RGB(R,G,B));
		m_cdc->SetPixel(N+x+1,M+y+1,RGB(R,G,B));
		m_cdc->SetPixel(N+x-1,M+y-1,RGB(R,G,B));
	}
}

//------------------------------------------
//Auteur : matlab -> Vincent Lapointe et Benoit Raybaud
//         c++    -> Maxime Lussier
//Date : 20 juillet 2011
//Entrées : Point de dépot ( X, Y, Z)
//Sorties : vecteurs Yt=[ty,y] Xt=[tx,x] Zt=[tz,z] 
//------------------------------------------
float** CP10App::GenTrajP10(double X3, double Y3, double Z3, long* TrajLength)
{
	// Détermination des temps minimaux
	double posInit[3] = {1, 1.9, 0.8};
	double wayPoint[3] = {1.5, 1.5, 0.3};
	double vmax=0.15;
	double vmaxZ=0.1;

	double tmin1 = (posInit[1]-1.5)/vmax;
	double tmin2 = (posInit[1]-Y3)/vmax;
	double ti=0;
	double t2 = ((0.7-0.2)/vmaxZ);
	double t3 = ((0.7-Z3)/vmaxZ)+t2;


	//--------------------------------  Z   -------------------------------


	// Génération de la position en Z en fonction du temps

	double tz[5] = {ti, t2, t3, ti, t3};
	double Z[5] = {posInit[2], wayPoint[2], Z3, 0, 0};

	int M_z=5;
	double dt_z=0.001;
	int IntZLength = floor((abs((double)t3))/((double)dt_z)) + 1;
	double* approx_z = MVOps::zerosD(IntZLength);
	// double* approx_vz = MVOps::zerosD(IntZLength);
	// double* approx_az = MVOps::zerosD(IntZLength);
	double* Interval_z = new double[IntZLength];
	for(int i = 0; i < IntZLength; i++){
		Interval_z[i] = dt_z*i;
	}
	int N_z = 4;

	double** P_z = new double*[M_z];
	for(int i = 0; i < M_z; i++){
		P_z[i] = new double[N_z+1];
		for(int j = 0; j < N_z+1; j++)
			P_z[i][j] = pow(tz[i], j);
	}

	for(int j = 0; j < N_z; j++){
		P_z[M_z-2][0]=0;
		P_z[M_z-2][j+1]=(j+1)*pow(tz[3],j);
		P_z[M_z-1][0]=0;
		P_z[M_z-1][j+1]=(j+1)*pow(tz[4],j);
	}

	double** P_z_i = MVOps::matrixInversion(P_z, M_z);
	double** A_z = MVOps::matrixProduct(P_z_i, M_z, M_z, Z, M_z);

	for(int i = 0; i < M_z; i++)  // C'est comme ca qu'on efface une matrice.
		delete [] P_z_i[i];       //
	delete [] P_z_i;              //

	for(int i = 0; i < M_z; i++){
		double* powIzi = MVOps::vectorPow(Interval_z,IntZLength, i);
		double* prodAzPIZi = MVOps::vectorProduct(powIzi, A_z[i][0], IntZLength);
		delete [] powIzi;
		double* temp = MVOps::vectorAdd(approx_z, prodAzPIZi, IntZLength);
		delete [] prodAzPIZi;
		delete [] approx_z;
		approx_z = temp;
	}

	// Vitesse en Z
	/*
	for(int i = 0; i < M_z-1; i++){
	double* powIzi = MVOps::vectorPow(Interval_z,IntZLength, i);
	double* prodAzPIZi = MVOps::vectorProduct(powIzi, ((i+1)*(A_z[i+1][0])), IntZLength);
	delete [] powIzi;
	double* temp = MVOps::vectorAdd(approx_vz, prodAzPIZi, IntZLength);
	delete [] prodAzPIZi;
	delete [] approx_vz;
	approx_vz = temp;
	}
	*/

	// Accélération en Z
	/*
	for(int i = 2; i <= M_z-1; i++){
	double* powIzi = MVOps::vectorPow(Interval_z,IntZLength, i-2);
	double* prodAzPIZi = MVOps::vectorProduct(powIzi, ((i)*(i-1)*(A_z[i][0])), IntZLength);
	delete [] powIzi;
	double* temp = MVOps::vectorAdd(approx_az, prodAzPIZi, IntZLength);
	delete [] prodAzPIZi;
	delete [] approx_az;
	approx_az = temp;
	}
	*/

	// Sortie Z

	// Free Memory
	delete [] Interval_z;
	MVOps::deleteMatrix(A_z, M_z);
	MVOps::deleteMatrix(P_z, M_z);

	// delete [] approx_vz;
	// delete [] approx_az;

	//---------------------------   Y   -----------------------------------

	// Génération de y en fonction du temps pour la première section
	int M1 = 3; // size de Y1
	double t1[3] = {0, tmin1, 0};
	double Y1[3] = {posInit[1], wayPoint[1], 0};
	double dt = 0.001;
	int IntT1Length = floor(abs(tmin1)/dt) + 1;
	double* approx_t1 = MVOps::zerosD(IntT1Length);
	double* Interval_t1 = new double[IntT1Length];
	for(int i = 0; i < IntT1Length; i++){
		Interval_t1[i] = dt*i;
	}
	int Nt1=2;


	double** Pt1 = new double*[M1];
	for(int i = 0; i < M1; i++){
		Pt1[i] = new double[Nt1+1];
		for(int j = 0; j < Nt1+1; j++)
			Pt1[i][j] = pow(t1[i], j);
	}

	for(int i = 1; i <= Nt1; i++){
		Pt1[M1-1][0]=0;
		Pt1[M1-1][i]=(i)*pow(t1[0],(i-1));
	}

	double** Pt1I = MVOps::matrixInversion(Pt1, M1);
	double** At1 = MVOps::matrixProduct(Pt1I, M1, M1, Y1, M1);

	MVOps::deleteMatrix(Pt1, M1);
	MVOps::deleteMatrix(Pt1I, M1);

	for(int i = 0; i < M1;i++){
		double* powIt1i = MVOps::vectorPow(Interval_t1,IntT1Length, i);
		double* prodAzPITi = MVOps::vectorProduct(powIt1i, At1[i][0], IntT1Length);
		delete [] powIt1i;
		double* temp = MVOps::vectorAdd(approx_t1, prodAzPITi, IntT1Length);
		delete [] prodAzPITi;
		delete [] approx_t1;
		approx_t1 = temp;
	}


	// Génération de y en fonction du temps pour la deuxième section

	double vit = At1[1][0]+2*At1[2][0]*Interval_t1[IntT1Length-1];
	//At1(2)+2*At1(3)*Interval_t1(end)

	MVOps::deleteMatrix(At1, M1);

	int M2=4; // size of Y2
	double t2d[4] = {tmin1, tmin2, tmin1, tmin2};
	double Y2[4] = {wayPoint[1], Y3, vit, 0};
	int IntT2Length = floor(abs(tmin2-tmin1)/dt) + 1;
	double* approx_t2 = MVOps::zerosD(IntT2Length);
	double* Interval_t2 = new double[IntT2Length];
	for(int i = 0; i < IntT2Length; i++){
		Interval_t2[i] = dt*i + tmin1;
	}

	int Nt2=3;

	double** Pt2 = new double*[M2];
	for(int i = 0; i < M2; i++){
		Pt2[i] = new double[Nt2+1];
		for(int j = 0; j < Nt2+1; j++)
			Pt2[i][j] = pow(t2d[i], (j));
	}

	for(int i = 1; i <= Nt2; i++){
		Pt2[M2-2][0]=0;
		Pt2[M2-2][i]=(i)*pow(t2d[2],(i-1));
		Pt2[M2-1][0]=0;
		Pt2[M2-1][i]=(i)*pow(t2d[3],(i-1));
	}

	double** Pt2I = MVOps::matrixInversion(Pt2, M2);
	double** At2 = MVOps::matrixProduct(Pt2I, M2, M2, Y2, M2);

	MVOps::deleteMatrix(Pt2, M2);
	MVOps::deleteMatrix(Pt2I, M2);

	for(int i = 0; i < M2; i++){
		double* powIt2i = MVOps::vectorPow(Interval_t2,IntT2Length, i);
		double* prodAzPITi = MVOps::vectorProduct(powIt2i, At2[i][0], IntT2Length);
		delete powIt2i;
		double* temp = MVOps::vectorAdd(approx_t2, prodAzPITi, IntT2Length);
		delete prodAzPITi;
		delete approx_t2;
		approx_t2 = temp;
	}

	MVOps::deleteMatrix(At2, M2);   

	// Création des vecteurs limitant la vitesse en Y

	double* Interval_d1 = approx_t1;  
	double* Interval_d2 = approx_t2;

	// Interval_y=[Interval_t1,Interval_t2]';
	double* Yt = MVOps::appendVectors(approx_t1, IntT1Length-1,approx_t2, IntT2Length);


	// Sortie Yt

	// Yt=[Interval_y,Y]


	//---------------------------TRAJECTOIRE-------------------------------


	// Déclaration des variable et vecteurs pour génération de la trajectoire.

	double X_d1[3] = {posInit[0], wayPoint[0], 0}; 
	double Y_d1[3] = {posInit[1], wayPoint[0], wayPoint[0]};  
	double X_d2[5] = {wayPoint[0], wayPoint[0], X3, 0, 0};
	double Y_d2[5] = {wayPoint[1], 1.4, Y3, wayPoint[1], Y3};
	int M_d1 = 3;
	int M_d2 = 5;
	double* approx_d1 = MVOps::zerosD(IntT1Length);
	double* approx_d2 = MVOps::zerosD(IntT2Length);
	int N_d1 = 2;
	int N_d2 = 4;

	// Génération de la trajectoire en deux partie

	double** P_d1 = new double*[M_d1];
	for(int i = 0; i < M_d1; i++){
		P_d1[i] = new double[N_d1+1];
		for(int j = 0; j < N_d1+1; j++)
			P_d1[i][j]=pow(Y_d1[i], j);
	}

	double** P_d2 = new double*[M_d2];
	for(int i = 0; i < M_d2; i++){
		P_d2[i] = new double[N_d2+1];
		for(int j = 0; j < N_d2+1; j++)
			P_d2[i][j]=pow(Y_d2[i], j);
	}

	for(int j = 0; j < N_d1; j++){
		P_d1[M_d1-1][0]=0;
		P_d1[M_d1-1][j+1]=(j+1)*pow(Y_d1[2], j);
	}

	for(int j = 0; j < N_d2; j++){
		P_d2[M_d2-2][0]=0;
		P_d2[M_d2-2][j+1]=(j+1)*pow(Y_d2[2], j);
		P_d2[M_d2-1][0]=0;
		P_d2[M_d2-1][j+1]=(j+1)*pow(Y_d2[3], j);
	}  

	double** P_d1_i = MVOps::matrixInversion(P_d1, M_d1);
	double** A_d1 = MVOps::matrixProduct(P_d1_i, M_d1, M_d1, X_d1, M_d1);

	MVOps::deleteMatrix(P_d1, M_d1);
	MVOps::deleteMatrix(P_d1_i, M_d1);

	double** P_d2_i = MVOps::matrixInversion(P_d2, M_d2);
	double** A_d2 = MVOps::matrixProduct(P_d2_i, M_d2, M_d2, X_d2, M_d2);

	MVOps::deleteMatrix(P_d2_i, M_d2);
	MVOps::deleteMatrix(P_d2, M_d2);

	for(int j = 0; j < M_d1; j++){
		double* powId1i = MVOps::vectorPow(Interval_d1,IntT1Length, j);
		double* prodAzPITi = MVOps::vectorProduct(powId1i, A_d1[j][0], IntT1Length);
		delete [] powId1i;
		double* temp = MVOps::vectorAdd(approx_d1, prodAzPITi, IntT1Length);
		delete [] prodAzPITi;
		delete [] approx_d1;
		approx_d1 = temp;
	}

	for(int j = 0; j < M_d2; j++){
		double* powId2i = MVOps::vectorPow(Interval_d2,IntT2Length, j);
		double* prodAzPITi = MVOps::vectorProduct(powId2i, A_d2[j][0], IntT2Length);
		delete [] powId2i;
		double* temp = MVOps::vectorAdd(approx_d2, prodAzPITi, IntT2Length);
		delete [] prodAzPITi;
		delete [] approx_d2;
		approx_d2 = temp;
	}

	MVOps::deleteMatrix(A_d1, M_d1);
	MVOps::deleteMatrix(A_d2, M_d2);

	double* Interval_x= MVOps::appendVectors(Interval_t1, IntT1Length-1, Interval_t2, IntT2Length);
	double* approx_x = MVOps::appendVectors(approx_d1, IntT1Length-1, approx_d2, IntT2Length);

	delete [] approx_t1;
	delete [] approx_t2;

	// Sortie Xt

	//Xt=[Interval_x,approx_x];

	// Trajectoire générée

	//     genTrajP10=[approx_d1 approx_d2];
	// Free Up Memory
	delete [] approx_d1;
	delete [] approx_d2;
	delete [] Interval_x;
	delete [] Interval_t1;
	delete [] Interval_t2;

	*TrajLength = IntT1Length+IntT2Length-1;

	float** Out = new float*[3];
	for(int i = 0; i < 3; i++){
		Out[i] = new float[*TrajLength];
		for(int j = 0; j < *TrajLength; j++){
			switch(i){
			case 0:
				Out[i][j] = approx_x[j];
				break;
			case 1:
				Out[i][j] = Yt[j];
				break;
			case 2:
				if(j < IntZLength){
					Out[i][j] = approx_z[j];
				} else {
					Out[i][j] = approx_z[IntZLength-1];
				}
				break;
			}
		}
	}
	delete [] Yt;
	delete [] approx_x;
	delete [] approx_z;


	//displayMatrix(Out, IntT1Length+IntT2Length, 3);
	return Out;
}

//------------------------------------------
//	Fonction pour ajouter une commande au stack d'exécution
//	Entrées: l'ID de la commande
//	Retourne: rien
//	Auteur: Maxime Lussier
//------------------------------------------
void CP10App::addCommandToStack(int command){
	if(currentCommand < 1)
		currentCommand = 1;
	if(currentCommand < 9){
		commandStack[currentCommand] = command;
		currentCommand++;
	} else {
		CP10View::GetView()->AfficherMessageConsole(CString("Pile de commandes pleine, veuillez attendre."));
	}
}

//------------------------------------------
//	Fonction pour exécuter le stack d'exécution
//	Entrées: les tailles en Y et en X de l'image, Le CDC de l'image
//	Retourne: un array contenant l'image en valeurs de 0-255
//	Auteur: Maxime Lussier
//------------------------------------------
void CP10App::executeStack(){
	int status = getCraneInstance()->GetCommand();
	int res, mode;
	if(currentCommand == 1)
		return;
	switch(commandStack[1]){

	case 0:
		break;
	case GSTOP:
		res = getCraneInstance()->Stop(); // On va chercher l'instance de la classe grue pour appeler stop
		if(res == 0) // On affiche le message de retour quel qu'il soit
			CP10View::GetView()->AfficherMessageConsole(CString("La fonction arrêt instantané a été exécutée avec succès."));
		else
			messages(res);
		updateStack();
		break;
	case GRAZ:
		if(!executed){
			CP10View::GetView()->StartButtonControl.EnableWindow(FALSE);
			getCraneInstance()->Reset();  // On va chercher l'instance de la classe grue pour appeler reset
			CP10View::GetView()->AfficherMessageConsole(CString("En attente d'arrêt.")); // On affiche le message
			executed = true;
		} else if(executed && status == STOP) {
			CP10View::GetView()->AfficherMessageConsole(CString("Grue initialisée à sa position d’origine.")); // On affiche le message
			updateStack();
		}
		break;
	case GRUN:
		mode = RESTART;
		if(!compLoaded){
			clearStack();
			CP10View::GetView()->AfficherMessageConsole(CString("Les paramètres des compensateurs ne sont pas chargés."));
			break;
		}
		if(status == STOP && !executed){
			executed = true;
			getCraneInstance()->SetTrajMode(HOLD);
			if(commandStack[0] == GSTOP)
				mode = CONTINUE;
			res = getCraneInstance()->Run(CLOSE, mode); // On va chercher l'instance de la classe grue pour appeler Run
			if(res == 0) // On affiche le message de retour quel qu'il soit
				CP10View::GetView()->AfficherMessageConsole(CString("La trajectoire a été démarré avec succès"));
			else
				messages(res);
		}
		if(isAtDestination() && isStable() && executed){
			CP10View::GetView()->SetTimer(2, 50, NULL);
			getCraneInstance()->Stop();
			updateStack();
		} else {
			CP10View::GetView()->SetTimer(2, 1000, NULL); // on vérifie au secondes si la grue est stable
		}
		break;
	case PICTURE:
		CP10View::GetView()->StartButtonControl.EnableWindow(FALSE);
		CP10View::GetView()->AfficherMessageConsole(CString("On prends une photo."));
		getPicture(); // enlever si pas de photo
		updateStack();
		break;
	default:
		break;
	}


}
void CP10App::clearStack(){
	currentCommand = 1;
	for(int i = 1; i < 9; i++){
		commandStack[i] = 0;
	}
	executed = false;
}

void CP10App::updateStack(){
	for(int i = 1; i < currentCommand; i++){
		commandStack[i-1] = commandStack[i];
	}
	if(currentCommand > 1)
		currentCommand--;
	executed = false;
}

bool CP10App::isAtDestination(){
	int i;
	long size;
	float seuil = 0.05;
	float  sen[5], meansen[5], *traj[3], cart[3];
	double	endxc = 1.0, endyc = 1.0, endzc = 1.0;
	// Initialiser les tableaux
	for(i = 0 ; i < 5; i++) {
		sen[i] = 0.0;
		meansen[i] = 0.0;
	}

	int res = crane->GetTrajBuffer(traj, &size);
	if(res==0)
	{
		endxc = (double)traj[0][size-1];
		endyc = (double)traj[1][size-1];
		endzc = (double)traj[2][size-1];
	}

	res = getCraneInstance()->GetSensor(sen);
	if(res != 0){ 
		theApp.messages(res);
		return false;
	}
	CP10View::GetView()->positionCharge(sen, cart);

	if(abs(cart[0]-endxc) < seuil
		&& abs(cart[1]-endyc) < seuil
		&& abs(cart[2]-endzc) < seuil){
			CP10View::GetView()->AfficherMessageConsole(CString("Arrivée à destination"));
			return true;
	}
	return false;
}


bool CP10App::isStable(){
	double seuil = 0.005;
	float  sen[5], cart1[3], cart2[3];
	double	endxc = 1.0, endyc = 1.0, endzc = 1.0;

	// Initialiser les tableaux
	for(int i = 0 ; i < 5; i++) {
		sen[i] = 0.0;
	}

	int res = getCraneInstance()->GetSensor(sen);
	if(res != 0){ 
		theApp.messages(res);
		return false;
	}
	CP10View::GetView()->positionCharge(sen, cart1);

	::Sleep(100);

	res = getCraneInstance()->GetSensor(sen);
	if(res != 0){ 
		theApp.messages(res);
		return false;
	}
	CP10View::GetView()->positionCharge(sen, cart2);

	if(abs(cart1[0]-cart2[0]) < seuil
		&& abs(cart1[1]-cart2[1]) < seuil
		&& abs(cart1[2]-cart2[2]) < seuil){
			CP10View::GetView()->AfficherMessageConsole(CString("Grue Stabilisée"));
			return true;
	}
	return false;
}


void CP10App::messages(int id){
	switch(id){
	case 0: 
		break;
	case STOP:
		CP10View::GetView()->AfficherMessageConsole(CString("La grue est arrêtée."));
		break;
	case OPEN:
		CP10View::GetView()->AfficherMessageConsole(CString("Un déplacement boucle ouverte est en cours d’exécution."));
		break;
	case CLOSE:
		CP10View::GetView()->AfficherMessageConsole(CString("Un déplacement boucle fermée est en cours d’exécution."));
		break;
	case RESET:
		CP10View::GetView()->AfficherMessageConsole(CString("La grue cherche sa position d’origine."));
		break;
	case ERR_OPEN_FILE:
		CP10View::GetView()->AfficherMessageConsole(CString("L'ouverture du fichié à échoué."));
		break;
	case ERR_CLOSE_FILE:
		CP10View::GetView()->AfficherMessageConsole(CString("La fermeture du fichié à échoué"));
		break;
	case ERR_WRITE_FILE:
		CP10View::GetView()->AfficherMessageConsole(CString("L'écriture dans le fichié à échoué"));
		break;
	case ERR_PARSE_FILE:
		CP10View::GetView()->AfficherMessageConsole(CString("Fichier introuvable ou format du fichier non valide."));
		break;
	case ERR_NULL_PTR:
		CP10View::GetView()->AfficherMessageConsole(CString("Un pointeur nul a été rencontré (peut être relié à un problème d’allocation dynamique de la mémoire).\n"));
		break;
	case ERR_THREAD:  
		CP10View::GetView()->AfficherMessageConsole(CString("Le démarrage du « thread » de communications et de surveillance du DSP a échoué."));
		break;
	case ERR_READ_DSP:
		CP10View::GetView()->AfficherMessageConsole(CString("La réception de données du DSP a échouée."));
		break;
	case ERR_WRITE_DSP:
		CP10View::GetView()->AfficherMessageConsole(CString("L’envoi de données au DSP a échoué.\n"));
		break;
	case ERR_START_DSP :  
		CP10View::GetView()->AfficherMessageConsole(CString("Le démarrage du programme d’asservissement du DSP a échoué."));
		break; 
	case ERR_CLOSE_DSP :  
		CP10View::GetView()->AfficherMessageConsole(CString("La fermeture de la connexion avec le DSP a échouée."));
		break; 
	case ERR_OPEN_DSP :  
		CP10View::GetView()->AfficherMessageConsole(CString("L’initialisation de la communication avec le DSP a échoué."));
		break; 
	case ERR_SYNC_DSP :  
		CP10View::GetView()->AfficherMessageConsole(CString("La synchronisation avec le DSP a été perdue."));
		break; 
	case ERR_TYPE:
		CP10View::GetView()->AfficherMessageConsole(CString("Le type d’au moins un compensateur n’est pas reconnu.\n")); 
		break;
	case ERR_TF:
		CP10View::GetView()->AfficherMessageConsole(CString("Tous les paramètres d’au moins une des fonctions transfert sont nuls.\n")); 
		break;
	case ERR_PID:
		CP10View::GetView()->AfficherMessageConsole(CString("Tous les paramètres d’au moins un PID sont nuls.\n")); 
		break;
	case ERR_RUNNING:
		CP10View::GetView()->AfficherMessageConsole(CString("La grue est déja en déplacement"));
		break;
	case ERR_TRAJ:
		CP10View::GetView()->AfficherMessageConsole(CString("La trajectoire n’est pas spécifiée ou est non valide."));
		break;
	case ERR_TRAJ_FLAG:
		CP10View::GetView()->AfficherMessageConsole(CString("Le paramètre trajFlag est non valide."));
		break;
	case ERR_CTL_MODE:
		CP10View::GetView()->AfficherMessageConsole(CString("Le paramètre ctlMode est non valide."));
		break;
	default:
		CP10View::GetView()->AfficherMessageConsole(CString("Code d'erreur inconnu.\n"));
	}
}