#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <fftw3.h>
#include <GL/glut.h>
#include <SFML/System.hpp>

#include "Audio.hpp"
#include "Graphic/Bar.hpp"


// const char *audioFilePath = "Audio/freq-test.wav";


// clock
sf::Clock clock_;
sf::Time timeElapsed = sf::milliseconds(0);
sf::Time fps_delay = sf::milliseconds(33);
sf::Time song_delay = sf::milliseconds(500);
// sf::Time delay;

// initializing custom Audio class and loading song into buffer
std::string audioFilePath = "Audio/aero.wav";
// std::string audioFilePath = "Audio/freq-test.wav";
Audio audio;


// inintializing some OpenGL primitives
Bar bar;


// initializing some fft variables
int real = 0;
int imag = 1;

const int freqBand = 30;
const int chunkSize = 2947;
int chunksCovered = 0;

int tempAmpFreq = 0;
int powerSpectrum[chunkSize];
int powerSpectrumSmoothed[freqBand];

int maxFreq = 0;
int overallMaxFreq = 0;
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
	
	for (int i = 0; i < chunkSize && i+chunksCovered < audio.sampleCount; i++) {
		input[i][real] = audio.samples[chunksCovered+i];
		input[i][imag] = 0;
	}

	fftw_plan plan = fftw_plan_dft_1d(chunkSize, input, output, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);

	fftw_destroy_plan(plan);
	fftw_cleanup();


	for (int i = 0; i < chunkSize && i+chunksCovered < audio.sampleCount; i++)
		powerSpectrum[i] = sqrt(pow(output[i+1][real], 2.0) + pow(output[i+1][imag], 2.0));


	for (int i = 0; i<freqBand; i++){
		powerSpectrumSmoothed[i] = 0;
		for (int j = 0; j<chunkSize && j+chunksCovered < audio.sampleCount; j++){
			powerSpectrumSmoothed[i] += powerSpectrum[j]; 
		}
		powerSpectrumSmoothed[i] = powerSpectrum[i]/chunkSize;

		if (powerSpectrumSmoothed[i] > maxFreq){
			maxFreq = powerSpectrumSmoothed[i];
		}
	}

	chunksCovered += chunkSize;

}


// prolly not the most cleanest way to find max amp of freq in the audio file, but meh
void findMaxFreq() {

	for (int c = 0; c < (audio.sampleCount/chunkSize); c++) {
		updateFFTInput();

		if ( overallMaxFreq < maxFreq) {
			overallMaxFreq = maxFreq;
		}
	}
	chunksCovered = 0;

}

void displayBars() {

	for (int i = 1; i<freqBand; i++) {

		// glPushMatrix();
		glTranslatef(0.06, 0.0, 0.0);
		glPushMatrix();
		// scaleBar = normalize(powerSpectrumSmoothed[i], minFreq, overallMaxFreq, 4.5);
		bar.scaleBar = normalize(powerSpectrumSmoothed[i], minFreq, maxFreq, 4.5);
		glScalef(1.0, bar.scaleBar, 1.0);
		glDrawElements(GL_POLYGON, 8, GL_UNSIGNED_BYTE, bar.barIndices);
		glPopMatrix();
		// glPopMatrix();

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

	updateFFTInput(); // gets new fft values
	// fps_delay.asMilliseconds() - clock_.getElapsedTime().asMilliseconds();
	sf::sleep(fps_delay - clock_.getElapsedTime());
	clock_.restart();
	// scaleBars();
	glutPostRedisplay();
	// std::cout << freqBandArray[0] << std::endl;

}



int main(int argc, char **argv) {
	

	// load raw audio data onto buffer
	audio.loadBuffer(audioFilePath);

	std::cout << "Sample Rate: " << audio.sampleRate << std::endl;
	std::cout << "Sample Count: " << audio.sampleCount << std::endl;

	findMaxFreq();
	std::cout << "Max Freq: " << overallMaxFreq << std::endl;

	// return 0;

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
	sf::sleep(song_delay);
	glutMainLoop();
	// sf::SoundStream::stop()
	
	return 0;

}
