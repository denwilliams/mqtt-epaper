# mqtt-epaper

ESP32 powered ePaper Dashboard Controlled via MQTT

I built this because I never got the idea of all the other single-purpose ePaper projects out there. So many need an overpowered RPi attached to them, others have very specific code running on Arduino code. I really want the ePaper as a generic display output for other systems, and change it regularly.

It seems that all the displays typically achieve are a bunch of rectangles with text over the top of them (a line is just a 1px rectangle).

So this project allows you to construct a simple screen layout by passing in an array of rectanges to draw backgrounds and lines, followed by a number of strings for text to write over the top.

I use this on my 7.5" Waveshare black&white display. Aiming to have this powered by battery one day. Really designed for my purposes, but feel free to use or steal code if it suits yours too.

> NOTE: I have no experience with Arduino code and haven't used C in almost 20 years, apologise for the hacky code. This will probably change regularly in a breaking way. Clone it if you care.

## API

Send an event to `esp32/output` with a JSON payload. **MQTT payload is limited to a predefined buffer size. If you are sending large packets you might need to increase that size.**

Examples...

Minimum:

```json
{
  "strings": [
    {
      "x": 100,
      "y": 100,
      "message": "Hello world"
    }
  ]
}
```

Full:

```json
{
  // Screen rotation: 0|1|2|3
  "rotation": 0,
  // Partial refresh
  "partial": true,
  "rectangles": [
    {
      // Box
      "x": 0,
      "y": 0,
      "w": 800,
      "h": 50
    },
    {
      // Line
      "x": 400,
      "y": 0,
      "w": 1,
      "h": 480
    },
    {
      // White Line
      "x": 0,
      "y": 25,
      "w": 800,
      "h": 1
    }
  ],
  "strings": [
    {
      // Note x,y represents the bottom-left position of the first character, not the top-left
      "x": 100,
      "y": 450,
      "message": "Hello world"
    },
    {
      // Strings are written last, so white text over black rectangle is readable
      "x": 100,
      "y": 25,
      "message": "Hello world",
      "color": "white",
      "font": "azonix36"
    }
  ]
}
```

### Fonts

The fonts are embedded. You have to change the code if you want new fonts. [Use fontconvert to make new fonts](https://github.com/adafruit/Adafruit-GFX-Library/tree/master/fontconvert).

Current fonts:

- `"robotothin72"` Roboto font, thin variant, 72pt
- `"azonix36"` Azonix font, 36pt

### Color

Use `"black"` or `"white"`. I haven't supported 3 color displays since I don't have one.

Defaults to `"black"` if not specified.

### WiFi

Credentials are hard-coded. Edit `network.h` to configure.

## TODO

- Allow for text to be centred.
- Allow for bitmaps.

## Related

- https://www.hackster.io/fablabeu/the-mrd-mqtt-remote-display-based-on-epaper-c5d0ae
- https://github.com/Dom1n1c/PaPiRus-MQTT
