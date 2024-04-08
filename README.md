# MonitorController

A Windows console utility to quickly change display settings including resolution, refresh rate, and scaling mode with multi-monitor support.
Packaged as a simple, dependency-free executable compatible with Windows 7 SP1+ (32/64 bit).


### Usage:

```./MonitorController -a, --list-all-modes```

Print all modes for all active displays.  A mode consists of a resolution, refresh rate, color depth, and scaling format.  The scaling format determines how to adjust the image when outputting at a resolution below the display's native resolution.
STRETCH:	Image is stretched to fit the entire screen.
CENTER:		Image is displayed in its original resolution in the center of the screen surrounded by black bars.
DEFAULT:	Use the display's default option.

```
./MonitorController -a

DISPLAY1 (Primary):
1.    640x480    60hz   32bit  DEFAULT
2.    640x480    60hz   32bit  STRETCH
3.    640x480    60hz   32bit  CENTER
4.    640x480    100hz  32bit  DEFAULT
5.    640x480    100hz  32bit  STRETCH
6.    640x480    100hz  32bit  CENTER
7.    640x480    144hz  32bit  DEFAULT
8.    640x480    144hz  32bit  STRETCH
9.    640x480    144hz  32bit  CENTER
10.   720x480    60hz   32bit  DEFAULT
...
205.  2560x1440  144hz  32bit  DEFAULT
206.  2560x1440  144hz  32bit  STRETCH
207.  2560x1440  144hz  32bit  CENTER
208.  3440x1440  60hz   32bit  DEFAULT
209.  3440x1440  100hz  32bit  DEFAULT
210.  3440x1440  144hz  32bit  DEFAULT
```

<br>

```./MonitorController -c, --list-current-modes```

Print current mode for all active displays.  

```
./MonitorController -c

DISPLAY1 (Primary):
3440x1440  144hz  32bit  DEFAULT
```

<br>

```./MonitorController -s, --set-mode [displayNum] modeNum```

Change the display mode.  ```displayNum``` and ```modeNum``` are taken from the output of ```--list-all-modes```.  If ```displayNum``` is omitted, the Primary display will be used by default.  The display and mode indexes are generally static unless there is a change in the physical monitor configuration, so they can be cached without having to invoke ```--list-all-modes``` repeatedly.

```
./MonitorController -s 1 207

OK
```


<br>

```./MonitorController -h, --help```

Print the help page.
