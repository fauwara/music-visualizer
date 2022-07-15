#include <fftw3.h>

class FFT {
	public:
		
		int real = 0;
		int imag = 1;

		const int freqBand = 30;
		const int chunkSize = 2947;
		int chunksCovered = 0;

		int tempAmpFreq = 0;

		FFT() {

			int powerSpectrum[chunkSize];
			int powerSpectrumSmoothed[freqBand];
			
			fftw_complex input[chunkSize]; // double x[n][2];
			fftw_complex output[chunkSize];
			
			// freqBandArray[freqBand];	
		}

		// int* calculate_fft() {
			
		// }

};

