# ILS window plugin for EuroScope

Shows aircraft position relative to the ILS.

## Installation and use

1. Download the latest release from the [Releases](https://github.com/EvenAR/ILS-Window/releases) page.
2. Place the DLL and JSON file in the same directory.
3. Load the plugin in EuroScope.

By default the plugin will automatically open ILS windows based on the active arrival runways. It's also possible to open windows manually using `.ils <title>` (where `<title>` refers to the `"title"` in the configuration file).

> [!NOTE]
> **Limitation**: This plugin uses the altitude reported by the pilot clients. Depending on the pilot's weather settings and the flight simulator they are using, the height displayed in the ILS Window may not match what the pilot sees ([more info](https://forums.flightsimulator.com/t/vatsim-ivao-pilotedge-users-be-aware-of-an-important-bug/426142)).

## Configuration

The plugin reads the JSON configuration file when the plugin is loaded. For detailed configuration options, refer to [the sample configuration file](/Sample%20config). Worth noticing:

- `localizerCourse` must be the **true** heading in degrees
- `maxOffsetLeft` and `maxOffsetRight` is used to specify the maximum visible range (in nautical miles) left and right of the localizer. Most relevant for airports with paralell approaches.

## Screenshot

![image](https://github.com/user-attachments/assets/49e513da-1fa5-4483-8823-f557a293c05a)

The design is inspired by the Norwegian Air Traffic Control System (NATCON).
