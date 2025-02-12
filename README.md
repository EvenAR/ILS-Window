# ILS window plugin for EuroScope

Shows aircraft positions relative to the ILS.

## Installation and use

1. Download the latest release from the [Releases](https://github.com/EvenAR/ILS-Window/releases) page.
2. Place the DLL and JSON file in the same directory.
3. Load the plugin in EuroScope.

By default the plugin will automatically open ILS windows based on the active arrival runways. It's also possible to open windows manually using `.ils <title>` (where `<title>` refers to the `"title"` in the configuration file).

> [!NOTE]
> The plugin applies cold temperature correction to improve the accuracy of the reported height from the aircraft. This is necessary for newer flight simulators with advanced atmospheric models ([more info](https://forums.flightsimulator.com/t/vatsim-ivao-pilotedge-users-be-aware-of-an-important-bug/426142)). For pilots using older simulators or custom weather settings, the presented height may be slightly different from what is shown in the flight simulator.

## Configuration

The plugin reads a JSON configuration file when the plugin is loaded. For detailed configuration options, refer to [the sample configuration file](/Sample%20config). Worth noticing:

- `localizerCourse` must be the **true** heading in degrees
- `maxOffsetLeft` and `maxOffsetRight` is used to specify the maximum visible range (in nautical miles) left and right of the localizer. Most relevant for airports with paralell approaches.
- `windowStyle` must be `"CDE"` (Common Desktop Environment) or `"X11"` (X Window System).
- `defaultTagMode` must be set to `"squawk"` (shows only the SSR code) or `"callsign"`.

## Screenshots

The first image shows an example using the "X11" window style. In the second image the "CDE" window style is used.

![image](https://github.com/user-attachments/assets/6dc74ee9-8536-46ee-a3ff-6e04dc470536) ![Screenshot 2025-02-12 215106](https://github.com/user-attachments/assets/0fa96a95-b088-4f0c-92d3-fc6df11aef1e)

The design is loosely inspired by the Norwegian Air Traffic Control System (NATCON).
