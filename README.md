# TGTWidgets

**An Adafruit_GFX-based widget library for embedded displays.**

TGTWidgets provides reusable UI widgets and screen containers for Arduino projects using Adafruit_GFX-compatible displays.

The library separates individual widget behavior from screen-level organization and application logic. Widgets support runtime value updates and user input through indexed handlers, while screen containers manage widget organization, navigation, and rendering.

## Features

- **Adafruit_GFX integration** — Uses an existing Adafruit_GFX-compatible display instance.
- **Reusable widgets** — Common display and input elements built on a shared `UIWidget` base class.
- **Screen containers** — Organize and manage collections of widgets through `UIScreen`.
- **Runtime value updates** — Update widget values without rebuilding the screen.
- **Indexed input handling** — Widgets support indexed handlers for user-set values and events.
- **Six-key navigation** — Standardized keypad interface with hierarchical key dispatch.
- **Per-digit numeric editing** — Select and modify individual digits of numeric values.
- **Character editing** — Edit character values using three revolving character sets.
- **Runtime widget creation** — Create widgets through screen factory methods.
- **Application-controlled layout** — Screen positioning and application-level navigation remain under developer control.

## Library Structure

| Class | Purpose |
|---|---|
| `UIWidget` | Base class for individual widgets |
| `UIScreen` | Screen container and widget manager |
| `WFLabel` | Formatted text label |
| `WChLabel` | Character-based label |
| `WCharVal` | Editable character value widget |
| `WNumVal` | Editable numeric value widget |
| `WEnumVal` | Enumerated value widget |

## Usage

### Configuration

Include `UIConfig.h` in your project.

It provides the common configuration definitions used by TGTWidgets, including:

- Display dimensions and font sizing
- Screen identifiers and navigation handles
- Widget handles
- Display and selection colors
- Keypad navigation constants
- Navigation event constants

### Display and Screen Initialization

TGTWidgets uses an existing Adafruit_GFX-compatible display object. Create a screen container using that display instance.

### Creating Widgets

Add widgets to the screen using the available `UIScreen` factory methods. Widgets are assigned their screen position, selection index, and widget handle.

### Display and Runtime Updates

Use the screen rendering functions to display the screen. Widget values can be updated at runtime without rebuilding the screen.

## Keypad Navigation

TGTWidgets uses a standardized six-key navigation interface defined in `UIConfig.h`.

| Constant | Key |
|---|---|
| `NAVKEYUP` | Up |
| `NAVKEYDOWN` | Down |
| `NAVKEYLEFT` | Left |
| `NAVKEYRIGHT` | Right |
| `NAVKEYSET` | Set |
| `NAVKEYGET` | Get |
| `NAVKEYNONE` | No key / no action |

Pass the corresponding `NAVKEY` constant to the navigation handler when a keypad key is pressed.

```cpp
OnScreen->Navigate(NAVKEYUP);
```

### Hierarchical Key Dispatch

Key presses are passed through the navigation hierarchy, beginning with the selected widget.

1. The selected widget receives the key first.
2. If the widget handles the key, it processes the associated action.
3. Unhandled keys return to the screen for screen-level navigation.
4. Navigation responses can propagate to higher-level navigation handlers.

This allows widgets to manage their own editing behavior while screens retain control of screen navigation.

### Navigation Events

In addition to physical key codes, `UIConfig.h` defines navigation event constants.

| Constant | Event |
|---|---|
| `NAVSTAGEVAL` | Stage a value |
| `NAVSETVAL` | Set a value |
| `NAVDISPLAY` | Display |
| `NAVGETVALS` | Get values |
| `NAVGETSETVAL` | Get/set value |

Key codes and navigation events use separate value ranges, distinguishing physical keypad input from navigation and value operations.

## Value Editing

### Numeric Values — WNumVal

Numeric widgets support per-digit editing using the six-key keypad.

Individual digits can be selected and modified independently rather than treating the entire number as a single increment/decrement value. This provides precise numeric entry using a compact keypad.

### Character Values — WCharVal

Character widgets use the same general keypad editing approach.

Character selection operates through three revolving character sets, allowing users to navigate character positions and cycle through available characters to select the desired value.

This provides character entry without requiring a full keyboard or a large display interface.

## Dependencies

- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- An Adafruit_GFX-compatible display driver

## Design Philosophy

TGTWidgets provides reusable UI building blocks without imposing a particular application structure or screen-navigation system.

The application determines how screens are arranged, selected, and used. Widgets provide their own display and input behavior, while `UIScreen` manages the widgets belonging to a screen.

The screen factory provides runtime widget creation and rendering, while keeping application-level layout decisions outside the library.

The result is a lightweight, adaptable interface library designed for embedded applications where memory and resource management matter.

## License

See the repository's license file.