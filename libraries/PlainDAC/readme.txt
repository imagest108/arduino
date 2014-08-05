This is a C++ library for Arduino for acquiring data repeatedly.
Tested on Arduino 0021 Alpha.

Installation
--------------------------------------------------------------------------------

To install this library, just place this entire folder as a subfolder in your


When installed, this library should look like:

Arduino\libraries\PlainDAC              			(this library's folder)
Arduino\libraries\PlainDAC\PlainDAC.cpp 			(the library implementation file)
Arduino\libraries\PlainDAC\PlainDAC.h   			(the library description file)
Arduino\libraries\PlainDAC\keywords.txt 			(the syntax coloring file)
Arduino\libraries\PlainDAC\examples     			(the examples in the "open" menu)
Arduino\libraries\PlainDAC\readme.txt   			(this file)

Building
--------------------------------------------------------------------------------

After this library is installed, you just have to start the Arduino application.
You may see a few warning messages as it's built.

To use this library in a sketch, go to the Sketch | Import Library menu and
select PlainDAC.  This will add a corresponding line to the top of your sketch:
#include <PlainDAC.h>
