# Rendering of Blam maps

Initial rendering, lots of wrong indexes:

![](update1.png)

Somewhat better, missing scenery, not handling multiple BSPs correctly:

![](update2.png)

Loading scenery, figuring out that they use `GL_TRIANGLE_STRIP` for rendering. Notice the holes in the tree:

![](update3.png)

Loading all scenery correctly, loads an entire level with poor performance (missing batching for scenery)

![](update4.png)

Rendering the entirety of 343 Guilty Spark, including scenery at nice framerates when using batching.
Also places matrices for scenery in a shader storage buffer.

![](update5.png)

# Significant values

Values that appear time and time again as `u32`

    1065353216 = Floating-point 1.0
