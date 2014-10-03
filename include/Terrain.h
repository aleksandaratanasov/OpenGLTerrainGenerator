/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */

/*
 * Modifications: Aleksandar, Fatih and Viktor (HS Karlsruhe Autonome Systems Lab) :-3
 */

#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include "vec3f.h"
#include "imageloader.h"

#define TERRAIN_HEIGHT 60

class Terrain
{
private:
	int w; //Width
	int l; //Length
	float** hs; //Heights
	Vec3f** normals;
	bool computedNormals; //Whether normals is up-to-date
public:
	Terrain(int w2, int l2);
	~Terrain();
		
	int width();		
	int length();
		
	//Sets the height at (x, z) to y
	void setHeight(int x, int z, float y);

	//Returns the height at (x, z)
	float getHeight(int x, int z);
		
	//Computes the normals, if they haven't been computed yet - needs optimization
	void computeNormals();
		
	//Returns the normal at (x, z)
	Vec3f getNormal(int x, int z);

	//Loads a terrain from a heightmap. The heights of the terrain range from
	//-height / 2 to height / 2.
	static Terrain* loadTerrain(const char* filename, float height);

	//Loads a terrain from a cv::Mat's data (pixels, rows, cols).
	//Heights -> see loadTerrain(filename, height) above
	static Terrain* loadTerrain(unsigned char* imagePixelData, int imageWidthCols, int imageLengthRows, float height, bool invert);

	//Deletes allocated memory for terrain so that we can use the pointer again without any memory leaks
	static void cleanup(Terrain *terrain);
};
#endif
