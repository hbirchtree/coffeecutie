#ifndef BLOCK_INSTANCES_GLSL
#define BLOCK_INSTANCES_GLSL

/* Instances in a uniform block window; lowered at load where blocks are
 * small (materials::block_instances) */
layout(constant_id = 0) const int BLOCK_INSTANCES = 128;

#endif
