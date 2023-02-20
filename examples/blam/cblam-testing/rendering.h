#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"

template<typename V>
using MeshRendererManifest = Components::SubsystemManifest<
    type_list_t<BspReference, SubModel, Model, MeshTrackingData>,
    type_list_t<
        BitmapCache<V>,
        ModelCache<V>,
        BSPCache<V>,
        ShaderCache<V>,
        BlamFiles>,
    empty_list_t>;

using ScreenClearManifest = compo::SubsystemManifest<
    empty_list_t,
    type_list_t<gfx::system, BlamResources>,
    type_list_t<comp_app::Windowing>>;

void alloc_renderer(compo::EntityContainer& container);
