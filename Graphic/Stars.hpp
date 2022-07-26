#pragma once

#include <GL/glut.h>


class Star {
	
	public:

		float size = 3;
		float displace = 0;
		// float displaceY = 0;

		GLfloat vertices[2] = {
			0.0, 0.0,
		};

		// color format: CMY
		GLfloat colorsOff[3] = {
			0.2314, 0.2824, 0.2039,
		};

		GLfloat colorsOn[4] = {
			1.0, 0.0, 0.0, 1.0
		};

		// bar indices
		GLubyte indices[2] = {
			0, 1, 
		};

		// initialize barScale
		// void initBarScale() {
		// 	for ( int i = 0; i < numBar; i++) {
		// 		barScale[i] = 0;
		// 	}
		// }

	
};
