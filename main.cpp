#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <fftw3.h>
#include <GL/glut.h>

#include "Audio.hpp"
#include "Graphic/Bar.hpp"
// #include "fft.hpp"



// const char *audioFilePath = "Audio/freq-test.wav";



// initializing custom Audio class and loading song into buffer
std::string audioFilePath = "Audio/aero.wav";
// std::string audioFilePath = "Audio/freq-test.wav";
Audio audio;


// inintializing some OpenGL primitives
Bar bar;
float scaleBar;


// sf::Time delay = sf::milliseconds(27.5);
sf::Time delay = sf::milliseconds(25);


// initializing some fft variables
int real = 0;
int imag = 1;

const int freqBand = 30;
const int chunkSize = 2874;
// const int binBandwidth = 511;
int chunksCovered = 0;

int tempAmpFreq = 0;
// int freqBandArray[freqBand];
int powerSpectrum[chunkSize];
int powerSpectrumSmoothed[freqBand];

int maxFreq = 0;
int minFreq = 0;

int avgFreq = 0;

fftw_complex input[chunkSize]; // double x[n][2];
fftw_complex output[chunkSize];


float normalize(float num, float min, float max, float maximus){
	// zi = (xi – min(x)) / (max(x) – min(x))
	return ( (num-min) / (max-min) )*maximus;
}


// gets new fft values
void updateFFTInput() {

	maxFreq = 0;
	// minFreq = 100000;
	
	for (int i = 0; i < chunkSize; i++) {
		input[i][real] = audio.samples[chunksCovered+i];
		input[i][imag] = 0;
	}


	// fftw_plan plan = fftw_plan_dft_1d(binBandwidth, input, output, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_plan plan = fftw_plan_dft_1d(chunkSize, input, output, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);

	fftw_destroy_plan(plan);
	fftw_cleanup();

	// avgFreq = 0;
	// maxFreq = 0;
	for (int i = 0; i < chunkSize; i++) {

		powerSpectrum[i] = sqrt(pow(output[i+1][real], 2.0) + pow(output[i+1][imag], 2.0));

	}

	for (int i = 0; i<freqBand; i++){
		powerSpectrumSmoothed[i] = 0;
		for (int j = 0; j<chunkSize; j++){
			powerSpectrumSmoothed[i] += powerSpectrum[j]; 
		}
		powerSpectrumSmoothed[i] = powerSpectrum[i]/chunkSize;

		if (powerSpectrumSmoothed[i] > maxFreq){
			maxFreq = powerSpectrumSmoothed[i];
			// std::cout << "max: "<< maxFreq << std::endl;
		}

		// if (powerSpectrumSmoothed[i] < minFreq) {
		// 	minFreq = powerSpectrumSmoothed[i];
		// }
	}

	// std::cout << "	POWER " << std::endl;
	// for (int i = 0; i < freqBand; i++) {

	// 	std::cout << powerSpectrumSmoothed[i] << std::endl;

	// }
	
	// std::cout << "min: "<< minFreq << std::endl;

	// for (int i=0; i<freqBand; i++) {
	// 	tempAmpFreq = 0;
		// for (int j=0; j<chunkSize/(2*freqBand); j++){
		// 	tempAmpFreq += output[(i*(chunkSize/freqBand))+j][real];
		// 	// tempAmpFreq += abs(output[(i*(chunkSize/freqBand))+j][real]);
		// }
	// 	freqBandArray[i] = tempAmpFreq/(chunkSize/(2*freqBand));
	// }

	chunksCovered += chunkSize;

	// if (freqBandArray[1] > max) {
	// 	max = freqBandArray[1];
	// }

	// std::cout << max << std::endl;

	// std::cout << "START" << std::endl;
	// for ( int i = 0; i<511; i++) {
	// 	std::cout << abs(output[i][real]) << std::endl;
	// }
	// std::cout << "-----------START" << std::endl;
	// for ( int i = 1; i<chunkSize/2; i++) {
	// 	std::cout << powerSpectrum[i] << std::endl;
	// }
	

}

void displayBars() {

	for (int i = 1; i<freqBand; i++) {

		// std::cout << i << ": " << powerSpectrumSmoothed[i] << std::endl;
		// std::cout << i << ": " << minFreq << std::endl;
		// std::cout << i << ": " << maxFreq << std::endl;
		// std::cout << i << ": " << normalize(powerSpectrumSmoothed[i], minFreq, maxFreq, 2) << std::endl;
		// audio.song.setPlayingOffset(time);
		// glScalef(1.0, normalize(powerSpectrumSmoothed[i], minFreq, maxFreq, 2), 1.0);
		scaleBar = normalize(powerSpectrumSmoothed[i], minFreq, maxFreq, 4.5);
		glScalef(1.0, scaleBar, 1.0);
		glTranslatef(0.06, 0.0, 0.0);
		glDrawElements(GL_POLYGON, 8, GL_UNSIGNED_BYTE, bar.barIndices);
		// if (scaleBar) {
			glScalef(1.0, 1/scaleBar, 1.0);
		// } else {
		// 	glScalef(1.0, 1.0, 1.0);
		// }
		// glTranslatef(0.0, 0.0, 0.0);
		// glScalef(1.0, normalize(freqBandArray[1], 0, 1000000, 3), 1.0);
		// glTranslatef(0.15, 0.0, 0.0);
		// glDrawElements(GL_POLYGON, 8, GL_UNSIGNED_BYTE, bar.barIndices);
	}

}

// void scaleBars() {
// 	scale = freqBandArray[0];

// }

void display() {
	glClearColor( 1.0, 1.0, 1.0, 1.0 ); // Background Color
	// std::cout << "HELLO" << std::endl;
	glClear( GL_COLOR_BUFFER_BIT );
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// glScalef( normFloat, normFloat, normFloat );
	// glScalef( j, j, j );

	displayBars();
	// std::cout << "HELLO" << std::endl;

	glFlush();
	glutSwapBuffers();
	// std::cout << "HELLO" << std::endl;
}



void idlefunc() {

	sf::sleep(delay);
	updateFFTInput(); // gets new fft values
	// scaleBars();
	glutPostRedisplay();
	// std::cout << freqBandArray[0] << std::endl;

}



int main(int argc, char **argv) {
	

	// load raw audio data onto buffer
	audio.loadBuffer(audioFilePath);

	std::cout << "Sample Rate: " << audio.sampleRate << std::endl;
	std::cout << "Sample Count: " << audio.sampleCount << std::endl;

	// initial filling of fft input array
	updateFFTInput();
	// std::cout << "done";
	
	
	// initializing OpenGL
	glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("MUSIC VISUALIZER");
	// glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(idlefunc);
	// glutMouseFunc(mouse);
	// glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, bar.barVertices);
	// glColorPointer(4, GL_FLOAT, 0, colors);
	glColorPointer(3, GL_FLOAT, 0, bar.barColors);
	// glColor3f(1.0, 1.0, 1.0);
	audio.song.setVolume(30);
    audio.song.play();
	glutMainLoop();
	// sf::SoundStream::stop()
	
	return 0;

}
