
#include "fourier.hpp"
#include <cmath>
#include <fstream>
#include <map>

void calcFftIndices(int K, int* indices)
{
	int i, j;
	int N;

	N = (int)log2f(K) ;

	indices[0] = 0 ;
	indices[1 << 0] = 1 << (N - (0 + 1)) ;
	for (i = 1; i < N; ++i)
	{
		for(j = (1 << i) ; j < (1 << (i + 1)); ++j)
		{
			indices[j] = indices[j - (1 << i)] + (1 << (N - (i + 1))) ;
		}
	}
}

void radix2DitCooleyTykeyFft(int K __attribute((annotate("scalar(disabled range(1,4194304))"))),
			     int* indices __attribute((annotate("scalar(disabled range(1,4194304))"))),
			     Complex* x __attribute((annotate("struct[scalar(range(-167776,167776)),scalar(range(-167776,167776))]"))), //__attribute((annotate("scalar(range(-167776,167776))"))),
			     Complex* f __attribute((annotate("struct[scalar(range(-167776,167776)),scalar(range(-167776,167776))]")))) //__attribute((annotate("scalar(range(-167776,167776))"))))
{
  /* This FFT implementation is buggy
   * x[0] should be < x[all i != 0] because the input is all positive, except it isn't
   * The actual maximum value is the integration of all values times 4 for some reason */
	calcFftIndices(K, indices) ;

	int step ;
	float __attribute((annotate("scalar()"))) arg ; // range(0,1.0) error(1e-10)
	int eI ;
	int oI ;

	float __attribute((annotate("target('dataOut') scalar()"))) fftSin ; // range(-1.0,1.0)
	float __attribute((annotate("target('dataOut') scalar()"))) fftCos ; // range(-1.0,1.0)

	Complex __attribute((annotate("struct[scalar(),scalar()]"))) t;
	//float __attribute((annotate("target('t_real') scalar()"))) t_real ; // range(-167776,167776)
	//float __attribute((annotate("target('t_imag') scalar()"))) t_imag ; // range(-167776,167776)
	int i ;
	int __attribute((annotate("scalar(disabled range(1,4194304))"))) N ;
	int j ;
	int __attribute((annotate("scalar(disabled range(1,4194304))"))) k ;
/*
	double __attribute((annotate("range -1.0 1.0"))) dataIn[1];
	double __attribute((annotate("target:dataOut range -1.0 1.0"))) dataOut[2];
*/
	for(i = 0, N = 1 << (i + 1); N <= K ; i++, N = 1 << (i + 1))
	{
		for(j = 0 ; j < K ; j += N)
		{
			step = N >> 1 ;
			for(k = 0; k < step ; k++)
			{
				arg = (float)k / N ;
				eI = j + k ;
				oI = j + step + k ;
/*
				dataIn[0] = arg;

#pragma parrot(input, "fft", [1]dataIn)
*/
				fftSinCos(arg, &fftSin, &fftCos);
/*
				dataOut[0] = fftSin;
				dataOut[1] = fftCos;

#pragma parrot(output, "fft", [2]<0.0; 2.0>dataOut)

				fftSin = dataOut[0];
				fftCos = dataOut[1];
*/

				// Non-approximate
				t =  x[indices[eI]] ;
				x[indices[eI]].real = t.real + (x[indices[oI]].real * fftCos - x[indices[oI]].imag * fftSin);
				x[indices[eI]].imag = t.imag + (x[indices[oI]].imag * fftCos + x[indices[oI]].real * fftSin);

				x[indices[oI]].real = t.real - (x[indices[oI]].real * fftCos - x[indices[oI]].imag * fftSin);
				x[indices[oI]].imag = t.imag - (x[indices[oI]].imag * fftCos + x[indices[oI]].real * fftSin);
/*
				t_real = COMPLEX_REAL(x,indices[eI]);
				t_imag = COMPLEX_IMAG(x,indices[eI]);
				COMPLEX_REAL(x,indices[eI]) = t_real + (COMPLEX_REAL(x,indices[oI]) * fftCos - COMPLEX_IMAG(x,indices[eI]) * fftSin);
                COMPLEX_IMAG(x,indices[eI]) = t_imag + (COMPLEX_IMAG(x,indices[eI]) * fftCos + COMPLEX_REAL(x,indices[oI]) * fftSin);

                COMPLEX_REAL(x,indices[oI]) = t_real - (COMPLEX_REAL(x,indices[oI]) * fftCos - COMPLEX_IMAG(x,indices[eI]) * fftSin);
                COMPLEX_IMAG(x,indices[eI]) = t_imag - (COMPLEX_IMAG(x,indices[eI]) * fftCos + COMPLEX_REAL(x,indices[oI]) * fftSin);
*/
			}

			//printf("%f, %f, %f\n", arg, t_real, t_imag);
		}
	}

	for (int i = 0 ; i < K ; i++)
	{
		f[i] = x[indices[i]] ;
	}
}
