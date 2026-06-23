#version 100
precision mediump float;

/* ES2 legacy scenery (mod2) vertex shader: diffuse only, no skinning.
 * Identical model instances are batched into one draw: the per-object world
 * transforms are uploaded as a uniform array and indexed by the emulated
 * instance id (glw_InstanceID, set by the RHI per instance), so a run of the
 * same model is one program/texture bind instead of one per object. The array
 * size caps the batch (kept within the ES2 vertex-uniform budget); the CPU
 * side splits larger runs across submits. */
#define MODEL_BATCH 32

attribute vec3 position;
attribute vec2 tex;

uniform mat4 camera;             /* view-projection (folds the BSP basis) */
uniform mat4 models[MODEL_BATCH]; /* per-instance world transforms */
uniform int  glw_InstanceID;     /* emulated instance index, 0..batch-1 */

varying vec2 frag_tex;

void main()
{
    frag_tex    = tex;
    gl_Position = camera * models[glw_InstanceID] * vec4(position, 1.0);
}
