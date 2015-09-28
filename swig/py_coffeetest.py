#!/usr/bin/python2

import Coffee

props = Coffee.CDWindowProperties()
renderer = Coffee.CDRenderer()

props.title = "Hello"
props.flags = props.Minimized | props.Resizable | props.Windowed | props.Decorated
props.contextProperties.flags = props.contextProperties.GLAutoResize
props.contextProperties.version = Coffee.CGLContextVersion(3,3)

renderer.run(props)
