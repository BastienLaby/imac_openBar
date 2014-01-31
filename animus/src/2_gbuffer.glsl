#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

uniform float Time;
uniform int RenderLightModel;
uniform int SpectrumOffset;

out vec2 uv;
out vec3 normal;
out vec3 position;

void main(void)
{
	vec4 new_position;
	if (RenderLightModel == 1)
		new_position = vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1);
	else
		new_position = vec4(VertexPosition.x + SpectrumOffset, VertexPosition.y + gl_InstanceID, VertexPosition.z, 1);
	
	uv = VertexTexCoord;
	normal = vec3(Object * vec4(VertexNormal, 1.0));
	position = vec3(Object * new_position);

	gl_Position = Projection * View * Object * new_position;
}

#endif

#if defined(FRAGMENT)
uniform vec3 CameraPosition;
uniform float Time;

in vec2 uv;
in vec3 position;
in vec3 normal;



uniform sampler2D Diffuse;
uniform sampler2D Spec;

out vec4  Color;
out vec4  Normal;

void main(void)
{
	Color = vec4(texture(Diffuse, uv).rgb, texture(Spec, uv).x);
	Normal = vec4(normal, 1);
}

#endif
