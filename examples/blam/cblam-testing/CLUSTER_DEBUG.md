# The application

We're attempting to re-implement the Halo: Combat Evoled engine with enhancements. As part of this effort, there are many systems to implement:

- Rendering
  - BSPs
    - What is the leaf data really?
    - What's the leaf map data for?
  - Scenery
  - Vehicles
  - Bipeds
  - Equipment
  - Shaders
    - Basic, textured shaders (senv/soso)
    - Transparent shader (sotr)
    - Chicago shader, multi-purpose shader (schi, Xbox only)
    - Chicago extended shader, multi-purpose and flexible (scex)
    - Water shader (swat)
    - Meter shader (smet)
    - Plasma shader (spla)
    - Glass shader (sgla)
  - Culling
    - CPU-based frustum culling for BSPs and models
    - Needs to rely on potentially visible set culling
      - Clusters detail which BSPs are visible in a given area
      - Portals determine visibility from each cluster
        - Directional? Or bidirectional?
      - Clusters can connect to other BSPs using its `transition_bsp` member, details are not fully known
      - What are mirrors?
- Audio
  - Background music
    - Variations on background music, and selecting new tracks to add to the playlist
    - Clusters each have their own background music
      - When entering a new cluster, we optimally want an event to be emitted that the sound system can pick up
    - Clusters also specify a sound environment
      - Describes their audio properties, eg. reverb and etc.
- Weather
  - Clusters have weather polyhedra associated with them, with a corresponding weather effect, eg. rain or snow
- Physics
  - Integrating Bullet into the build
  - Hooking up bipeds and vehicles to physics
- Vehicles
  - Simulating the dynamics of the vehicles based on the data provided by the vehi tags
- Bipeds
  - Controlling non-player bipeds
  - Allowing the player to control their own biped, of which the player biped is provided by the map
- Animations
  - Animating vehicles and bipeds correctly
  - Animating shaders according to their provided data
    - Requires GLSL work to pack animation data for use on the GPU

Stretch goals:
- Adding rendering codepath for GLES 2.0
  - Can't use UBOs, instancing or array textures
  - Optimizing rendering is a pre-condition
  - Currently only renders terrain with their mesh normals as color

# Running the application

In order to build and run the application, one can run:

    ./cb build desktop:x86_64-buildroot-linux-gnu:multi:rel/BlamGraphics
    DUMMY_PLUG_CONFIG=.github/tests/dummy_plug_short.json COFFEE_DISABLE_PROFILER=1 LD_LIBRARY_PATH=multi_build/desktop-x86_64-buildroot-linux-gnu-multi/lib/ multi_build/desktop-x86_64-buildroot-linux-gnu-multi/bin/BlamGraphics multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/ /mnt/blam/pc/beavercreek.map

There is a dummy plug inserted in the application when using the current command line, which will move the camera in a straight line for 3 seconds upon start of the program.
The implementation of the dummy plug is not important, but can help produce more data for debugging purposes.
As part of the dummy plug, the application will exit after ~5 seconds (excluding load time), to allow collection of the logs without user interaction.

There is also a secondary dummy plug config, `dummy_plug_nomove.json`, where the camera stays in the same position

# Problems to solve

[ ] There is undocumented data structures in the src/coffee/blam part of the project surrounding BSP clusters and portals.
    There exists tooling that builds them, but little information on how the data structures look on disk.
    The data structures in src/coffee/blam/ are memory-mapped from the Halo: CE map files.
[ ] The BSP clusters and portals, when combined, help optimize the rendering of maps in the form of potentially visible sets
    The implementation in source is currently in occluder.cpp, however it's not functioning correctly. The surrounding code is likely not correct.
[ ] There is a difference in the camera space and the BSP space in the BlamGraphics application currently which might need to be amended.
    This would require undoing some of the current implementation, and fixing camera matrices to render the map in the correct orientation.

