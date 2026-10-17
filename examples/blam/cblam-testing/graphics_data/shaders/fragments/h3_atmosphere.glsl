/* Halo 3's atmosphere.fx, shared by the BSP/model and water programs */

layout(location=99) uniform vec3 camera_position;
/* v_atmosphere_constant_0..5, _extra */
layout(location=113) uniform vec4 atmosphere[7];

/* Halo 3's compute_scattering: extinction of the surface's light and the
 * sunlight scattered in along the view ray */
void compute_scattering(vec3 view_point, vec3 scene_point, out vec3 extinction, out vec3 inscatter)
{
    extinction = vec3(1.0);
    inscatter  = vec3(0.0);
    if(atmosphere[1].w < 0.0)
        return;
    vec3  sun_dir  = atmosphere[0].xyz;
    vec3  view_vec = view_point - scene_point;
    float dist     = length(view_vec);
    view_vec /= max(dist, 1e-6);
    float c_theta = -dot(view_vec, sun_dir);
    dist = min(max(dist + atmosphere[0].w, 0.0), atmosphere[1].w);
    const float log2_e = 1.4426950;
    float view_height  = max(view_point.z - atmosphere[3].w, 0.0) * log2_e;
    float scene_height = max(scene_point.z - atmosphere[3].w, 0.0) * log2_e;
    float diff         = view_height - scene_height;
    float mie_scale = atmosphere[4].w, ray_scale = atmosphere[5].w;
    float dp, dm;
    if(diff * diff > 0.001)
    {
        dp = -(exp2(-view_height / mie_scale) - exp2(-scene_height / mie_scale)) * dist * mie_scale / diff;
        dm = -(exp2(-view_height / ray_scale) - exp2(-scene_height / ray_scale)) * dist * ray_scale / diff;
    } else
    {
        dp = exp2(-view_height / mie_scale) * dist;
        dm = exp2(-view_height / ray_scale) * dist;
    }
    extinction = exp2(-(atmosphere[2].xyz * dm + atmosphere[3].xyz * dp));
    vec3  beta_m_theta = atmosphere[4].xyz * (1.0 + c_theta * c_theta);
    float heyey        = atmosphere[2].w - atmosphere[6].x * c_theta;
    vec3  beta_p_theta = atmosphere[5].xyz * pow(max(heyey, 1e-4), -1.5);
    inscatter = atmosphere[1].xyz * (beta_m_theta + beta_p_theta) * (1.0 - extinction);
}
