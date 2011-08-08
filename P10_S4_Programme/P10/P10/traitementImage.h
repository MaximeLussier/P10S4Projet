// Classe comprenant le code effectuant le traitement de l'image

#include "MVOps.h"
#include "resource.h"       // symboles principaux

class traitementImages
{
public:
	traitementImages();
	~traitementImages();
	static double* fft(double* inCol, int length);
	static double trouverParametres(double** imageM, int M, int N, double* DeltaW0);
	static int* FiltrePoints(int* points, double* VecteurAmplitude, int* length);
	static int* TrouverPoints(double* amplitude, int length ,int N);
	static double** filtrer(double** X, int M, int N, double W0, double DeltaW0);
	static double** selEtPoivre(double** X, int M, int N);
	static double** interference(double** X, int M, int N);
	static double** filtrerFlou(double** X, int M, int N);
	static double PSNR(double** Image1, double** Image2, int M, int N);
	static double ValidePixel(double x);
	static double** blackAndWhite(double** X, int M, int N);

	static double** detectImage(double** X, int M, int N, double** target, int Mt, int Nt, double** ref, int Mr, int Nr);
	static double** correlation(double** X, int M, int N, double** target, int Mt, int Nt, double** ref, int Mr, int Nr, double** corrRef);
	static double** DetectPosition(double** X, int M, int N, int Nb);
	static void FindPinv(double** coor);
	static void masquerPoints(double** X, int M, int N, double** refs);
};