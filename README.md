# ILS window plugin for EuroScope

Shows aircraft position relative to the ILS.


## Installation and use

1. Download the latest release from the [Releases](https://github.com/EvenAR/ILS-Window/releases) page.
2. Place the DLL and JSON file in the same directory.
3. Load the plugin in EuroScope.

By default the plugin will automatically open ILS windows based on the active arrival runways. It's also possible to open windows manually using `.ils <title>` (where `<title>` refers to the `"title"` in the configuration file).

## Configuration

The plugin reads the JSON configuration file when the plugin is loaded. For detailed configuration options, refer to [the sample configuration file](/Sample%20config). Worth noticing:

- `localizerCourse` must be the **true** heading in degrees
- `maxOffsetLeft` and `maxOffsetRight` is used to specify the maximum visible range (in nautical miles) left and right of the localizer. Most relevant for airports with paralell approaches.


## Screenshot

Inspired by the Norwegian Air Traffic Control System (NATCON).

![image](https://github.com/user-attachments/assets/49e513da-1fa5-4483-8823-f557a293c05a)
