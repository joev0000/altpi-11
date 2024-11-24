# AltPi-11

AltPi-11 is an alternate software implementation for the [PiDP-11], an
emulated PDP-11/70 running on the Raspberry Pi with a replica of the PDP-11/70
front panel, created by Oscar Vermeulen. The emulation is done by [SimH],
a suite that provides emulations for many historic computers, including the
PDP-11 series. The PiDP-11 includes a software package that contains
installation and start scripts, and a patched version of SimH that includes a
"real console" device. That device uses Jörg Hoppe's [Blinkenbone], a protocol
used to connect real and virtual console devices to emulated computers.

## Goals

The overall goal of this project is to make a lighter integration between the
PiDP-11 and SimH. Point-by-point:
* Run with unmodified releases of SimH, without requiring patches or custom
  devices.
* Remove dependency on Blinkenbone, and specifically its dependency on
  [SunRPC].
* Provide an architecture that will provide a path to support additional SBCs
  beyond the Raspberry Pi 3 and 4.

## Status

* With used with Raspberry Pi 3, memory can be deposited and examined using
  the PiDP-11 panel. Programs entered using PiDP-11 switches run.
* SimH process is spawned, and some of the PiDP-11 lamps are illuminated
  based on the simulator state.
* Raspberry Pi 4: Not tested, but the driver for the Broadcom BCM2711 SoC
  is included.
* Raspberry Pi 5: Skeleton for the driver for the RP1 in place.

## TODO

* Finish the console implementation as described in Section 3 of the
  _KB-11C Processor Manual (PDP-11/70)_ ([EK-KB11C-TM-001]).
* Verify functionality on Raspberry Pi 4.
* Complete RP1 GPIO implementation for Raspberry Pi 5.
* Connect to console port and redirect to tty.
** This is how the SimH front panel support works; possibly provide upstream
   patch to change how this works.
* Parse ini file to find `go` and `boot` directives, and use the SimH
  front panel API to start execution.

## Building

1. A few source files from SimH are required. Download a source release or
   use a local source control workspace with the SimH source.
2. Set the `SIMH_SRC` environment variable to the root of the SimH source tree.
   The default value is `../simh`.
3. Run the `build.sh` script

## Running

AltPi-11 requires the `pdp11` binary from a SimH release. The `pidp11`
executable takes two arguments: the first is the path to the `pdp11` binary
from SimH. The second is the path to a SimH initialization file. Note, that
file must not include "go" or "boot" directives, and it must include the
configuration to move the console to telnet. For example

```
set cpu 11/70 4M
set console telnet=1030
set console telnet=unbuffered
```

Run:
```
pidp11 /path/to/pdp11 /path/to/ini
```

SimH will wait until a connection to the console has been made. Use the `nc`
command to make that connection:

```
nc localhost 1030
```

The `pidp11-off` program can be used to turn off the lamps on the PiDP-11,
if any are left on.

## Acknowledgements

* Oscar Vermeulen: Creator of the PiDP-11 and other high-quality console
  replicas.
* Jörg Hoppe: Creator of Blinkenbone and the original software to connect the
  PiDP-11 to SimH.
* Bob Supnik: Creator of SimH.
* SimH Maintainers

[Blinkenbone]: https://retrocmp.com/projects/blinkenbone
[EK-KB11C-TM-001]: https://bitsavers.org/pdf/dec/pdp11/1170/EK-KB11C-TM-001_1170procMan.pdf
[PiDP-11]: https://obsolescence.dev/pidp11.html
[SimH]: https://github.com/simh/simh
[SunRPC]: https://www.rfc-editor.org/rfc/rfc5531.html
