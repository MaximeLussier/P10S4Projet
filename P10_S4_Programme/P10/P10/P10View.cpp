// P10View.cpp : implémentation de la classe CP10View
//

#include "stdafx.h"
#include "P10.h"

#include "P10Doc.h"
#include "P10View.h"
#include <iostream>
#include <fstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CP10View

IMPLEMENT_DYNCREATE(CP10View, CFormView)

	BEGIN_MESSAGE_MAP(CP10View, CFormView)
		ON_BN_CLICKED(IDC_BUTTON2, &CP10View::OnBnClickedRazerobutton)
		ON_BN_CLICKED(RAZeroButton, &CP10View::OnBnClickedRazerobutton)
		ON_BN_CLICKED(FullStopButton, &CP10View::OnBnClickedFullstopbutton)
		ON_BN_CLICKED(StartButton, &CP10View::OnBnClickedStartbutton)
		ON_BN_CLICKED(StatusButton, &CP10View::OnBnClickedStatusbutton)
		ON_COMMAND(OpenTrajectory, &CP10View::OnOpentrajectory)
		ON_COMMAND(OpenTarget, &CP10View::OnOpentarget)
		ON_WM_TIMER()
		ON_COMMAND(OpenCompensators, &CP10View::OnOpenCompensators)
		ON_COMMAND(SaveSensors, &CP10View::OnSaveSensors)
		ON_COMMAND(ControleRAZ, &CP10View::OnBnClickedRazerobutton)
		ON_COMMAND(ControleFullStop, &CP10View::OnBnClickedFullstopbutton)
		ON_BN_CLICKED(BtnFiltree, &CP10View::OnBnClickedBtnfiltree)
		ON_BN_CLICKED(BtnBruite, &CP10View::OnBnClickedBtnbruite)
		ON_COMMAND(ID_FICHIER_ENREGISTERIMAGEFILTR32793, &CP10View::OnFichierEnregisterimagefiltre)
		ON_BN_CLICKED(BtnOriginale, &CP10View::OnBnClickedBtnoriginale)
		ON_BN_CLICKED(ButtonTakePic, &CP10View::OnBnClickedButtontakepic)
	END_MESSAGE_MAP()

	// construction ou destruction de CP10View

	CP10View::CP10View()
		: CFormView(CP10View::IDD)
	{
		Ofile = new ofstream();
		Ofile->open ("c:/MessagesOut.txt");
	}

	CP10View::~CP10View()
	{
		theApp.getCraneInstance()->CloseSensorFile();
		if (Ofile)
		{
			Ofile->close();
		}
	}

	void CP10View::DoDataExchange(CDataExchange* pDX)
	{
		CFormView::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_LIST1, ConsoleBoxControl);
		DDX_Control(pDX, PictureBox, m_Picture);
		DDX_Control(pDX, TargeImage, targetImageControl);
		DDX_Control(pDX, beta, BetaControl);
		DDX_Control(pDX, Alpha, AlphaControl);
		DDX_Control(pDX, Xw, XwControl);
		DDX_Control(pDX, Yw, YwControl);
		DDX_Control(pDX, Corde, RControl);
		DDX_Control(pDX, Xc, XcControl);
		DDX_Control(pDX, Yc, YcControl);
		DDX_Control(pDX, Zc, ZcControl);
		DDX_Control(pDX, PictureBox2, ImageBruitControl);
		DDX_Control(pDX, BtnFiltree, ButtonFiltreeControl);
		DDX_Control(pDX, BtnBruite, ButtonBruiteControl);
		DDX_Control(pDX, BtnOriginale, ButtonOriginalControl);
		DDX_Control(pDX, StartButton, StartButtonControl);
		DDX_Control(pDX, PSNR_Avant, PSNR_Avant_Control);
		DDX_Control(pDX, PSNR_Apres, PSNR_Apres_Control);
		DDX_Control(pDX, destination, destinationControl);
		DDX_Control(pDX, destination2, destination2Control);
	}

	BOOL CP10View::PreCreateWindow(CREATESTRUCT& cs)
	{
		return CFormView::PreCreateWindow(cs);
	}

	void CP10View::OnInitialUpdate()
	{
		CFormView::OnInitialUpdate();
		GetParentFrame()->RecalcLayout();
		ResizeParentToFit();
		CRect r;
		r.top = 0;
		// Ici on indique la grandeur de l'interface en pixels
		r.bottom = 850;
		r.right = 1280;   
		CalcWindowRect(&r);
		GetParent()->SetWindowPos(NULL, 0, 0, r.Width(), r.Height(), SWP_NOZORDER);
		// On initialise le timer 1 à toutes les 300ms 
		SetTimer(1, 300, NULL); // timer des capteurs
		// On initialise le timer 2 à toutes les 100ms
		SetTimer(2, 100, NULL);
	}


	// diagnostics pour CP10View

#ifdef _DEBUG
	void CP10View::AssertValid() const
	{
		CFormView::AssertValid();
	}

	void CP10View::Dump(CDumpContext& dc) const
	{
		CFormView::Dump(dc);
	}

	CP10Doc* CP10View::GetDocument() const // la version non déboguée est en ligne
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CP10Doc)));
		return (CP10Doc*)m_pDocument;
	}
#endif //_DEBUG

//------------------------------------------
//	Gestionnaires de messages pour CP10View
//  Affiche les messages dans la boite sur l'interface
//	Entrées: message à afficher
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::AfficherMessageConsole(CString message)
	{
		ConsoleBoxControl.AddString(message);
		*Ofile << message << endl;
		int nCount = ConsoleBoxControl.GetCount();
		if (nCount > 0)
			ConsoleBoxControl.SetCurSel(nCount - 1);
	}

	// Fonction permettant d'obtenir la classe de l'interface à partir de partout...
//------------------------------------------
//	Fonction permettant d'obtenir la classe de l'interface à partir de partout...
//	Entrées: vide
//	Retourne: l'instance de la "view"
//	Auteur: Maxime Lussier
//------------------------------------------
	CP10View * CP10View::GetView()
	{
		CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);

		CView * pView = pFrame->GetActiveView();

		if ( !pView )
			return NULL;

		if ( ! pView->IsKindOf( RUNTIME_CLASS(CP10View) ) )
			return NULL;

		return (CP10View *) pView;
	}

//------------------------------------------
//	Code exécuté lorsque le bouton de remise à zéro est appuyé ou dans le menu ou Ctrl+Z
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnBnClickedRazerobutton()
	{
		theApp.clearStack();
		theApp.addCommandToStack(GRAZ);
	}


//------------------------------------------
//  Code exécuté lorsque le bouton d'arrêt est appuyé ou dans le menu ou Ctrl+X
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnBnClickedFullstopbutton()
	{
		theApp.clearStack();
		theApp.addCommandToStack(GSTOP);
	}
	
//------------------------------------------
// Code exécuté lorsque le bouton de démarrage est appuyé
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnBnClickedStartbutton()
	{
		theApp.addCommandToStack(GRUN);
		theApp.addCommandToStack(PICTURE);
	}

//------------------------------------------
//	Fonction exécutée à chaque interval d'un timer
//	Entrées: l'identificateur du timer
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnTimer(UINT_PTR nIDEvent)
	{
		if(nIDEvent == 1){ // si le OnTime est appelé par le timer 1
			int res = theApp.getCraneInstance()->GetCommand(); // On identifie la commande en cours d'exécution
			if(res == STOP) // On affiche le message de retour quel qu'il soit dans la barre de status
				GetParentFrame()->SetMessageText(CString("La grue est arrêtée."));
			else if(res == OPEN)
				GetParentFrame()->SetMessageText(CString("Un déplacement boucle ouverte est en cours d’exécution."));
			else if(res == CLOSE)
				GetParentFrame()->SetMessageText(CString("Un déplacement boucle fermée est en cours d’exécution."));
			else if(res == RESET)
				GetParentFrame()->SetMessageText(CString("La grue cherche sa position d’origine."));
			else
				GetParentFrame()->SetMessageText(CString("Message de retour inconnu."));
		}
		if(nIDEvent == 1){ // si le temps du timer 1 est trop long creer un nouveau timer...
			actualiserSensorBoxes();
		}
		if(nIDEvent == 2){
			theApp.executeStack();
		}


	}

//------------------------------------------
//	Fonction qui actualise les valeurs des capteurs
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::actualiserSensorBoxes(){
		float sen[5]; //[xw, yw, r, alpha,beta]
		int res = theApp.getCraneInstance()->GetSensor(sen);
		CString temp;
		float posCart[3];
		positionCharge(sen, posCart);
		if(res == 0){ 
			// on affiche rien car sinon on a un affichage / 300ms
			temp.Format("%f", sen[4]);
			BetaControl.SetWindowText(temp);
			temp.Format("%f", sen[3]);
			AlphaControl.SetWindowText(temp);
			temp.Format("%f", sen[0]);
			XwControl.SetWindowText(temp);
			temp.Format("%f", sen[1]);
			YwControl.SetWindowText(temp);
			temp.Format("%f", sen[2]);
			RControl.SetWindowText(temp);
			temp.Format("%f", posCart[0]);
			XcControl.SetWindowText(temp);
			temp.Format("%f", posCart[1]);
			YcControl.SetWindowText(temp);
			temp.Format("%f", posCart[2]);
			ZcControl.SetWindowText(temp);
		} else 
			theApp.messages(res);
	}

	  
//------------------------------------------
//  Code exécuté lorsque le bouton status est appuyé
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnBnClickedStatusbutton()
	{
		int res = theApp.getCraneInstance()->GetCommand();  // On identifie la commande en cours d'exécution
		theApp.messages(res);
		res = theApp.getCraneInstance()->GetErr();
		if(res != 0)
			theApp.messages(res);
		theApp.getCraneInstance()->ClearErr();
	}
	
//------------------------------------------
// Méthode utilisée lorsqu'on veut spécifier un fichier de trajectoire
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnOpentrajectory()
	{
		if(theApp.getCraneInstance()->GetCommand() != STOP)
		{
			AfficherMessageConsole(CString("Attendre l'arrêt de la grue pour charger une trajectoire."));
			return;
		}
		CString OpenFilter;

		OpenFilter = "Fichier de trajectoire (*.bin)|*.bin||";

		CFileDialog FileOpenDialog(TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
			OpenFilter,
			this,
			0,
			TRUE);

		if(FileOpenDialog.DoModal()!=IDCANCEL){
			char Path[256]; // Assez long pour un path?
			strcpy(Path, (LPCTSTR)FileOpenDialog.GetPathName());
			int res = theApp.getCraneInstance()->LoadTrajFile(Path);
			if(res == 0){
				AfficherMessageConsole(CString("Trajectoire chargée avec succès."));
				StartButtonControl.EnableWindow(TRUE);
			}
			else
				theApp.messages(res);
		}
		else
			AfficherMessageConsole(CString("Échec de chargement de la trajectoire"));
	}


	//------------------------------------------
	//
	// Lorsque l'utilisateur appuie sur le bouton " Parcourir ", cette
	//	méthode est appelée. Elle utilise l'information déjà inscrite 
	//	dans le champ " Choisir une image : " et y replace la nouvelle 
	//	sélection de l'utilisateur en terminant. 
	//
	// L'utilisation de l'objet CFileDialog permet l'accès à la fenêtre 
	//	" standard " de MS-Windows pour la recherche de fichier.
	//
	// Notez qu'un filtre est ajouté pour faciliter la recherche des 
	//	" .BMP ".
	//
	//	Le squelette de cette méthode est généré par MS-VS.
	//
	//------------------------------------------
	void CP10View::OnOpentarget()  
	{
		CString OpenFilter;

		OpenFilter = "Fichier cible (*.bmp)|*.bmp||";

		CFileDialog FileOpenDialog(TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST,
			OpenFilter,
			this,
			0,
			TRUE);

		if(FileOpenDialog.DoModal()!=IDCANCEL){
			char Path[256]; // Assez long pour un path?
			strcpy(Path, (LPCTSTR)FileOpenDialog.GetPathName());
			theApp.ChargerCible(Path);
		}
		else
			AfficherMessageConsole(CString("Échec de l'ouverture du bitmap de la cible"));
	}


	
//------------------------------------------
//  Fonction executée lorsqu'on veut charger une fichier de configuration des conpensateurs 
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnOpenCompensators()
	{
		if(theApp.getCraneInstance()->GetCommand() != STOP)
		{
			AfficherMessageConsole(CString("Attendre l'arrêt de la grue pour charger les compensateurs."));
			return;
		}

		CString OpenFilter;

		OpenFilter = "Fichier Conpensateurs (*.ini)|*.ini||";

		CFileDialog FileOpenDialog(TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST,
			OpenFilter,
			this,
			0,
			TRUE);

		if(FileOpenDialog.DoModal()!=IDCANCEL){
			char Path[256]; // Assez long pour un path?
			strcpy(Path, (LPCTSTR)FileOpenDialog.GetPathName());
			theApp.ConfigureCrane(Path);
			theApp.compLoaded = true;
		}
		else
			AfficherMessageConsole(CString("Échec de l'ouverture du fichier ini"));
	}


//------------------------------------------
//  Fonction executée lorsqu'on veut définir fichier ou les données des capteur seront enregistrés 
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnSaveSensors() 
	{

		CString SaveFilter;

		SaveFilter = "Fichier de Capteurs (*.bin)|*.bin||";

		CFileDialog FileOpenDialog(FALSE, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
			SaveFilter,
			this,
			0,
			TRUE);

		if(FileOpenDialog.DoModal()!=IDCANCEL){
			char Path[256]; // Assez long pour un path?
			strcpy(Path, (LPCTSTR)FileOpenDialog.GetPathName());
			int res = theApp.getCraneInstance()->OpenSensorFile(Path);
			if(res == 0){
				AfficherMessageConsole(CString("Fichier de sortie des capteurs spécifié avec succès."));
			}
			else
				theApp.messages(res);
		}
		else
			AfficherMessageConsole(CString("Échec de spécification du fichier bin"));
	}

//------------------------------------------
//  Fonction pour calculer la position cartésienne de la charge à partir de la position sphérique
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------	
	void CP10View::positionCharge(float* posSpheric, float* posCart){
		//[xw, yw, r, alpha,beta]

		posCart[0] = (posSpheric[2] * cos(posSpheric[3]) * sin(posSpheric[4])) + posSpheric[0]; // Xc = rcos(alpha)sin(beta) + Xw
		posCart[1] = (posSpheric[2] * sin(posSpheric[3])) + posSpheric[1];  // Yc = rsin(alpha) + Yw
		posCart[2] = (posSpheric[2] * cos(posSpheric[3]) * cos(posSpheric[4])); // Zc = rcos(alpha)cos(beta)

		// [Xc, Yc, Zc]
	}

//------------------------------------------
//  Code exécuté lorsque le bouton pour l'image filtrée est appuyé
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnBnClickedBtnfiltree()
	{
		ButtonFiltreeControl.EnableWindow(FALSE);
		ButtonBruiteControl.EnableWindow(TRUE);
		ButtonOriginalControl.EnableWindow(TRUE);
		theApp.DisplayImage(&m_Picture, &theApp.m_BMP_filtered, &theApp.m_CDC_filtered);
	}

//------------------------------------------
//  Code exécuté lorsque le bouton pour l'image bruitée est appuyé
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnBnClickedBtnbruite()
	{
		ButtonFiltreeControl.EnableWindow(TRUE);
		ButtonBruiteControl.EnableWindow(FALSE);
		ButtonOriginalControl.EnableWindow(TRUE);
		theApp.DisplayImage(&m_Picture, &theApp.m_BMP_bruite, &theApp.m_CDC_bruite);

	}

//------------------------------------------
//  Code exécuté lorsque le bouton pour enregister l'image filtrée est appuyé
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnFichierEnregisterimagefiltre()
	{
		theApp.sauvegarde();
	}

//------------------------------------------
//  Code exécuté lorsque le bouton pour l'image originale est appuyé
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnBnClickedBtnoriginale()
	{
		ButtonOriginalControl.EnableWindow(FALSE);
		ButtonFiltreeControl.EnableWindow(TRUE);
		ButtonBruiteControl.EnableWindow(TRUE);
		theApp.DisplayImage(&m_Picture, &theApp.m_BMP_original, &theApp.m_CDC_original);
	}

//------------------------------------------
//  Code exécuté lorsque le bouton pour prendre une image est appuyé
//	Entrées: vide
//	Retourne: vide
//	Auteur: Maxime Lussier
//------------------------------------------
	void CP10View::OnBnClickedButtontakepic()
	{
		theApp.clearStack();
		theApp.getPicture();
	}
