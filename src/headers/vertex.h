#pragma once

#include "vec.h"
#include "color.h"

typedef struct {
	Vector position;
	Color color;
	float texture_coordinates[2];
} Vertex;