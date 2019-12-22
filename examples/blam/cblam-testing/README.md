# Rendering of Blam maps

![](update1.png)
Initial rendering, lots of wrong indexes everywhere. Unplayable framerates when rendering terrain.

![](update2.png)

Somewhat better, missing scenery, not handling multiple BSPs correctly:

![](update3.png)

Loading scenery, figuring out that they use `GL_TRIANGLE_STRIP` for rendering. Notice the holes in the tree:

![](update4.png)

Loading all scenery correctly, loads an entire level with poor performance for rendering scenery, as each piece of scenery is a drawcall.

![](update5.png)

Rendering the entirety of 343 Guilty Spark, including scenery at nice framerates when using batching.
Also places matrices for scenery in a shader storage buffer to allow batching at all.

# Significant values

Values that appear time and time again as `u32`

    1065353216 = Floating-point 1.0
