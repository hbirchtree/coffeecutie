#!/usr/bin/python2

import Coffee

props = Coffee.CDWindowProperties()
renderer = Coffee.CDRenderer()

props.title = "Hello"
props.flags = props.Minimized | props.Resizable | props.Windowed | props.Decorated
props.contextProperties.flags = props.contextProperties.GLDebug | props.contextProperties.GLAutoResize

renderer.run(props)
