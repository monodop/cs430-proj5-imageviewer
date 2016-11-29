#pragma once

#include "vec.h"
#include "color.h"

typedef struct {
	float position[3];
	float color[4];
	float texture_coordinates[2];
} Vertex;