#include <fftw3.h>

class FFT {
	public:
		int real = 0;
		int imag = 1;

		int chunkSize = 10000;
		int chunksCovered = 0;

		fftw_complex output;
		fftw_complex input; // double x[n][2];
		
		int freqBand = 10;
		// int freqBandArray[];
		int tempAmpFreq = 0;

		FFT() {
			
			input[chunkSize]; 
			output[chunkSize];
			
			// freqBandArray[freqBand];
			
		}

		// int* calculate_fft() {
			
		// }

};
	
	
	
	


	// return 0;

	// iterate through song
	// while (!(chunksCovered == sampleSize)) {
		
	// 	// fill buffer with new chunk of audio data
	// 	for ( int i=0; i<chunkSize; i++) {
	// 		input[i][real] = samples[chunksCovered+i];
	// 		input[i][imag] = 0;
	// 	}


	// 	// plan the fft and execute
	// 	fftw_plan plan = fftw_plan_dft_1d(chunkSize, input, output, FFTW_FORWARD, FFTW_ESTIMATE);
	// 	fftw_execute(plan);

	// 	fftw_destroy_plan(plan);
	// 	fftw_cleanup();


	// 	// compressing fft to required freq band range
	// 	for (int i=0; i<freqBand; i++) {
	// 	tempAmpFreq = 0;
	// 		for (int j=0; j<chunkSize/freqBand; j++){
	// 			tempAmpFreq += abs(output[(freqBand*i)+j][real]);
	// 		}
	// 		freqBandArray[i] = tempAmpFreq/(chunkSize/freqBand);
	// 	}

	// 	// output data for testing
	// 	std::cout << "CHUNKS COVERED: " << chunksCovered << std::endl;
	// 	// for ( int i = 0; i<chunkSize/2; i++) {
	// 	// 	if (output[i][imag] < 0)
	// 	// 		std::cout << output[i][real] << " - " << abs(output[i][imag]) << std::endl;
	// 	// 	else
	// 	// 		std::cout << output[i][real] << " + " << output[i][imag] << std::endl;

	// 	// 	// for ( int j = 0; j < chunkSize/2)
		
	// 	// }
	// 	for ( int i = 0; i<freqBand; i++) {
	// 			std::cout << freqBandArray[i] << std::endl;
	// 	}

	// 	// if (chunksCovered == 5000*30) {
	// 	// 	return 0;
	// 	// }

	// 	chunksCovered += chunkSize;
	// }