# ILS window plugin for EuroScope

Shows aircraft positions relative to the ILS.

## Installation and use

1. Download the latest release from the [Releases](https://github.com/EvenAR/ILS-Window/releases) page.
2. Place the DLL and JSON file in the same directory.
3. Load the plugin in EuroScope.

By default the plugin will automatically open ILS windows based on the active arrival runways. It's also possible to open windows manually using `.ils <title>` (where `<title>` refers to the `"title"` in the configuration file).

> [!NOTE]
> The plugin applies cold temperature correction to improve aircraft height accuracy. This is necessary for newer flight simulators with advanced atmospheric models ([more info](https://forums.flightsimulator.com/t/vatsim-ivao-pilotedge-users-be-aware-of-an-important-bug/426142)). For pilots using older simulators or custom weather settings, the displayed height may be slightly different from what is shown in the simulator.

## Configuration

The plugin reads the JSON configuration file when the plugin is loaded. For detailed configuration options, refer to [the sample configuration file](/Sample%20config). Worth noticing:

- `localizerCourse` must be the **true** heading in degrees
- `maxOffsetLeft` and `maxOffsetRight` is used to specify the maximum visible range (in nautical miles) left and right of the localizer. Most relevant for airports with paralell approaches.
- `windowStyle` must be `"CDE"` (Common Desktop Environment) or `"X11"` (X Window System).
- `defaultTagMode` must be set to `"squawk"` (shows only the SSR code) or `"callsign"`.

## Screenshot

![image](https://github.com/user-attachments/assets/49e513da-1fa5-4483-8823-f557a293c05a)

The design is inspired by the Norwegian Air Traffic Control System (NATCON).
