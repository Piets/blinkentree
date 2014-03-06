#blinkentree

[blinkentree](http://www.macpiets.net/post/blinkentree) was a fun project I did in summer 2012. It uses an Arduino with an Ethernet shield to control a cheap RGB-LED which lights the tree in my garden.  On the hardware-side there is nothing crazy going on: I just desoldered the original control chip and instead wired in the Arduino. (This means that the Arduino drives the MOSFETs which in turn power the RGB LED).

##Requirements
*    Arduino (with Ethernet shield)
*    SD-card to store the `INDEX.HTM` file on
*    some cheap RGB-LED to light up your tree
*    possibly a tree? ;-)

##Notes
Make sure to change the PINs in the `blinkentree.ino` file accordingly.  
The code is probably not the prettiest one, if you find bugs or add something cool feel free to commit a pull request! :-)

##Thanks
The Arduino software uses the standard `SDI.h`, `Ethernet.h`, and `SD.h` libraries. For the web server it uses the very awesome [Webduino](https://github.com/sirleech/Webduino) project. The web client (`INDEX.HTM`) uses [jQuery](http://jquery.com/) and [jQuery mobile](http://jquerymobile.com/).

##License
blinkentree is licensed under the MIT license. For more details see the `LICENSE` file.