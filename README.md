## EDGE In the Dark
---
#### EITD ("EDGE in the Dark") is an open source version of the engine used in the Alone in the Dark (AITD) and its sequels. It is based off of fitd ("Free in the Dark") and uses the Enhanced DOOM Gaming Engine ("EDGE") as its backend.

## (C) 1999-2021 The EDGE Team & [Contributors](https://github.com/3dfxdev/hyper3DGE/blob/master/AUTHORS.md)
## (c) FITD Authors/Team ("Free in the Dark") (see Contributors above)
##### Uses GPL-licensed technology from id Tech 1-4 (C) 1997-2011 id Software, LLC
##### DOSDoom originally developed by Chi Hoang and the DOSDoom Team, (C) 1997-1999
#### [Licensed under the GPLv2 or greater](https://github.com/Corbachu/EITD/blob/master/AUTHORS.md)
#### [See our EDGE Wiki](http://3dfxdev.net/edgewiki/index.php/Main_Page) - TODO MAKE EITD WIKI

### *special notice:
----------------------------

This is a massive hack-job/WIP until I get things more under control!



## Build System for EITD
---
To build EITD, build with CMake to produce the binary.
- Simply drop into folder with AITD release data and run.
#### Libraries
---
For the list of libraries required by EDGE, please see the following document: docs/tech/libraries.txt. These are pre-built if you choose to use them; see the /deps/ folder.

---
## FAQ

### What is Dream in the Dark?
------------

Years ago, this started as "Dream in the Dark", a port of fitd to SH-4/DC. There might still be refs to "DITD" or DREAMDARK hanging around.



### What Is Supported?
---
- AITD1:
    Should be completable. Saving and loading is supported,
    but only one slot is available (the screen to select the
    save position is not implemented). It's possible to load
    a savegame from the original game, provided it's file is
    named "save0.itd" (rename it if necessary). Like wise,
    it is possible to load a FITD save in the original engine.
    There is a big issue concerning the projectils. Basically,
    guns, won't work, and throwing objects may produce strange
    result. Save before throwing anything, as the object may
    appear in a wall...

- AITD2:
      Supported but uncompletable. The introduction is buggy
      due to a regression in the script engine. Hit 'esc' to skip
      the intro (both parts) to bypass the bug .Save/load is
      unsupported.
- JACK:
      Supported but uncompletable.
      Save/load is unsupported.

- AITD3:
      Supported but uncompletable.
      Save/load is unsupported.

# The EDGE Team
---
## Team and Contributors

EDGE has and will continue to exist thanks to all the people who contribute:
- Corbin "Corbachu" Annis
	  Programmer, SH4/CC ports

- hogz
	  Maintainer of his own fork "hfitd", which I've incorporated some parts of.

- somaen
	  Initial FITD-risidualVM stuff

- tigrouind
	  AITD Room Viewer

- Vincent Hamm "yaz0r" (yazoo@yaz0r.net)
      Project leader and main programmer

- Nicolas Noble "Pixel"
      Cross platform issues. Helped understanding some
      complex aspect of the 3d engine
	  
- Andrew Apted
	  Authored EDGE, EPI, and the coding style we use here. :P


## Special Thanks
---
- The FITD Team for Free in the Dark.
- The EDGE Team, and the EDGE Engine from which we use significant pieces of here.
- The ridisualvm team for initial backends/support

### Original "Greetings" portion from FITD:
- All the Alone in the Dark 1 team:
      Frederick Raynal
      Yael Barroz
      Didier Chanfray
      Philippe Vachey
      Franck De Girolami
      Jean-Marc Torroella
      Hubert Chardot
      Franck Manzetti

- Laurent Paret, you know what for ;)

- Sebastien Viannay, maybe this project wouldn't
      have existed without you...

- All those I forgot....




## Development History
---
Read the historical timeline of EDGE at [The Doom Wiki](https://doomwiki.org/wiki/EDGE), or check out the /docs/logs/ folder.

---

# Support
* Visit the [EDGE forums](http://tdgmods.net/smf) and get involved with the community and the various projects for the engine.
* The [EDGEWiki](http://3dfxdev.net/edgewiki) is also a great resource for editing documentation and other information related to the engine.

(C) 1999 - 2021 [Isotope SoftWorks](https://www.facebook.com/IsotopeSoftWorks/) and Contributors (The EDGE Team). All Rights Reserved.
