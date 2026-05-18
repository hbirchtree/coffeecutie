# Basis

This trace format is based on ChromeTraceFormat with some extensions. The events should be visualized as a timeline with different tracks for each thread and process if applicable. In most ways it should behave as `chrome://tracing` does in terms of displaying its calling stack metrics.

Some of which are:

* Metrics; tracing a scalar, a symbolic value and screenshots over time, for example frequency and power state. There is also a type for markers which display a vertical marker across the event timeline. The metric events are denoted by `"ph": "m"` in the event stream, with an `"id"` field pointing to the end of the event stream, where the type and name of the value is denoted.
  * The `"ph": "M"` events at the end of the event stream describe what each metric ID contains. As such it's necessary to first parse every `"ph": "M"` event before displaying metrics. It contains the name and the type of the metric. `0` denotes a scalar value metric, `1` is a symbolic value (not a scalar), `2` is a marker and `3` is a screenshot.
  * Screenshots are encoded as base64 in the value string

Displaying metric depends on the type. A scalar should be displayed as a graph spanning the entire event timeline. A symbolic metric should be displayed as a timeline showing where the value changes. A marker should be displayed as a vertical line across the event timeline. A screenshot should be displayed as a thumbnail in its track which can be expanded to the full screen.

There are also commonly values stuffed in the `"extra"` section as well as other keys on the root object. For an example, `examples/profile.json` demonstrates most of these different features of the extended trace format, including screenshots.
