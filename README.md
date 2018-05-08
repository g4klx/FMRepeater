
These are the source files for building the FMRepeater. This is a simplified version of the old Analogue Repeater that has existed on the OpenDV GitHub repository for many years. It has the following features:

* Seperate id and beacon texts.
* Timeout can be one of three styles, or no timeout.
* Optional callsign at repeater open or close, or both.
* Id may be suppressed if too close in time to a previous id.
* Optional kerchunk timer.
* Selectable acknowledgement style.
* All ids and acknowledgements have selectable frequency and speed.
* Minimum talk time before an acknowledgement is sent.
* Built-in string audio bandpass filter.
* Repeater control via an Arduino or a UDRC.
* Audio I/O via any supported sound system, including the UDRC.

It builds on 32-bit and 64-bit Linux as well as on Windows using Visual Studio 2017 on x86 and x64.

This software is licenced under the GPL v2 and is intended for amateur and educational use only. Use of this software for commercial purposes is strictly forbidden.

