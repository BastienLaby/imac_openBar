#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec2 uv;
out vec3 normal;
out vec3 position;

void main(void)
{	
	uv = VertexTexCoord;
	normal = vec3(Object * vec4(VertexNormal, 1.0));; 
	position = vec3(Object * vec4(VertexPosition, 1.0));
	position.y += (gl_InstanceID * 1.5); 
	gl_Position = Projection * View * vec4(position, 1.0);
}

#endif

#if defined(FRAGMENT)
uniform vec3 CameraPosition;
uniform float Time;

uniform int RenderMode;

in vec2 uv;
in vec3 position;
in vec3 normal;

uniform sampler2D Diffuse;
uniform sampler2D Spec;

out vec4  Color;
out vec4  Normal;

void main(void)
{
	/*if(RenderMode == 0)
	{
		Color = texture(Diffuse, uv);
	}
	else if(RenderMode == 1)
	{
		Color = vec4(position, 1);
	}
	else if(RenderMode == 2)
	{
		Color = vec4(normal, 1);
	}
	else if(RenderMode == 3)
	{
		spec = texture(Spec, uv);
		Color = vec4(spec.x, spec.x, spec.x, 1);
	}
	else
	{
		Color = vec4(1, 1, 1, 1);
	}*/

	Color = vec4(texture(Diffuse, uv).rgb, texture(Spec, uv).x);
	Normal = vec4(normal, 1);
	
}

#endif
