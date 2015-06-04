#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in float typeInput[];
in vec3 posInput[];
in vec3 velInput[];
in vec3 accInput[];
in float lifeInput[];

out float typeOut;
out vec3 posOut;
out vec3 velOut;
out vec3 accOut;
out float lifeOut;

uniform float deltaTime;
uniform float fullTime;
uniform sampler1D randSampler;
uniform float emitterLife;
uniform float shellLife;
uniform float shellSecondLife;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

vec3 GetRandomDir(float TexCoord)
{
     vec3 Dir = texture(randSampler, TexCoord).xyz;
     Dir -= vec3(0.5, 0.5, 0.5);
     return Dir;
}

void main()
{
    float Age = lifeInput[0] + deltaTime;

    if (typeInput[0] == PARTICLE_TYPE_LAUNCHER) {
        if (Age >= emitterLife) {
            typeOut = PARTICLE_TYPE_SHELL;
            posOut = posInput[0];
            vec3 Dir = GetRandomDir(fullTime/1000.0);
            Dir.y = max(Dir.y, 0.5);
            velOut = normalize(Dir) / 20.0;
            lifeOut = 0.0;
            EmitVertex();
            EndPrimitive();
            Age = 0.0;
        }

        typeOut = PARTICLE_TYPE_LAUNCHER;
        posOut = posInput[0];
        velOut = velInput[0];
        lifeOut = Age;
        EmitVertex();
        EndPrimitive();
    }
    else {
        float DeltaTimeSecs = deltaTime;
        float t1 = lifeInput[0];
        float t2 = Age;
        vec3 DeltaP = DeltaTimeSecs * velInput[0];
        vec3 DeltaV = vec3(DeltaTimeSecs) * (0.0, -9.81, 0.0);

        if (typeInput[0] == PARTICLE_TYPE_SHELL)  {
                if (Age < shellLife) {
                    typeOut = PARTICLE_TYPE_SHELL;
                    posOut = posInput[0] + DeltaP;
                    velOut = velInput[0] + DeltaV;
                    lifeOut = Age;
                    EmitVertex();
                    EndPrimitive();
                }
            else {
                for (int i = 0 ; i < 10 ; i++) {
                     typeOut = PARTICLE_TYPE_SECONDARY_SHELL;
                     posOut = posInput[0];
                     vec3 Dir = GetRandomDir((fullTime + i)/1000.0);
                     velOut = normalize(Dir) / 20.0;
                     lifeOut = 0.0f;
                     EmitVertex();
                     EndPrimitive();
                }
            }
        }
        else {
            if (Age < shellSecondLife) {
                typeOut = PARTICLE_TYPE_SECONDARY_SHELL;
                posOut = posInput[0] + DeltaP;
                velOut = velInput[0] + DeltaV;
                accOut = accInput[0];
                lifeOut = Age;
                EmitVertex();
                EndPrimitive();
            }
        }
    }
}
