# Fairy Tale Phone
X-Mas 2013 Arduino project.
Using an old Zellweger Modell 70 (anno 1982) telephone with dial wheel.
The wheel can be spinned to count impulses and then playing one of ten different audio fairies stored as 8bit full-rate (62500) mono PCM files named from 1.PCM to 10.PCM

## SD Card > Arduino pinout:
```
CS (1)   -> Pin 10 (Chip select)
MOSI (2) -> Pin 11
MISO (7) -> Pin 12
SCK  (5) -> Pin 13

Ear phone (tone) -> Pin 3 and GND
Ear phone (sound) -> Pin 9 and GND
Dial wheel -> Pin 2 and GND
```

## Used libraries
SimpleSDAudio library for audio playback from SD card
http://www.hackerspace-ffm.de/wiki/index.php?title=SimpleSDAudio

## Convert PCM files
Use the sox tool to convert your audio file as PCM file
```
sox audiofile.mp3 --norm=1 -e unsigned-integer -b 8 -r 62500 -c 1 -t raw 1.PCM
```
