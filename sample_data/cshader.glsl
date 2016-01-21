#version 430 core
#extension GL_EXT_gpu_shader4 : enable

const int enum_solid = 1;

const ivec4 border_color = ivec4( 0); //hacky
const int tree_size = 16;

layout(local_size_x=16,local_size_y=16) in;

uniform isampler3D treeSource;

uniform writeonly image2D fbTarget;

layout(std430,binding=0) buffer IndexContainerL1
{
    uint idx[];
} indices_l1;
layout(std430,binding=1) buffer IndexContainerL2
{
    uint idx[];
} indices_l2;
layout(std430,binding=2) buffer ArrayContainer
{
    float val[];
} array_l0;

uniform vec3 cPos;
uniform vec3 rayBL; /*0,0*/
uniform vec3 rayTL; /*0,1*/
uniform vec3 rayBR; /*1,0*/
uniform vec3 rayTR; /*1,1*/

float raycast(in vec3 p, in vec3 d)
{
    float t = 0;

    if(p.x < 0.0) t = max(t,(-p.x)/d.x);
    if(p.y < 0.0) t = max(t,(-p.y)/d.y);
    if(p.z < 0.0) t = max(t,(-p.z)/d.z);

    if(p.x > 1.0) t = max(t,(1.0-p.x)/d.x);
    if(p.y > 1.0) t = max(t,(1.0-p.y)/d.y);
    if(p.z > 1.0) t = max(t,(1.0-p.z)/d.z);

    t += 1e-5;

    vec3 cubeloc = p+t*d;
//    ivec3 volsize = textureSize3D(treeSource,0);
    ivec3 volsize = ivec3(512);

    ivec3 samloc = ivec3(cubeloc*volsize);
    vec3 d_sign = vec3(d.x>=0, d.y>=0, d.z>=0);

    vec3 dist = ((vec3(samloc)+d_sign)/volsize-p)/d;

    return float(cubeloc.x);

    int i = 0;
    while(all(greaterThanEqual(samloc,ivec3(0))) && all(lessThan(samloc,volsize)) &&i++<100)
    {
	ivec4 value = texture(treeSource,samloc);
	if(value.r == enum_solid || true)
	    return 1.0;

	if(dist.x < dist.y && dist.x < dist.z)
	{
	    t = dist.x;
	    float dir = d_sign.x*2.0-1.0;
	    dist.x += dir/(d.x*volsize.x);
	    samloc.x += int(dir);
	}else if(dist.y < dist.z)
	{
	    t = dist.y;
	    float dir = d_sign.y*2.0-1.0;
	    dist.y += dir/(d.y*volsize.y);
	    samloc.y += int(dir);
	}else{
	    t = dist.z;
	    float dir = d_sign.z*2.0-1.0;
	    dist.z += dir/(d.z*volsize.z);
	    samloc.z += int(dir);
	}
    }

    return t*1000-5.5;
}

float spherecast(in vec3 p, in vec3 d)
{
    p = vec3(0.5)-p;
    float pp = dot(p,p);
    float pd = dot(p,d);
    float dd = dot(d,d);
    float sq = pd*pd+dd*(0.4*0.4-pp);
    if(sq < 0) return 0;
    return ((pd-sqrt(sq))/dd)*1000-5;
}

float lookup(in vec3 p)
{
    return 0.0;
}

void main(void){
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    ivec2 offset = ivec2(pix-gl_NumWorkGroups.xy/2);
    vec3 dir = vec3(vec2(offset),2000.0);
    vec4 color = vec4(vec3(raycast(cPos,dir)),1.0);
    imageStore(fbTarget,pix,color);
}
