#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <fftw3.h>
#include <GL/glut.h>
#include <SFML/System.hpp>

#include "Audio.hpp"
#include "Graphic/Bar.hpp"
#include "Graphic/Stars.hpp"
#include "Graphic/Sky.hpp"


// const char *audioFilePath = "Audio/freq-test.wav";


// clock
sf::Clock clock_;
sf::Time timeElapsed = sf::milliseconds(0);
sf::Time fps_delay = sf::milliseconds(33.33);
sf::Time song_delay = sf::milliseconds(950);
// sf::Time delay;

// initializing custom Audio class and loading song into buffer
// std::string audioFilePath = "Audio/aero-trim.wav";
// std::string audioFilePath = "Audio/aero.wav";
std::string audioFilePath = "Audio/eden-xo.wav";
// std::string audioFilePath = "Audio/freq-test.wav";
Audio audio;


// inintializing some OpenGL primitives
Bar bar;
const int barCount = 30;

Star star;

Sky sky;

// initializing some fft variables
int real = 0;
int imag = 1;


// const int chunkSize = 2947; // aero
const int chunkSize = 3200; // eden-xo
int chunksCovered = 0;


int tempAmpFreq = 0;
int powerSpectrum[chunkSize];
// int powerSpectrumSmoothed = new int[count];
int powerSpectrumSmoothed[barCount];

float maxChunkFreq = 0;
float minChunkFreq = 0;
float maxFreq = 0;
float minFreq = 0;

int avgFreq = 0;

fftw_complex input[chunkSize]; // double x[n][2];
fftw_complex output[chunkSize];


float normalize(float num, float min, float max, float maximus){
	// zi = (xi – min(x)) / (max(x) – min(x))
	return ( (num-min) / (max-min) )*maximus;
}



// gets new fft values
void updateFFTInput() {

	maxChunkFreq = 0;
	
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


	for (int i = 0; i<barCount; i++){
		powerSpectrumSmoothed[i] = 0;
		for (int j = 0; j<chunkSize && j+chunksCovered < audio.sampleCount; j++){
			powerSpectrumSmoothed[i] += powerSpectrum[j]; 
		}
		powerSpectrumSmoothed[i] = powerSpectrum[i]/chunkSize;

		if (powerSpectrumSmoothed[i] > maxChunkFreq){
			maxChunkFreq = powerSpectrumSmoothed[i];
		}
	}

	chunksCovered += chunkSize;

}


// prolly not the most cleanest way to find max amp of freq in the audio file, but meh
void findMaxFreq() {

	for (int c = 0; c < (audio.sampleCount/chunkSize); c++) {
		updateFFTInput();

		if ( maxFreq < maxChunkFreq) {
			maxFreq = maxChunkFreq;
		}
	}
	chunksCovered = 0;

}

void displayBars() {

	glVertexPointer(2, GL_FLOAT, 0, bar.vertices);
	glColorPointer(3, GL_FLOAT, 0, bar.colors);
	
	glPushMatrix();
	for (int i = 1; i<barCount; i++) {

		glTranslatef(0.06, 0.0, 0.0);
	
		glPushMatrix();
			// bar.scaleBar = normalize(powerSpectrumSmoothed[i], minFreq, maxFreq, 4.5);
			bar.scaleBar = normalize(powerSpectrumSmoothed[i], minChunkFreq, maxChunkFreq, 4.5);
			glScalef(1.0, bar.scaleBar, 1.0);
			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, bar.indices);
		glPopMatrix();

	}
	glPopMatrix();

}

void displayStars() {

	glVertexPointer(2, GL_FLOAT, 0, star.vertices);
	
	if ( maxChunkFreq > 0.35*maxFreq ) {
		glColorPointer(4, GL_FLOAT, 0, star.colorsOn);
		// std::cout << maxChunkFreq << " " << 0.35*maxFreq << std::endl;
	}
	else
		glColorPointer(4, GL_FLOAT, 0, star.colorsOff);
	
	glPointSize(star.size);

	// glTranslatef( star.displaceX, star.displaceY, 0.0 );
	glPushMatrix();
	for ( float y = -0.9; y <= 1; y += 0.1) {
		for ( float x = -0.9; x <= 1; x += 0.1) {

			glRotatef( 1000, 0.0, 0.0, 1.0);
			glRotatef( star.displace, 0.0, 0.0, 1.0);
			glPushMatrix();
				glTranslatef( x, y, 0.0 );
				glDrawElements(GL_POINTS, 1, GL_UNSIGNED_BYTE, star.indices);
			glPopMatrix();
			
		}
	}
	glPopMatrix();

}

void rotateStars() {
	star.displace += normalize( maxChunkFreq, 0, maxFreq, 0.1);
	// star.displace += 0.01;
	// star.displaceY += 0.01;
}


void drawSky() {
	glVertexPointer(2, GL_FLOAT, 0, sky.vertices);
	glColorPointer(3, GL_FLOAT, 0, sky.colors);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, sky.indices);
}



void display() {
	glClearColor( 1.0, 1.0, 1.0, 1.0 ); // Background Color
	// std::cout << "HELLO" << std::endl;
	glClear( GL_COLOR_BUFFER_BIT );
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// glScalef( normFloat, normFloat, normFloat );
	// glScalef( j, j, j );



	// if ( maxChunkFreq > (9/10)*maxFreq )
	drawSky();
	displayStars();

	displayBars();
	// std::cout << "HELLO" << std::endl;

	glFlush();
	glutSwapBuffers();
	// std::cout << "HELLO" << std::endl;
}



void idlefunc() {

	updateFFTInput(); // gets new fft values
	rotateStars();
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
	std::cout << "Audio Duration: " << audio.duration.asSeconds() << std::endl;

	findMaxFreq();
	std::cout << "Max Freq: " << maxFreq << std::endl;

	// return 0;

	// initial filling of fft input array
	updateFFTInput();
	// std::cout << "done";
	
	
	// initializing OpenGL
	glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize(500, 500);
	glutCreateWindow("MUSIC VISUALIZER");
	// glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(idlefunc);
	// glutMouseFunc(mouse);
	// glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	// glColorPointer(4, GL_FLOAT, 0, colors);

	// glColor3f(1.0, 1.0, 1.0);
	audio.song.setVolume(30);
	audio.song.play();
	sf::sleep(song_delay);

	glutMainLoop();
	// sf::SoundStream::stop()
	
	return 0;

}