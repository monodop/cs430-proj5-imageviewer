attribute vec4 Position;
attribute vec4 SourceColor;
uniform vec2 Translation;

varying vec4 DestinationColor;

mat4 TranslationMatrix = mat4(1.0, 0.0, 0.0, Translation.x,
	                          0.0, 1.0, 0.0, Translation.y,
	                          0.0, 0.0, 1.0, 0.0,
	                          0.0, 0.0, 0.0, 1.0);

void main(void) {
    DestinationColor = SourceColor;
    gl_Position = Position*TranslationMatrix;
}