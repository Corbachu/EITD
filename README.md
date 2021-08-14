----------------
EDGE In The Dark
AITD-Style Engine ported to EDGE and other archs. 

Licensed under the GPLv2
(C) The EDGE Team, 2021
(C) FITD team/contributors
(C) AUTHORS


What is Edge in the Dark ("EITD")?
------------

EITD ("EDGE in the Dark") is an open source vesion of the
engine used in the Alone in the Dark (AITD) and sequels. It
is based off of fitd ("Free in the Dark").

What is Dream in the Dark?
------------

Years ago, this started as "Dream in the Dark", a port of fitd to SH-4/DC. There
might still be refs to "DITD" or DREAMDARK hanging around.

*special notice:
----------------------------

This is a massive hack-job/WIP until I get things more under
control!

What is supported
-----------------

AITD1:
      Should be completable. Saving and loading is supported,
      but only one slot is available (the screen to select the
      save position is not implemented). It's possible to load
      a savegame from the original game, provided it's file is
      named "save0.itd" (rename it if necessary). Like wise,
      it is possible to load a FITD save in the original engine.
      There is a big issue concerning the projectils. Basically,
      guns, won't work, and throwing objects may produce strange
      result. Save before throwing anything, as the object may
      apprear in a wall...

AITD2:
      Supported but uncompletable. The introduction is buggy
      due to a regression in the script engine. Hit 'esc' to skip
      the intro (both parts) to bypass the bug .Save/load is
      unsupported.

JACK:
      Supported but uncompletable.
      Save/load is unsupported.

AITD3:
      Supported but uncompletable.
      Save/load is unsupported.

How to use EITD
---------------

Build with CMake. Supports Windows and Unix, just like EDGE. Drop into folder with AITD release data and run.

Authors
-------

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
	  Authored EDGE and the EPI library, also the coding style, we use here. :P


Special Thanks
---------

- The EDGE Team, and the EDGE Engine from which we use significant pieces of here.

- Andrew Apted

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

