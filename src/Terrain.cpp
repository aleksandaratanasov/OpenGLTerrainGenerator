#include "../include/Terrain.h"

Terrain::Terrain(int w2, int l2) {
	w = w2;
	l = l2;
			
	hs = new float*[l];
	for(int i = 0; i < l; i++) {
		hs[i] = new float[w];
	}
			
	normals = new Vec3f*[l];
	for(int i = 0; i < l; i++) {
		normals[i] = new Vec3f[w];
	}
			
	computedNormals = false;
}

Terrain::~Terrain(void)
{
	for(int i = 0; i < l; i++) {
		delete[] hs[i];
	}
	delete[] hs;
			
	for(int i = 0; i < l; i++) {
		delete[] normals[i];
	}
	delete[] normals;
}

int Terrain::width() {
	return w;
}

int Terrain::length() {
	return l;
}

void Terrain::setHeight(int x, int z, float y) {
	hs[z][x] = y;
	computedNormals = false;
}

float Terrain::getHeight(int x, int z) {
	return hs[z][x];
}

void Terrain::computeNormals() {
	if (computedNormals) {
		return;
	}
			
	//Compute the rough version of the normals
	Vec3f** normals2 = new Vec3f*[l];
	for(int i = 0; i < l; i++) {
		normals2[i] = new Vec3f[w];
	}
			
	for(int z = 0; z < l; z++) {
		for(int x = 0; x < w; x++) {
			Vec3f sum(0.0f, 0.0f, 0.0f);
					
			Vec3f out;
			if (z > 0) {
				out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
			}
			Vec3f in;
			if (z < l - 1) {
				in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
			}
			Vec3f left;
			if (x > 0) {
				left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
			}
			Vec3f right;
			if (x < w - 1) {
				right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
			}
					
			if (x > 0 && z > 0) {
				sum += out.cross(left).normalize();
			}
			if (x > 0 && z < l - 1) {
				sum += left.cross(in).normalize();
			}
			if (x < w - 1 && z < l - 1) {
				sum += in.cross(right).normalize();
			}
			if (x < w - 1 && z > 0) {
				sum += right.cross(out).normalize();
			}
					
			normals2[z][x] = sum;
		}
	}
			
	//Smooth out the normals
	const float FALLOUT_RATIO = 0.5f;
	for(int z = 0; z < l; z++) {
		for(int x = 0; x < w; x++) {
			Vec3f sum = normals2[z][x];
					
			if (x > 0) {
				sum += normals2[z][x - 1] * FALLOUT_RATIO;
			}
			if (x < w - 1) {
				sum += normals2[z][x + 1] * FALLOUT_RATIO;
			}
			if (z > 0) {
				sum += normals2[z - 1][x] * FALLOUT_RATIO;
			}
			if (z < l - 1) {
				sum += normals2[z + 1][x] * FALLOUT_RATIO;
			}
					
			if (sum.magnitude() == 0) {
				sum = Vec3f(0.0f, 1.0f, 0.0f);
			}
			normals[z][x] = sum;
		}
	}
			
	for(int i = 0; i < l; i++) {
		delete[] normals2[i];
	}
	delete[] normals2;
			
	computedNormals = true;
}
		
Vec3f Terrain::getNormal(int x, int z) {
	if (!computedNormals) {
		computeNormals();
	}
	return normals[z][x];
}

Terrain* Terrain::loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);

	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}

Terrain* Terrain::loadTerrain(unsigned char* imagePixelData, int imageWidthCols, int imageLengthRows, float height, bool invert)
{
	//NOTE: when using a normal web cam we convert our BGRA to RGB and than to GRAYSCALE. Sadly
	//it appears that for some reason OpenCV creates an image that stores again 3 channels but
	//all of them store the same grayscale so we have to skip pixel-data and read ONLY one cell
	//of the array (R, G or B). If we remove this we get 3 terrains
	Terrain* t = new Terrain(imageWidthCols/3, imageLengthRows/3);

	for(int y = 0; y < imageLengthRows/3; y++) {
		for(int x = 0; x < imageWidthCols/3; x++) {
			unsigned char color;
			if(!invert)
				color = imagePixelData[3 * (y * imageWidthCols + x)];
			else
				color = 255-imagePixelData[3 * (y * imageWidthCols + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}

	t->computeNormals();
	return t;
}

void Terrain::cleanup(Terrain *terrain)
{
	delete terrain;
}
