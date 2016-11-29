attribute vec4 Position;
attribute vec4 SourceColor;
uniform vec2 Translation;
uniform vec2 Scale;
uniform float Rotation;

varying vec4 DestinationColor;

mat4 TranslationMatrix = mat4(1.0, 0.0, 0.0, Translation.x,
	                          0.0, 1.0, 0.0, Translation.y,
	                          0.0, 0.0, 1.0, 0.0,
	                          0.0, 0.0, 0.0, 1.0);

mat4 RotationMatrix = mat4(cos(Rotation), -sin(Rotation), 0.0, 0.0,
	                       sin(Rotation), cos(Rotation), 0.0, 0.0,
	                       0.0, 0.0, 1.0, 0.0,
	                       0.0, 0.0, 0.0, 1.0);

mat4 ScaleMatrix = mat4(Scale.x, 0.0, 0.0, 0.0,
	                    0.0, Scale.y, 0.0, 0.0,
	                    0.0, 0.0, 1.0, 0.0,
	                    0.0, 0.0, 0.0, 1.0);

void main(void) {
    DestinationColor = SourceColor;
    gl_Position = Position*ScaleMatrix*RotationMatrix*TranslationMatrix;
}