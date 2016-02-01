# FonaSMS

This is a stripped down version of Adafruits wonderful FONA Library.

This was done to try and reduce memory constraints for a project where we were
tight on RAM and space on the device, and needed to cut things out.

All credit for the code is Adafruit, all I did is hit the delete key a bunch.

I did not originally intend to host this publically, as I did not properly fork
their repo, but just took one of their downloaded samples and then began modifying.
I put it onto github to make it easier for another member of the project to have access to it,
but as it stands the current repo is not a proper reference to the authors original work,
and thus I want to call out that NONE of this code, save a tiny number of renames,
belongs to me.

# Adafruit FONA Library [![Build Status](https://secure.travis-ci.org/adafruit/Adafruit_FONA_Library.svg?branch=master)](https://travis-ci.org/adafruit/Adafruit_FONA_Library)

**This library requires Arduino v1.0.6 or higher**

This is a library for the Adafruit FONA Cellular GSM Breakouts etc

Designed specifically to work with the Adafruit FONA Breakout
  * https://www.adafruit.com/products/1946
  * https://www.adafruit.com/products/1963
  * http://www.adafruit.com/products/2468
  * http://www.adafruit.com/products/2542

These modules use TTL Serial to communicate, 2 pins are required to interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Check out the links above for our tutorials and wiring diagrams

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution
With updates from Samy Kamkar

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Adafruit_FONA
Check that the Adafruit_FONA folder contains Adafruit_FONA.cpp and Adafruit_FONA.h

Place the Adafruit_FONA library folder your *arduinosketchfolder*/libraries/ folder.
You may need to create the libraries subfolder if its your first library. Restart the IDE.
