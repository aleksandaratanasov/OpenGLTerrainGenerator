#ifndef COLOURING_H_INCLUDED
#define COLOURING_H_INCLUDED

class ColourModel{
private:
	float height;
	float maxHeight;
	float minHeight;
	float colour[3];
public:
	ColourModel();

	ColourModel(float max, float min, float height);

	~ColourModel();

	// Colouring variations
	// TODO Create those automatically using some gradient algorithm for the RGB values

	float* getColourForHeight5();

	float* getColourForHeight9();

	float* getColourForHeight17();

	void setMaxHeight(float max);

	void setMinHeight(float min);

	void setHeight(float height);
};

#endif
