#include "StdAfx.h"
#include "traitementImage.h"
#include "fftw3.h"
#include <math.h>
#include <stdlib.h>
#include <complex>
#include <iostream>
#include <fstream>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI          3.141592653589793   // Constante pi

////// test

static void displayVect(double* Vect, int length){

	for(int i = 0; i< length; i++){
		cout << Vect[i] << " ";
	}
	cout << "\n";
}

static void displayVect(int* Vect, int length){

	for(int i = 0; i< length; i++){
		cout << Vect[i] << " ";
	}
	cout << "\n";
}

/////


traitementImages::traitementImages()
{
}

traitementImages::~traitementImages()
{
}

/*************************************************
fonction pour trouver les paramètres du filtre à peigne par rapport à une image interférée
Auteurs: P10 Matlab: Marc-André Veilleux
			   C++ : Maxime Lussier
Date de création: 2011-07-19
Entrées:
- x : image non traitée (répertoire de l'image originale)
- M : nombre de lignes
- N : nombre de colonnes
Sorties:
- w0: fréquence normalisé des interférence
- deltaw0: largeur de bande normalisé de l'interférence (en référence)
**************************************************/
double traitementImages::trouverParametres(double** imageM,int M, int N, double* DeltaW0)
{
	// Détermination des paramètre constant
	// // Colonne sur laquelle nous allons étudier les fréquences
	int iColonne = 50;

	//     Détermination de la FFT de l'image (transformation dans le domaine
	//     fréquentielle)
	//     Détermination de l'amplitude des fréquence
	int length = M;
	double* inCol = MVOps::getColumn(imageM, 50, M);
	double *fftOut = traitementImages::fft(inCol, M); 

	// on efface le vecteur inCol inutile
	delete inCol;
	//     Ignorer les premières fréquences du spectre
	int FacteurDeDebut = 10;
	// on ignore la moitié (double) de la fft et on enlève les premières valeurs.
	double* ampSansDebut = MVOps::subVector(fftOut, FacteurDeDebut, (int)(M/2));
	// on efface le tableau fft inutile
	delete fftOut;
	// on calcule la nouvelle valeur du vecteur fft
	int newLength = ((int)(M/2))-FacteurDeDebut;
	// Trouve la fréquence non-normalisé du maximum du spectre de fréquence
	int* peak = TrouverPoints(ampSansDebut, newLength ,1);
	int peakI = peak[0] + FacteurDeDebut + 1;  // à vérifier
	delete peak;

	// on efface le vecteur ampSansDebut inutile
	delete ampSansDebut;

	//     Normalisation des fréquences
	double W0 = (((double)peakI-1)/(double)M)*(2*PI);

	//     Détermination de la largeur de bande par essai et erreur.
	*DeltaW0 = W0/3;

	return W0;
}

/*************************************************
Fonction qui retourne les indexes des N plus grand peaks du vecteur d'amplitude (Appears to work)
Auteurs: P10 Matlab: Marc-André Veilleux
			   C++ : Maxime Lussier
Date de création: 2011-07-19
Entrées:
- amplitude : vecteur d'amplitude
- length : longueur du vecteur
- N : nombre de peaks voulu
Sorties:
- les indexes des peaks
**************************************************/
// Retourne les indexes des N plus grand peaks du vecteur d'amplitude (Appears to work)
int* traitementImages::TrouverPoints(double* amplitude, int length ,int N){

	// Nombre d'itération maximal avant d'arrêter la recherche binaire
	int iterationMAX = 30;

	// Initialisation des variables
	int iteration = 0;
	int* points;
	int* pointsFiltres;
	int found = 0;

	// détermination des min et max de la recherche intialement
	int indiceMin = 0;
	int indiceMax = 0;
	double Ymin = MVOps::minimum(amplitude, length, &indiceMin);   
	double Ymax = MVOps::maximum(amplitude, length, &indiceMax);
	double Yevaluate = (Ymin + Ymax)/2;


	while(found < N && iteration < iterationMAX){
		iteration++; // Augmente le nombre d'itération
		points = MVOps::FindPoints(amplitude, Yevaluate, length, &found); // Retourne les points qui sont audessus de la limite actuelle
		pointsFiltres = FiltrePoints(points,amplitude, &found); //Filtre les points qui sont semblable
		delete points;
		if(found > N) // Vérifie s'il plus ou moins de point que demander
			Ymin = Yevaluate;
		else
			Ymax = Yevaluate;
		Yevaluate = (Ymin + Ymax)/2;
	}

	return pointsFiltres;
}

/*************************************************
Filtre les points en entrée pour éliminer les points semblables (Appears to work)
Auteurs: P10 Matlab: Marc-André Veilleux
			   C++ : Maxime Lussier
Date de création: 2011-07-19
Entrées:
- points : vecteur des indices des maximums
- amplitude : vecteur d'amplitude
- length : longueur du vecteur
Sorties:
- le vecteur des indices restants
**************************************************/
int* traitementImages::FiltrePoints(int* points, double* VecteurAmplitude, int* length){
	int ConstanteDeSeuil = 5;
	int* pointsFiltre =  MVOps::zeros(*length);
	int i = 0;
	int LastElem = 0;
	while(i < *length){
		int x = i+1;
		while((ConstanteDeSeuil >= (points[x]-points[x-1])) && (x < *length)){
			x++; 
		}
		double* ValuesAt = MVOps::getValuesAt(VecteurAmplitude, points, *length);
		double* VecteurATrouverMax = MVOps::subVector(ValuesAt, i, x-1);
		delete ValuesAt;
		int I = 0;
		double value = MVOps::maximum(VecteurATrouverMax, (x-i) , &I);
		delete VecteurATrouverMax;
		pointsFiltre[LastElem] = points[i+I];
		i = x;
		LastElem = LastElem+1;
	}
	int* temp = MVOps::subVector(pointsFiltre, 0, LastElem);
	delete pointsFiltre;
	*length = LastElem;
	return temp;
}

/*************************************************
Fonction pour calculer l'absolue de la fft.
Auteurs: P10 : Maxime Lussier
Date de création: 2011-07-19
Entrées:
- inCol : colonne à calculer
- length : longueur de la colonne
Sorties:
- fft calculée
**************************************************/
double* traitementImages::fft(double* inCol, int length)
{

	fftw_complex    *data, *fft_result;
	fftw_plan       plan_forward;
	int             i;

	data        = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * length );
	fft_result  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * length );

	plan_forward  = fftw_plan_dft_1d( length, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE );

	/* populate input data */
	for( i = 0 ; i < length ; i++ ) {
		data[i][0] = inCol[i];
		data[i][1] = 0.0;
	}

	fftw_execute( plan_forward );

	double* absOut = new double[length];
	for( i = 0 ; i < length ; i++ ) {
		complex<double> mycomplex (fft_result[i][0],fft_result[i][1]);
		absOut[i] = abs(mycomplex) / 10;
	}

	/* free memory */
	fftw_destroy_plan( plan_forward );

	fftw_free( data );
	fftw_free( fft_result );

	return absOut;

}

/*************************************************
Fonction pour filtre l'image d'entrée avec une filtre à peigne avec une fréquence normalisé W0 et une largeur de DeltaW0 trouver les paramètres du filtre à peigne par
rapport à une image interférée
Auteurs: P10 Matlab: Marc-André Veuilleux
				C++: Maxime Lussier
Date de création: 2011-07-19
Entrées:
- X : image non traitée (répertoire de l'image originale)
- M : nombre de lignes
- N : nombre de colonnes
- W0 : Fréquence normalisé de l'interférence
- DeltaW0: Largeur de bande normalisé de l'interférence
Sorties:
- Y: Image filtrée par le filtre à peigne
**************************************************/
double** traitementImages::filtrer(double** X, int M, int N, double W0, double DeltaW0){
	DeltaW0 = W0/2;

	// Détermination des constantes pour le filtre (Beta et Alpha)
	double Beta_ = cos(W0);
	double Alpha_ = (1 - sin(DeltaW0))/cos(DeltaW0);


	// Initialisation de la sortie avec l'entrée
	double** Y = MVOps::subMatrix(X, 0, M-1, 0, N-1);
	//col = N
	//     Itération pour le filtre
	for(int j = 0; j < N; j++){
		for(int i = 2; i < M; i++){
			//             Détermination de la sortie du filtre pour chacun des pixels
			double temp = ((1+Alpha_)/2) * (X[i][j] - 2*X[i-1][j]*Beta_ + X[i-2][j]) + Y[i-1][j]*(1+Alpha_)*Beta_ - Y[i-2][j]*Alpha_;
			Y[i][j] = temp;
		}
	}
	return Y;
}



/**********************************************************************************
Fonction pour enlever le bruit d'une image.
Auteurs: P10 Matlab: Michael Fujioka
				C++: Maxime Lussier
Date de création: 2011-07-19
Entrées:
- X : image originale
- M : nombre de lignes
- N : nombre de colonnes
Sorties:
- Y : image traitée
***********************************************************************************/
double** traitementImages::selEtPoivre(double** X, int M, int N){

	int Xmax = N;
	int Ymax = M;

	//double** Y = MVOps::subMatrix(X, 0, Ymax-1, 0, Xmax-1);


	// initialise les seuils
	double seuil = 0.25; // pour le pourcentage de valeur similaires permis
	double area = 0.08;  // pour la différence entre les éléments adjacents
	double marge = 0.05; // pour la différence entre l'ancienne valeur et la valeur calculée

	// boucle pour vérifier tous les pixels
	for(int j = 0; j < Xmax; j++){
		for(int i = 0; i < Ymax; i++){

			double* matM = MVOps::zerosD(4);
			int count = 0;

			// valeur de début en y
			int y1  = 0;
			// valeur de fin en y
			int y1_ = 0;

			// ces conditions déterminent les cases qui seront
			// analysée. du à l'interférence, on vérifie juste les y. Cette
			// algorithme permet de déterminer les bornes des
			// valeurs en y.
			if(i >= 2)
				y1 = -2;
			else if(i = 1)
				y1 = -1;
			else
				y1 = 0;
			if(i < Ymax-2)
				y1_ = 2;
			else if(i < Ymax-1)
				y1_ = 1;
			else
				y1_ = 0;


			int nb = 0;

			// compte le nombre de pixel qui
			// sont similaire au pixel courant
			for(int k = i + y1; k <= i + y1_; k++){
				if(k != j){
					if((abs(X[i][j]-X[k][j])/255) < area)
						count = count + 1; 
					// compte le nombre de case
					nb++;
				}
			}

			// détermine si le pixel est abérant, si oui
			// on le change par une moyenne de pixels
			// adjacents.
			if(((double)(count/nb)) < seuil){

				// détermine les valeurs qui
				// feront partie de la moyenne.
				// Les pixels qui ne sont comptabilisés
				// sont les pixels qui ne sont pas
				// dans les limites de l'image
				int nbCote = 0;
				int nbLoin = 0;

				if(j >= 1){
					matM[0] = X[i][j-1]; 
					nbCote++;
					if(j >= 2){
						matM[2] = X[i][j-2]; 
						nbLoin++;
					}
				}


				if(j <= Xmax-1){
					matM[1] = X[i][j+1]; 
					nbCote++;
					if(j <= Xmax-2){
						matM[3] = X[i][j+2]; 
						nbLoin++;
					}
				}

				// calcul de la moyenne
				double som = 0.7*((matM[0]+matM[1])/nbCote)+0.3*((matM[2]+matM[3])/nbLoin);

				// si le changement calculé dépasse
				// un certain seuil, le pixel change
				// de valeur par la moyenne calculée
				if((abs(X[i][j]-som)/255) > marge)
				{
					X[i][j] = som;
				}

			}
			delete matM;
		}
	}

	return X;
}

double** traitementImages::interference(double** X, int M, int N){
	double DeltaW0 = 0;
	double W0 = traitementImages::trouverParametres(X, M, N, &DeltaW0);
	return traitementImages::filtrer(X, M, N, W0, DeltaW0);
}


/*************************************************
Fonction pour filtre l'image d'entrée pour enlever le flou
Auteurs: P10 Matlab: Marc-André Veuilleux
				C++: Marc-André Veuilleux
Date de création: 2011-07-19
Entrées:
- X : image non traitée (répertoire de l'image originale)
- M : nombre de lignes
- N : nombre de colonnes
Sorties:
- Y: Image filtrée par le filtre de defloutage
**************************************************/
double** traitementImages::filtrerFlou(double** X, int M, int N){

	//     Doublage de l'entrée pour atténuer l'effet du "setting time" du
	//     filtre



	//     Détermination des largeur et longueur de l'image


	int nbCoeff = 11;
	double *b = new double[nbCoeff];
	b[0] = -0.0000;
	b[1] = -0.0005;
	b[2] = 0.0085;
	b[3] = -0.0189;
	b[4] = -0.2487;
	b[5] = 1.5538;
	b[6] = -0.2487;
	b[7] = -0.0189;
	b[8] = 0.0085;
	b[9] = -0.0005;
	b[10] = 0.0000;


	// Initialisation de la sortie avec l'entrée
	int j2 = 0;
	double** Y = MVOps::subMatrix(X, 0, M-1, 0, N-1);

	//     Itération pour le filtre
	for(int j = 0; j < N; j++){
		for(int i = 0; i < M; i++){
			//             Détermination de la sortie du filtre pour chacun des pixels
			double temp = 0;

			if(j<nbCoeff) {
				j2 = nbCoeff;
			} else {
				j2 = j;
			}

			for(int k=0; k<nbCoeff; k++){
				temp = temp + b[k] * X[i][j2-k];
			}
			Y[i][j] = temp;
		}
	}

	double** Z = MVOps::subMatrix(Y, 0, M-1, 5, N-1);
	delete [] b;
	MVOps::deleteMatrix(Y, M);
	return Z;
}

/*************************************************
Fonction pour calculer le PSNR entre deux images
Auteurs: P10 Maxime Lussier
Date de création: 2011-07-19
Entrées:
- Image1 : image à analyse 1
- Image2 : image à analyse 2
- M : nombre de lignes
- N : nombre de colonnes
Sorties:
- le PSNR filtré
**************************************************/
double traitementImages::PSNR(double** Image1, double** Image2, int M, int N)
{
	double Somme = 0;
	for(int j = 0 ; j < N ; j++)
	{
		for(int i = 0 ; i < M ; i++)
		{
			Somme += pow(ValidePixel(Image1[i][j])-ValidePixel(Image2[i][j]), 2);
		}
	}
	return 10*log10((255*255*(double)N*(double)M)/Somme);
}

/*************************************************
Fonction pour valider un pixel entre 0 et 255
Auteurs: P10 Maxime Lussier
Date de création: 2011-07-19
Entrées:
- X : valeur du pixel
Sorties:
- valeur validée
**************************************************/
double traitementImages::ValidePixel(double x)
{
	if (x > 255) return 255;
	if (x < 0) return 0;
	return x;
}

/*************************************************
Fonction pour détecter les références et une cible dans une image
Auteurs: P10 Matlab: Marc-André Veuilleux
		        C++: Maxime Lussier
Date de création: 2011-07-19
Entrées:
- X : image
- M : nombre de lignes
- N : nombre de colonnes
- target : cible à trouver
- Mt : nombre de lignes de la cible
- Nt : nombre de colonnes de la cible
- ref : référence à trouver
- Mr : nombre de lignes de la référence
- Nr : nombre de colonnes de la référence
Sorties:
- matrice contenant les positions des références et de la cible et la conversion pixel/m
**************************************************/
double** traitementImages::detectImage(double** X, int M, int N, double** target, int Mt, int Nt, double** ref, int Mr, int Nr){
	double** Y = MVOps::newMatrix(6, 2);
	double** Y1 = MVOps::zeros(M, N);
	double** Y2 = correlation(X, M, N, target, Mt,  Nt, ref, Mr, Nr, Y1);
	double** refsT = DetectPosition(Y1, M, N, 4);

	double** refs = MVOps::garderPerpendiculaires(refsT, 4);

	masquerPoints(Y2, M, N, refs);

	MVOps::deleteMatrix(refsT, 4);
	double** targ = DetectPosition(Y2, M, N, 1);
	MVOps::deleteMatrix(Y1, M);
	MVOps::deleteMatrix(Y2, M);
	Y[0][0] = refs[0][0];
	Y[0][1] = refs[0][1];
	Y[1][0] = refs[1][0];
	Y[1][1] = refs[1][1];
	Y[2][0] = refs[2][0];
	Y[2][1] = refs[2][1];
	Y[3][0] = targ[0][0];
	Y[3][1] = targ[0][1];
	MVOps::deleteMatrix(refs, 3);
	MVOps::deleteMatrix(targ, 1);
	FindPinv(Y);
	return Y;
}

/*************************************************
Fonction pour faire la corrélation des références et d'une cible dans une image
Auteurs: P10 Matlab: Marc-André Veuilleux
		        C++: Maxime Lussier
Date de création: 2011-07-19
Entrées:
- X : image
- M : nombre de lignes
- N : nombre de colonnes
- target : cible à trouver
- Mt : nombre de lignes de la cible
- Nt : nombre de colonnes de la cible
- ref : référence à trouver
- Mr : nombre de lignes de la référence
- Nr : nombre de colonnes de la référence
Sorties:
- matrice contenant la corrélation de la cible
- matrice contenant la corrélation de la référence (EN RÉFÉRENCE)
**************************************************/
double** traitementImages::correlation(double** X, int M, int N, double** target, int Mt, int Nt, double** ref, int Mr, int Nr, double** corrRef){
	double SeuilDetectionNoir = 15;

	double meanTarget = MVOps::mean(target, Mt, Nt);
	double meanRef = MVOps::mean(ref, Mr, Nr);

	//Initialisation de la matrice de sortie

	double** Yimg = MVOps::zeros(M,N);

	//On enlève les bordures dans l'image
	int minX = floor((double)Nt/2);
	int maxX = floor((double)N-(double)Nt/2)-1;
	int minY = floor((double)Mt/2);
	int maxY = floor((double)M-(double)Mt/2)-1;

	//Pour tous les lignes de l'image X1  
	for(int i = minY; i < maxY; i++){
		for(int j = minX; j < maxX; j++){
			if(j == 630 && i == 65)
			{
				int w = 0;
			}
			if(X[i][j] < SeuilDetectionNoir){
				//On respecte le seuil, donc on correle les points noirs.

				//On centre l'image et on le multiplie (le modele)
				int rowMin = i-floor((double)Mt/2);
				int rowMax = i+floor((double)Mt/2)-1;
				int colMin = j-floor((double)Nt/2);
				int colMax = j+floor((double)Nt/2)-1;

				double meanTemp = MVOps::mean(X, rowMin, rowMax, colMin, colMax);
				double sum = 0;

				for(int k = 0; k < Nt; k++){
					for(int l = 0; l < Mt; l++){
						double temp = X[rowMin+l][colMin+k] - meanTemp;
						double modele = target[l][k] - meanTarget;
						sum += temp*modele;
					}
				}
				Yimg[i][j] = sum;


				rowMin = i-floor((double)Mr/2);
				rowMax = i+floor((double)Nr/2)-1;
				colMin = j-floor((double)Nr/2);
				colMax = j+floor((double)Nr/2)-1;

				meanTemp = MVOps::mean(X, rowMin, rowMax, colMin, colMax);
				sum = 0;

				for(int k = 0; k < Nr; k++){
					for(int l = 0; l < Mr; l++){
						double temp = X[rowMin+l][colMin+k] - meanTemp;
						double reference = ref[l][k] - meanRef;
						sum += temp*reference;
					}
				}
				corrRef[i][j] = sum;
			} else {
				corrRef[i][j] = 0;
				Yimg[i][j] = 0;
			}
		}
	}
	return Yimg;
}

/*************************************************
Fonction pour détecter les positions des maximums
Auteurs: P10 Matlab: Marc-André Veuilleux
		        C++: Maxime Lussier
Date de création: 01-08-2011
Entrées:
- X : matrice de corrélation
- M : nombre de lignes
- N : nombre de colonnes
- Nb : nombre de points cherchés
Sorties:
- matrice contenant les indices des maximums
**************************************************/
double** traitementImages::DetectPosition(double** X, int M, int N, int Nb){
	// function [ vecteur, maximum, imagemodifier] = DetectPosition( corr, N )
	// Initialisation des variables
	int dimension = 20;
	double** vecteur = MVOps::newMatrix(Nb,2);
	double* maximum = new double[Nb];
	int* indice = new int[2];
	double max = 0;

	// Détection des maximums
	for(int i = 0 ; i < Nb ; i++)
	{
		// Détermine le maximum
		//int ans = 0;
		//Matrice* v = X->find_MinMax_Matrice(false, &ans);
		max = MVOps::maximum(X, M, N, indice);
		vecteur[i][0] = indice[0];
		vecteur[i][1] = indice[1];
		maximum[i] = max;
		// Détermination des bornes du carré gris
		int minY = indice[1] - dimension;
		int maxY = indice[1] + dimension;
		int minX = indice[0] - dimension;
		int maxX = indice[0] + dimension;
		// Conditions pour la création du carré gris
		if(minX<1){
			minX = 1;
		}
		if(minY<1){
			minY = 1;
		}
		if(maxY>M){
			maxY = M;
		}
		if(maxX>N){
			maxX = N;
		}
		// modification de la corrélation pour ajouter les carrés gris
		for(int k = 0 ; k < 41 ; k++)
		{
			for(int g = 0 ; g < 41 ; g++)
			{
				X[minY+k][minX+g] = 120;
			}
		}
	}
	return vecteur;
}

/*************************************************
Fonction pour masquer avec un carré les références dans la matrice de corréllation
Auteurs: P10 Maxime Lussier
Date de création: 01-08-2011
Entrées:
- X : matrice de corrélation
- M : nombre de lignes
- N : nombre de colonnes
- refs : matrice des position des références
Sorties:
- rien
**************************************************/
void traitementImages::masquerPoints(double** X, int M, int N, double** refs){
	int dimension = 20;
	for(int i = 0 ; i < 3 ; i++)
	{

		// Détermination des bornes du carré gris
		int minY = refs[i][1] - dimension;
		int maxY = refs[i][1] + dimension;
		int minX = refs[i][0] - dimension;
		int maxX = refs[i][0] + dimension;
		// Conditions pour la création du carré gris
		if(minX<1){
			minX = 1;
		}
		if(minY<1){
			minY = 1;
		}
		if(maxY>M){
			maxY = M;
		}
		if(maxX>N){
			maxX = N;
		}
		// modification de la corrélation pour ajouter les carrés gris
		for(int k = 0 ; k < 41 ; k++)
		{
			for(int g = 0 ; g < 41 ; g++)
			{
				X[minY+k][minX+g] = 120;
			}
		}
	}
}

/*************************************************
Fonction pour trouver les coordonnées en metres de la destination
Auteurs: P10 Maxime Lussier
Date de création: 01-08-2011
Entrées:
- coor : matrice contenant les positions en pixels
Sorties:
rien (positions mises dans la matrice d'entrée
**************************************************/
void traitementImages::FindPinv(double** coor)
{
	/*
	coor:
	ref1 (x, y)
	ref2 (x, y)
	ref3 (x, y)
	targ (x, y)
	dest (x, y)
	coef (x, y)
	*/

	double* longeur = new double[3];
	longeur[0] = MVOps::pythagore(coor[0][0]-coor[1][0], coor[0][1]-coor[1][1]);
	longeur[1] = MVOps::pythagore(coor[1][0]-coor[2][0], coor[1][1]-coor[2][1]);
	longeur[2] = MVOps::pythagore(coor[0][0]-coor[2][0], coor[0][1]-coor[2][1]);

	// Le plus long
	int pivot, A, B;
	if(longeur[0] > longeur[1] && longeur[0] > longeur[2]){
		pivot = 2;
		A = 1;
		B = 0;
		coor[5][0] = (longeur[1]+longeur[2])/2;
	} else if(longeur[1] > longeur[2] && longeur[1] > longeur[0]){
		pivot = 0;
		A = 1;
		B = 2;
		coor[5][0] = (longeur[0]+longeur[2])/2;
	} else {
		pivot = 1;
		A = 0;
		B = 2;
		coor[5][0] = (longeur[0]+longeur[1])/2;
	}

	double** bases = MVOps::newMatrix(2,2);
	bases[0][0] = (coor[A][0] - coor[pivot][0] );
	bases[0][1] = (coor[A][1] - coor[pivot][1] );
	bases[1][0] = (coor[B][0] - coor[pivot][0] );
	bases[1][1] = (coor[B][1] - coor[pivot][1] );

	double Vcross = bases[0][0]*bases[1][1] - bases[0][1]*bases[1][0];
	if( Vcross > 0)
	{
		// Si le cross product est négatif, on a inverser les deux vecteurs
		// a et b
		double temp = bases[0][0];
		bases[0][0] = bases[1][0];
		bases[1][0] = temp;
		temp = bases[0][1];
		bases[0][1] = bases[1][1];
		bases[1][1] = temp;
	}

	double** pinv = MVOps::matrixInversion(bases, 2);
	MVOps::deleteMatrix(bases, 2);

	coor[4][0] = (coor[3][0]-coor[pivot][0])*pinv[0][0]+(coor[3][1]-coor[pivot][1])*pinv[1][0];
	coor[4][1] = (coor[3][0]-coor[pivot][0])*pinv[0][1]+(coor[3][1]-coor[pivot][1])*pinv[1][1];
	MVOps::deleteMatrix(pinv, 2);
}

double** traitementImages::blackAndWhite(double** X, int M, int N){
	int seuil = 40;
	double** Z = MVOps::newMatrix(M, N);
	for(int i = 0; i<M; i++){
		for(int j = 0; j < M; j++){
			if(X[i][j] < seuil)
				Z[i][j] = 0;
			else
				Z[i][j] = 255;
		}
	}
	return Z;
}