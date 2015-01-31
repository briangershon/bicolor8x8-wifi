bicolor8x8-wifi
===============

This Arduino project allows you to set the colors for a Bi-Color 8x8 Matrix
from a website via Wifi.

Author: Brian Gershon

License: MIT

Leverages hardware and Arduino software libraries created by Adafruit.

How to use
----------

This app continually polls an HTTP page looking for a body that just contains
64 chars, representing 8 rows of 8 columns. 'o' for empty, 'r' for red,
'g' for green,'y' for yellow.

For example, this shows a smiley face in green:

    `ooggggooogoooogogogoogoggooooooggogoogoggooggoogogoooogoooggggoo`

I've created an example web application (Rails 4.2) that allows you to create the
various emoticons, along with a sample route that picks a random 64-char emoticon.
See <https://github.com/briangershon/bicolor8x8-rails>

Components
----------

* [Bi-Color 8x8 Matrix](https://www.adafruit.com/products/902) and [Getting Started and Arduino library](https://learn.adafruit.com/adafruit-led-backpack/bi-color-8x8-matrix) available from Adafruit.

* [Adafruit HUZZAH CC3000 WiFi Breakout with Onboard Antenna - v1.1](https://www.adafruit.com/product/1469)

* Arduino Uno

Wiring and Setup
----------------

* Wire up components and install Adafruit Arduino libraries as described in the two product tutorials above.
