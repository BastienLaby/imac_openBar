#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

void main(void)
{
	gl_Position = Projection * View * Object * vec4(VertexPosition, 1.0);
}

#endif

#if defined(FRAGMENT)
uniform vec3 CameraPosition;

out vec4 Color;

void main(void)
{
	Color = vec4(1.0, 0.0, 0.0, 1.0);
}

#endif
