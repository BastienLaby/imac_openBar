#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;
uniform float Time;
uniform float Speed;
uniform int InstanceCount;
uniform float Espacement;
uniform float RotateRangeX;
uniform float RotateRangeY;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
    flat int instance;
}vertex;

void main(void)
{	

	
	gl_Position = Projection * View * vec4(VertexPosition.x + gl_InstanceID*Espacement,
									   VertexPosition.y + RotateRangeX*cos(Time+gl_InstanceID),
									   VertexPosition.z - RotateRangeY*sin(Time+gl_InstanceID),
									   1.0);


	vertex.uv = VertexTexCoord;
	vertex.normal = VertexNormal;
	vertex.position = VertexPosition;
	vertex.instance = gl_InstanceID;
}

#endif

#if defined(GEOMETRY)

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
    flat int instance;

}vertices[];

out fData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
    flat int instance;
}frag;    

void main()
{
	int i;
	for (i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = gl_in[i].gl_Position;
		frag.normal = vertices[i].normal;
		frag.position = vertices[i].position;
		frag.uv = vertices[i].uv;
		frag.instance = vertices[i].instance;
		EmitVertex();
	}
	EndPrimitive();
}

#endif

#if defined(FRAGMENT)
uniform vec3 CameraPosition;
uniform float Time;

in fData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
    flat int instance;
}frag;

out vec4 Color;

void main(void)
{
	float red = cos(32*frag.uv.x*Time);
	float green = sin(32*frag.uv.y*Time);
	float blue = 0.5;
	float alpha = 1.f;

	Color = vec4(red, green, blue, alpha);
}
#endif
