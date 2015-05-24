#version 330

uniform struct Matrices
{
	mat4 mProj;
	mat4 mView;
} matrices;

uniform vec3 vQuad1, vQuad2;

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

in vec3 vColorPass[];
in float fLifeTimePass[];
in float fSizePass[];
in int iTypePass[];

smooth out vec2 vTexCoord;
flat out vec4 vColorPart;

void main()
{
  if(iTypePass[0] != 0)
  {
    vec3 vPosOld = gl_in[0].gl_Position.xyz;
    float fSize = fSizePass[0];
    mat4 mVP = matrices.mProj*matrices.mView;
    
    vColorPart = vec4(vColorPass[0], fLifeTimePass[0]);
       
    vec3 vPos = vPosOld+(-vQuad1-vQuad2)*fSize;
    vTexCoord = vec2(0.0, 0.0);
    gl_Position = mVP*vec4(vPos, 1.0);
    EmitVertex();
    
    vPos = vPosOld+(-vQuad1+vQuad2)*fSize;
    vTexCoord = vec2(0.0, 1.0);
    gl_Position = mVP*vec4(vPos, 1.0);
    EmitVertex();
    
    vPos = vPosOld+(vQuad1-vQuad2)*fSize;
    vTexCoord = vec2(1.0, 0.0);
    gl_Position = mVP*vec4(vPos, 1.0);
    EmitVertex();
    
    vPos = vPosOld+(vQuad1+vQuad2)*fSize;
    vTexCoord = vec2(1.0, 1.0);
    gl_Position = mVP*vec4(vPos, 1.0);
    EmitVertex();
      
    EndPrimitive();
  }
}