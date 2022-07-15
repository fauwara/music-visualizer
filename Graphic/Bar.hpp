#include <GL/glut.h>


class Bar {
	
	public:
		const int numBar = 20; // number of bars
		float scaleBar; // value to scale the bars
		// float barScale[];

		GLfloat barVertices[8] = {
			-0.90,  0.05,
			-0.95,  0.05,
			-0.95, -0.05,
			-0.90, -0.05,
		};

		// color format: CMY
		GLfloat barColors[12] = {
			// 0.6275, 0.7059, 0.2863, // purple
			// 0.6275, 0.7059, 0.2863, 
			// 0.1451, 0.0000, 0.5098, // green
			// 0.1451, 0.0000, 0.5098,
			0.2314, 0.2824, 0.2039, // light purple
			0.2314, 0.2824, 0.2039,
			0.4078, 0.0824, 0.1765, // mint bllue
			0.4078, 0.0824, 0.1765,
		};

		// bar indices
		GLubyte barIndices[4] = {
			0, 1, 
			2, 3,
		};

		// initialize barScale
		// void initBarScale() {
		// 	for ( int i = 0; i < numBar; i++) {
		// 		barScale[i] = 0;
		// 	}
		// }

	
};
