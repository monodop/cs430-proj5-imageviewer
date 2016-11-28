//
// Created by Harrison on 10/11/2016.
//

#include "../headers/color.h"

Color color_blend(Color a, Color b, BLEND_MODE blendMode) {
    switch (blendMode) {
        case BLEND_ADD:
            return (Color) {
                    .r = a.r + b.r,
                    .g = a.g + b.g,
                    .b = a.b + b.b,
					.a = a.a + b.a
            };
        case BLEND_SUBTRACT:
            return (Color) {
                    .r = a.r - b.r,
                    .g = a.g - b.g,
                    .b = a.b - b.b,
					.a = a.a - b.a
            };
        case BLEND_MULTIPLY:
            return (Color) {
                    .r = a.r * b.r,
                    .g = a.g * b.g,
                    .b = a.b * b.b,
					.a = a.a * b.a
            };
        default:
            return a;
    }
}

Color color_scale(Color a, double scale) {
    return (Color) {
            .r = a.r * scale,
            .g = a.g * scale,
            .b = a.b * scale,
			.a = a.a * scale
    };
}
