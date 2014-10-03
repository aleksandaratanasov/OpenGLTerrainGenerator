/*
 * Colouring.cpp
 *
 *  Created on: Feb 23, 2014
 *      Author: rbaleksandar
 */


#include "../include/Colouring.h"

	ColourModel::ColourModel(){
	}

	ColourModel::ColourModel(float max, float min, float height){
		maxHeight = max;
		minHeight = min;
		this->height = height;
	}

	ColourModel::~ColourModel(){
	}

	float* ColourModel::getColourForHeight5()
	{
		float scale = maxHeight - minHeight;
		float step = scale / 5;

		if(height > (maxHeight - step)){
			colour[0] = 1.0f;
			colour[1] = 0.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (2 * step))){
			colour[0] = 1.0f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (3 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (4 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 1.0f;
		}else {
			colour[0] = 0.0f;
			colour[1] = 0.0f;
			colour[2] = 1.0f;
		}
		return colour;
	}

	float* ColourModel::getColourForHeight9(){
		float scale = maxHeight - minHeight;
		float step = scale / 10;

		if(height > (maxHeight - step)){
			colour[0] = 1.0f;
			colour[1] = 0.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (2 * step))){
			colour[0] = 1.0f;
			colour[1] = 0.5f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (3 * step))){
			colour[0] = 1.0f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (4 * step))){
			colour[0] = 0.5f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (5 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (6 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 0.5f;
		}else if (height > (maxHeight - (7 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 1.0f;
		}else if (height > (maxHeight - (8 * step))){
			colour[0] = 0.0f;
			colour[1] = 0.5f;
			colour[2] = 1.0f;
		}else {
			colour[0] = 0.0f;
			colour[1] = 0.0f;
			colour[2] = 1.0f;
		}
		return colour;
	}

	float* ColourModel::getColourForHeight17(){
		float scale = maxHeight - minHeight;
		float step = scale / 17;

		if(height > (maxHeight - step)){
			colour[0] = 1.0f;
			colour[1] = 0.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (2 * step))){
			colour[0] = 1.0f;
			colour[1] = 0.25f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (3 * step))){
			colour[0] = 1.0f;
			colour[1] = 0.5f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (4 * step))){
			colour[0] = 1.0f;
			colour[1] = 0.75f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (5 * step))){
			colour[0] = 1.0f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (6 * step))){
			colour[0] = 0.75f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (7 * step))){
			colour[0] = 0.5f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (8 * step))){
			colour[0] = 0.25f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (9 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 0.0f;
		}else if (height > (maxHeight - (10 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 0.25f;
		}else if (height > (maxHeight - (11 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 0.5f;
		}else if (height > (maxHeight - (12 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 0.75f;
		}else if (height > (maxHeight - (13 * step))){
			colour[0] = 0.0f;
			colour[1] = 1.0f;
			colour[2] = 1.0f;
		}else if (height > (maxHeight - (14 * step))){
			colour[0] = 0.0f;
			colour[1] = 0.75f;
			colour[2] = 1.0f;
		}else if (height > (maxHeight - (15 * step))){
			colour[0] = 0.0f;
			colour[1] = 0.5f;
			colour[2] = 1.0f;
		}else if (height > (maxHeight - (16 * step))){
			colour[0] = 0.0f;
			colour[1] = 0.25f;
			colour[2] = 1.0f;
		}else {
			colour[0] = 0.0f;
			colour[1] = 0.0f;
			colour[2] = 1.0f;
		}
		return colour;
	}

	void ColourModel::setHeight(float height)
	{
		this->height = height;
	}

	void ColourModel::setMaxHeight(float max){
		maxHeight = max;
	}

	void ColourModel::setMinHeight(float min){
		minHeight = min;
	}

