#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <string.h>
#include <fftw3.h> // fast fourier transform
#include <GL/glut.h> //
#include <SFML/System.hpp>
#include "stb/stb_image.h"

#include "Audio.hpp"
#include "Graphic/Bar.hpp"
#include "Graphic/Stars.hpp"
#include "Graphic/DescriptionPage.hpp"


// const char *audioFilePath = "Audio/freq-test.wav";




// clock
sf::Clock clock_;
sf::Time timeElapsed = sf::milliseconds(0);
sf::Time fps_delay = sf::milliseconds(33.33);
// sf::Time song_delay = sf::milliseconds(500);
sf::Time song_delay = sf::milliseconds(450); //nito
// sf::Time song_delay = sf::milliseconds(300); //tabla
// sf::Time delay;

// initializing custom Audio class and loading song into buffer
// std::string audioFilePath = "Audio/aero-trim.wav";
// std::string audioFilePath = "Audio/aero.wav";
// std::string audioFilePath = "Audio/eden-xo.wav";
std::string audioFilePath = "Audio/ichika-nito.wav";
// std::string audioFilePath = "Audio/vocals.wav";
// std::string audioFilePath = "Audio/tabla.wav";
// std::string audioFilePath = "Audio/freq-test.wav";
Audio audio;


// inintializing some OpenGL primitives
Bar bar;
const int barCount = 30;
// const int barCount = 40;

Star star;

// float bgColor[3] = { 0.8666, 0.8431, 0.8352 } ;
float bgColor[3] = { 0.9725, 0.9607, 0.9607 } ;

DescriptionPage dp;

// Sky sky;

// OpenGL Scene

int scene = 0;



// initializing some fft variables
int real = 0;
int imag = 1;


// const int chunkSize = 2939; // tabla
// const int chunkSize = 2947; // aero
const int chunkSize = 3200; // eden-xo, ichkia
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


void updatePowerSpectrum() {
	
	maxChunkFreq = 0; minChunkFreq = 10000;
	
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
		if (powerSpectrumSmoothed[i] < minChunkFreq){
			minChunkFreq = powerSpectrumSmoothed[i];
		}
	}

	chunksCovered += chunkSize;

}

// prolly not the most cleanest way to find max amp of freq in the audio file, but meh
void findMaxFreq() {

	for (int c = 0; c < (audio.sampleCount/chunkSize); c++) {
		updatePowerSpectrum();

		if ( maxFreq < maxChunkFreq) {
			maxFreq = maxChunkFreq;
		}
	}
	chunksCovered = 0;

}

void displayBars() {

	glVertexPointer(2, GL_FLOAT, 0, bar.vertices);
	glColorPointer(3, GL_FLOAT, 0, bar.colorsLow);
	
	glPushMatrix();
	for (int i = 1; i<barCount; i++) {

		glTranslatef( bar.barGap, 0.0, 0.0);
	
		glPushMatrix();
			// bar.scaleBar = normalize(sqrt(powerSpectrumSmoothed[i]), minFreq, sqrt(maxFreq), 5);
			bar.scaleBar = normalize(powerSpectrumSmoothed[i], minChunkFreq, maxChunkFreq, bar.maxScale);

			// if ( bar.scaleBar <= 0.75 )
			// 	bar.scaleBar = 0.75;

			// bar.scaleBar -= 0.75;
			// std::cout << bar.scaleBar << std::endl;
			// bar.scaleBar = sqrt( ( 4/25 ) * ( 25 - pow( bar.scaleBar-5 , 2)) ) * 2.5;
			// std::cout << bar.scaleBar << std::endl;
			// bar.scaleBar = normalize(powerSpectrumSmoothed[i], minFreq, maxFreq, bar.maxScale);
			glScalef(1.0, bar.scaleBar, 1.0);
			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, bar.indices);
		glPopMatrix();

	}
	glPopMatrix();

}

void displayStars() {

	glVertexPointer(2, GL_FLOAT, 0, star.vertices);
	
	// if ( maxChunkFreq > 0.35*maxFreq ) 
	// 	glColorPointer(3, GL_FLOAT, 0, star.colorsOn);
		// std::cout << maxChunkFreq << " " << 0.35*maxFreq << std::endl;
	// else
		glColorPointer(3, GL_FLOAT, 0, star.colorsOff);
	
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

void displayDescription() {

	glVertexPointer(2, GL_FLOAT, 0, dp.vertices);
	glColorPointer(3, GL_FLOAT, 0, dp.colors);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, dp.indices);

	glColor3f( 1.0, 1.0, 1.0 );
	std::string description[14] = {
		"Department of Computer Science and Engineering",
		"Sahyadri",
		"College of Engineering and Management",
		"Adyar, Mangaluru-575007",
		"2021 - 2022",
		"",
		"Music Visualizer",
		"by",
		"Fawaz Abid Hussain  - 4SF19CS064",
		"Vinol Chris D'Souza - 4SF19CS186",
		"",
		"Computer Graphics Laboratory with Mini Project",
		"Under the Guidance of",
		"Ms. Megha Rani R.",
	};

	std::string instructions = "Press SPACE to begin.";


	float rasterPosX = -0.5;
	float rasterPosY =  0.5;

	for (int y = 0; y < 14; y++, rasterPosY -= 0.075 ) {

		glRasterPos2f( rasterPosX, rasterPosY );
		for( int x = 0; x < description[y].length(); x++){
			glutBitmapCharacter( GLUT_BITMAP_8_BY_13, description[y][x]);
		}
	
	}

	// glColorPointer(3, GL_FLOAT, 0, dp.colors);
	glColor3f( 0.2314, 0.2824, 0.2039 );
	glRasterPos2f( -0.24, rasterPosY - 0.31 );
	for( int x = 0; x < instructions.length(); x++){
		glutBitmapCharacter( GLUT_BITMAP_8_BY_13, instructions[x]);
	}
	
}


void displayMenuPage() {

	glVertexPointer(2, GL_FLOAT, 0, dp.vertices);
	glColorPointer(3, GL_FLOAT, 0, dp.colors);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, dp.indices);

	glColor3f( 1.0, 1.0, 1.0 );
	std::string menuPage[6] = {
		"Choose the audio file:",
		"",
		"1. tabla.wav",
		"2. guitar.wav",
		"3. vocals.wav",
		"4. eden-xo.wav",
	};

	// std::string instructions = "Press SPACE to begin.";


	float rasterPosX = -0.5;
	float rasterPosY =  0.125;

	for (int y = 0; y < 5; y++, rasterPosY -= 0.075 ) {

		glRasterPos2f( rasterPosX, rasterPosY );
		for( int x = 0; x < menuPage[y].length(); x++){
			glutBitmapCharacter( GLUT_BITMAP_8_BY_13, menuPage[y][x]);
		}
	
	}

	// glColorPointer(3, GL_FLOAT, 0, dp.colors);
	// glColor3f( 0.2314, 0.2824, 0.2039 );
	// glRasterPos2f( -0.24, rasterPosY - 0.31 );
	// for( int x = 0; x < instructions.length(); x++){
	// 	glutBitmapCharacter( GLUT_BITMAP_8_BY_13, instructions[x]);
	// }

	
}

void rotateStars() {
	star.displace += normalize( maxChunkFreq, 0, maxFreq, 0.04);
	// star.displace += 0.01;
	// star.displaceY += 0.01;
}


void initMusic( std::string audioFile) {
				
	audio.loadBuffer(audioFile);

	std::cout << "Sample Rate: " << audio.sampleRate << std::endl;
	std::cout << "Sample Count: " << audio.sampleCount << std::endl;
	std::cout << "Audio Duration: " << audio.duration.asSeconds() << std::endl;

	findMaxFreq();
	std::cout << "Max Freq: " << maxFreq << std::endl;
					
	audio.song.setVolume(30);
	audio.song.play();
	audio.played = 1;
	
	sf::sleep(song_delay);

}


void display() {

	glClearColor( bgColor[0], bgColor[1], bgColor[2],  1.0 );
	glClear( GL_COLOR_BUFFER_BIT );

	glLoadIdentity();
	
	if ( scene == 0 ) {

		displayStars();
		displayDescription();
	
	} else if ( scene == 1 ) {
		
		displayStars();
		displayMenuPage();

	} else if ( scene == 2 ) {
			// audio.loadBuffer(audioFilePath);

		displayStars();
		displayBars();	

		if (!audio.played) {

			initMusic("Audio/ichika-nito.wav");

		}

	}




	// glLoadIdentity();

	glFlush();
	glutSwapBuffers();
}


void keys(unsigned char k,int x,int y) {
	
	if ( k == ' ' ) scene = 1;
	if ( k == '1' ) scene = 2;
	if ( k == '2' ) scene = 3;
	if ( k == '3' ) scene = 4;
	glutPostRedisplay();

}

void idlefunc() {

	if ( scene > 1 ) {

		updatePowerSpectrum(); // gets new fft values
		rotateStars();
		// fps_delay.asMilliseconds() - clock_.getElapsedTime().asMilliseconds();
		sf::sleep(fps_delay - clock_.getElapsedTime());
		clock_.restart();
		// scaleBars();
	
	}

	glutPostRedisplay();
	// std::cout << freqBandArray[0] << std::endl;

}



int main(int argc, char **argv) {
	

	// load raw audio data onto buffer
	// audio.loadBuffer(audioFilePath);




	// return 0;

	// initial filling of fft input array
	updatePowerSpectrum();
	// std::cout << "done";
	
	
	// initializing OpenGL
	glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize(900, 900);
	glutCreateWindow("MUSIC VISUALIZER");
	// glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(idlefunc);
	// glutMouseFunc(mouse);
	// glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glutKeyboardFunc(keys);
	// glEnableClientState(GL_POLYGON_STIPPLE);

	// glColorPointer(4, GL_FLOAT, 0, colors);

	// glColor3f(1.0, 1.0, 1.0);
	


	glutMainLoop();
	// sf::SoundStream::stop()

	
	return 0;

}