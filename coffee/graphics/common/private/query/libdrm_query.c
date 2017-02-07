#include <stdint.h>

#include <sys/ioctl.h>
#include <sys/fcntl.h>

#include <libdrm/drm.h>

#include <libdrm/i915_drm.h>
#include <libdrm/nouveau_drm.h>
#include <drm/i915_drm.h>
#include <drm/nouveau_drm.h>

uint64_t get_nouveau_property(int fd)
{
    struct drm_nouveau_gem_new gem;
    ioctl(fd, DRM_IOCTL_NOUVEAU_GEM_NEW, &gem);

    return param.value;
}
