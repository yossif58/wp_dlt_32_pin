
#ifndef _SDK_ENVIRONMENT_H_
#define _SDK_ENVIRONMENT_H_

/**
@page sdk_environment SDK Environment

As a prerequisite for this guide, you must be able to successfully build
one of the provided application example from the SDK.

@section installation_of_sdk_environment Installation of SDK Environment

@subsection windows_installation  Windows installation

For Windows installation, we're using <a href="https://docs.microsoft.com/en-us/windows/wsl/about">
Windows Subsystem for Linux</a>. There are other possible options, such as <a href="https://www.cygwin.com/">Cygwin</a>
or <a href="http://www.mingw.org/">MinGW</a> but they are not covered here.

First, install Windows Subsystem for Linux according to
<a href="https://docs.microsoft.com/en-us/windows/wsl/install-win10">
installation guidelines</a>. Choose Ubuntu installation.

After done that and system is update, proceed following:

Update the system:
@code
sudo apt update && sudo apt upgrade && sudo apt dist-upgrade && sudo apt autoremove
@endcode

Install essentials:
@code
sudo apt install build-essential
@endcode

Install some extra packages:
@code
sudo apt install apt-src gawk gzip autoconf m4 automake libtool libncurses5-dev gettext gperf dejagnu expect tcl autogen guile-1.8 flex flip bison tofrodos texinfo g++ gcc-multilib libgmp3-dev libmpfr-dev debhelper texlive texlive-extra-utils
@endcode

Install some packages for 32-bit compatibility and srecord support:
@code
sudo apt install lib32z1 lib32ncurses5 lib32bz2-1.0
sudo apt install lib32ncurses5
sudo apt install srecord
@endcode

Install required Python libraries:
@code
sudo -H python -m pip install pycryptodome
@endcode

Then, download and unpack <a href="http://distribute.atmel.no/tools/opensource/Atmel-ARM-GNU-Toolchain/4.8.4/arm-gnu-toolchain-4.8.4.371-linux.any.x86_64.tar.gz">this pre-compiled version of the Arm Toolchain.</a>

@subsection mac_installation Mac installation

This guide describes the easiest way to get started with the Wirepas SDK on OSX.
This SDK needs various tools which are freely installable.

This guide will show you how to get these installed.

The installation steps are tested on MacOS High Sierra (10.13.1)

@subsubsection homebrew Homebrew

First, we need to install homebrew (www.brew.sh)

This will allow us to install various packages with relative ease.

Open the Terminal and type the following command:

@code
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)
@endcode

This will install the brew package manager.

@subsubsection python Python

Verify python has the right version by typing following in the terminal:

@code
python --version
@endcode

Version number should be 2.7.X (osx ships with 2.7.10). Then install following:

@code
Install pycryptodome by typing
pip install pycryptodome
@endcode

@subsubsection install_gcc_arm_48 Install GCC Arm 4.8

To install the required GCC Arm compiler we need to add a "tap"

In the terminal type:

@code
brew tap PX4/homebrew-px4
@endcode

After that is done type:
@code
brew update
@endcode

Now to install the correct GCC ARM compiler type:
@code
brew install gcc-arm-none-eabi-48
@endcode

@subsubsection install_srecord Install Srecord

In the terminal type:
@code
brew install srecord
@endcode

@subsubsection install_segger_jlink_driver

If you are planning on flashing firmware you might need to install the Segger J-Link Driver which can be found here:

https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack

@subsubsection install_nordic_semiconductor_command_line_tools Install Nordic Semiconductor Command line tools

Go to the following URL to download the Nordic Semiconductor Command Line tools for OSX:

https://www.nordicsemi.com/eng/nordic/Products/nRF51822/nRF5x-Command-Line-Tools-OSX/53402

After unpacking the zip file you might want to move the tools either to <code>/usr/local/bin/</code> or create a symbolic link to them.

In the terminal type:
@code
ln -s < full path>/nrfjprog /usr/local/bin/nrfjprog
@endcode
where full path is the exact location of the unzipped file (eg, <code>/Development/Tools/NRF/nrfjprog/</code>)
Do the same for mergehex:
@code
ln -s <full path >/mergehex /usr/local/bin/mergehex
@endcode

@subsubsection troubleshooting Troubleshooting Python Version Conflicts

If you used to have homebrew installed and used it with python development, it is possible that your version of python is different from where pip installs its packages.
If this is case you most likely run into an error during the compiling where the make file complains about:
@code
cannot import Crypto.Hash.
@endcode
If this happens try the following in the terminal, (and try make command again after each try)

@code
brew link python
python2 -version
@endcode

If python2 exists (because python was installed with brew at some point) edit the <code>makefile</code> and search for <code>python</code> and change the line
@code
python tools/genscratchpad.py
@endcode
 to
@code
python2 tools/genscratchpad.py
@endcode


@section checking_the_installation_validity Checking the installation validity

@ref custom_app "The custom_app application" is a simple application that sends
a periodically incrementing counter value over the Wirepas network.

The application can be built with the following command:

@code
    make app_name=custom_app
@endcode

After execution of this command, you should find the
<code>final_image_custom_app.hex</code> under <code>build/<mcu>/custom_app</code>
folder.

@section flashing_guideline Flashing devices

@subsection flashing_nordic_nrf52_devices Flashing Nordic nRF52 devices

For information on how to flash Nordic nRF52xxx devices, check out <a
href="https://www.nordicsemi.com/DocLib/Content/User_Guides/nrf5x_cltools/latest/UG/cltools/nrf5x_nrfjprogexe">
nRF5x Command Line Tools User Guide</a>

@subsection flashing_silabs_devices Flashing Silicon Labs Devices

If you haven't yet installed <a href="https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack"> Segger J-Link Software and Documentation Pack</a>, do so.

For information how to use Segger Jlink command-line executable (named either <code>JLink.exe
</code> or <code>Jlinkexe</code> according to your operating system), refer to
<a href="https://wiki.segger.com/J-Link_Commander">J-Link Commander</a>.

The following information should be used.

<table>
<tr><th>Device architecture</th><th>Radio profile</th><th>JLink device type</th></tr>
<tr><td>\ref efr32xg12 "EFR32xG12"</td><td>2.4 GHz/SubGhz</td><td>EFR32FG12PxxxF1024</td></tr>
<tr><td>EZR32LGx30F256R60G</td><td>SubGhz 868 MHz only</td><td>EFM32LG230F256</td></tr>
</table>

Note: usually interface is SWD but it depends on the hardware design. For example:
<a href="https://www.silabs.com/support/getting-started/proprietary-wireless/flex-gecko">
Flex Gecko</a> uses SWD.

Here, couple of example <I> J-Link Commander Command files</I> that can be used
as a reference:

Programming EZR32LGx30F256R60G device:
@code
device EFM32LG230F256
if SWD
speed 1000
connect
r
loadfile <path_to_hex_file>.hex
r
g
q
@endcode

Programming EFR32xG12 device:
@code
device EFR32FG12PxxxF1024
if SWD
speed 1000
connect
r
loadfile <path_to_hex_file>.hex
r
g
q
@endcode

You may also need to <i>recover</i> the device when:
*  Remove all the parameterization
*  Flashing the firmware again
The recovery removes flash write protection mechanism and whole flash contents.
Following command for recovering is issued when using JLink commander:
@code
J-Link>unlock <enter>
Syntax: unlock <DeviceName>
---Supported devices---
  LM3Sxxx [<Auto>]
  Kinetis
  EFM32Gxxx
  LPC5460x
J-Link>unlock LM3Sxxx <enter>
Be sure that nRESET is LOW before continuing.
If you are working with an eval board,
please press the RESET button and keep it pressed.
Press enter to start unlock sequence...
Unlocking device...O.K.
Please power-cycle target hardware.
@endcode

@section sdk_file_structure SDK File structure

This section describes the main files provided in the SDK.

The location of each file is specified in brackets.

@subsection makefile <code>makefile (./)</code>

The makefile is used to build an application and produce images to be
flashed or used with OTAP mechanism.

Each application has its own folder under <code>./source/</code> directory. To
build an application the makefile must be invoked with the parameter
<code>app_name=<app_name></code>.

For example, the following command will build the <code>custom_app</code>
application:

@code
    make app_name=custom_app
@endcode

The makefile produces several binaries under <code>build/<mcu>/<app_name>/</code>:

-  <code>final_image_<app_name>.hex</code>: this image can be flashed on a
   blank chip. It contains everything (Bootloader, Wirepas Mesh Stack
   and the application).

-  <code><app_name>.otap</code>: this image contains only the application and
   can be used to update the application with the OTAP mechanism.

-  <code><app_name>_wc_stack.otap</code>: this image contains the application
   and the Wirepas Mesh stack and can be used to update the application
   and the stack at the same time with the OTAP mechanism.

@subsection gcc_app_ld gcc_app.ld (./mcu/<mcu>/linker)

This file is the linker script. It ensures that the application is
loadable in its dedicated area in Flash. Particularly, it sets the
application entry point at the beginning of the area.

The recommended size for application is set in this file. If needed the
size can be increased here.

@subsection library_h <library>.h (./api)

@ref api "These files" define the API between the Wirepas Mesh stack and the
application. It must not be modified. Any modification can break the
compatibility with the Wirepas Mesh stack and make the device unusable.

@subsection start_c start.c (./mcu/common/)

@ref start.c "This file" is present in all applications. It positions the application
entry point at the correct place in memory and do basic initialization:
it loads the initialized data from flash to RAM, sets the bss area to 0
in RAM, and calls the application initialization function defined in
<code>app.c</code>.

It also manages compatibility with the stack to avoid issues when
running an application built for an old stack to a newer stack. Running
an application built with a SDK newer than the stack version is not
allowed.

@subsection app_c app.c (./source/<app_name>/)

This file is present in all applications. Application init function and
its application callback functions should be implemented here as defined
in the <code>app.h</code> file.

@subsection config_mk config.mk (./)

Configuration file where following can be configured: the name of the
application, the arm toolchain, the otap version (when testing the
application with OTAP).

@subsection genscratchpad_py genscratchpad.py (./tools)

@ref genscratchpad.py "This tool", used by the @ref makefile "Makefile", allows the scratchpad binary generation
for the OTAP update. It uses the <code>ini</code> file in the same directory

@subsection board_h board.h (./board/<board>/)

@ref board "These files" contain board specific definitions to ease and factorize
application development.

@subsection mcu_specific_files mcu specific files (./mcu/<mcu>/)

@ref mcu "These folders" contain mcu specific files to ease and factorize
application development. Header files (<code>.h</code>) from this folder can be
included in applications directly.

@section nordic_nrf52832_nrf52840 Resources on Nordic nRF52832 & nRF52840

The nRF52 chip version supported by Wirepas Mesh has 512kB of flash and
64kB of RAM.

@subsection flash_memory Flash Memory

To be correctly detected by the Wirepas Mesh stack, the entry point of
the application code must be position at address <code>0x40000</code> (256KB
after the beginning of flash).

As described with more details in OTAP Reference manual \ref relmat3 "[3]", the
application shares its flash memory area with the scratchpad area. There
is no strict limit for the size of the application but if the
application is too big, it will prevent the OTAP to store its scratchpad
in the remaining free space.

The recommended maximum size for an application is 40kB.

@subsection ram_memory RAM Memory

The RAM memory reserved for the application is 8kB.

Note that the application doesn't need its own execution stack.
Application uses the same stack as the Wirepas Mesh firmware.

@subsection peripherals_accessible_by_stack_only Peripherals accessible by stack only

Some peripherals are used by the Wirepas Mesh stack and cannot be
used by the application.

<table>
<tr><th>Peripheral</th><th>Associated interrupt (from file @ref nrf52.h)</th></tr>
<tr><td>Power</td><td><code>POWER_CLOCK_IRQn</code></td></tr>
<tr><td>Radio</td><td><code>RADIO_IRQn</code></td></tr>
<tr><td>Timer0</td><td><code>TIMER0_IRQn</code></td></tr>
<tr><td>WDT</td><td><code>WDT_IRQn</code></td></tr>
<tr><td>Rtc1</td><td><code>RTC1_IRQn</code></td></tr>
<tr><td>ECB (AES)</td><td><code>ECB_IRQn</code></td></tr>
</table>

All the internal interrupt of cortex M are handled by the stack
directly (NMI, HardFault,...)

@subsection peripherals_shared_between_the_stack_and_the_application Peripherals Shared between the stack and the application

Some peripherals are used by the stack but can also be accessed by the
application. There is a @ref app_lib_hardware_t "dedicated API" to reserve these peripherals.

These peripherals must be reserved through the API before being used and
the application must initialize them each time after reserving it. In
fact, the stack may configure them differently.

It is also important to avoid long reservation of theses peripherals to
let the stack use them for its own purpose.

<table>
<tr><th>Peripheral</th><th>Reservation ID (from @ref app_lib_hardware_peripheral_e)</th></tr>
<tr><td>ADC</td><td>@ref APP_LIB_HARDWARE_PERIPHERAL_ADC</td></tr>
<tr><td>Temp</td><td>@ref APP_LIB_HARDWARE_PERIPHERAL_TEMPERATURE</td></tr>
</table>

@subsection peripherals_available_for_the_application Peripherals available for the application

All the other peripherals not listed above are free to be used by the application.

@section efr32xg12 Resources on EFR32xG12

Following chip variants (at 2.4 GHz only) are supported:

-   EFR32FG12P232F1024G L125/M48    [2.4 GHz only, 1024/128, BGA125/QFN48]
-   EFR32FG12P432F1024G L125/M48    [2.4 GHz only, 1024/256, BGA125/QFN48]
-   EFR32FG12P433F1024G L125/M48    [2.4 GHz & SubGHz, 1024/256, BGA125/QFN48]
-   EFR32MG12P232F1024G L125/M48    [2.4 GHz only, 1024/128, BGA125/QFN48]
-   EFR32MG12P332F1024G L125/M48    [2.4 GHz only, 1024/128, BGA125/QFN48]
-   EFR32MG12P432F1024G L125/M48    [2.4 GHz only, 1024/256, BGA125/QFN48]
-   EFR32MG12P433F1024G L125/M48    [2.4 GHz & SubGHz, 1024/256, BGA125/QFN48]
-   EFR32BG12P232F1024G L125/M48    [2.4 GHz only, 1024/128, BGA125/QFN48]
-   EFR32BG12P432F1024G L125/M48    [2.4 GHz only, 1024/256, BGA125/QFN48]
-   EFR32BG12P433F1024G L125/M48    [2.4 GHz & SubGHz, 1024/256, BGA125/QFN48]

@subsection flash_memory2 Flash Memory

To be correctly detected by the Wirepas Mesh stack, the entry point of the
application code must be position at address <code>0x00040000</code> (256kB after the beginning of flash).

As described with more details in OTAP Reference manual @ref relmat3 "[3]", the pplication shares its flash memory area with the scratchpad area. There is no strict limit for the size of the application but if the application is too big, it will prevent the OTAP to store its scratchpad in the remaining free space.
The recommended maximum size for an application is 256kB.

@subsection ram_memory2 RAM Memory

The RAM memory reserved for the application is 72kB-8B (<code>0x2000d000-0x2001eff7</code>).
The application doesn't need its own execution stack. Application uses the same stack as the Wirepas Mesh firmware.

@subsection peripherals_accessible_by_stack_only2 Peripherals accessible by stack only

Some peripherals are used by the Wirepas Mesh stack and cannot be used by the application.

<table>
<tr><th>Peripheral</th><th>Associated interrupt (from chip vendor files)</th></tr>
<tr><td><code>TIMER0</code></td><td><code>TIMER0_IRQn</code></td></tr>
<tr><td><code>RTCC</code></td><td><code>RTCC_IRQn</code></td></tr>
<tr><td><code>WDOG0</code></td><td><code>WDOG0_IRQn</code></td></tr>
<tr><td><code>CMU</code></td><td><code>CMU_IRQn</code></td></tr>
<tr><td><code>CRYPTO0</code></td><td><code>CRYPTO0_IRQn</code></td></tr>
</table>

All the internal interrupt of cortex M are handled by the stack directly (NMI, HardFault,...)

@subsection peripherals_shared_between_the_stack_and_the_application2 Peripherals Shared between the stack and the application

Some peripherals are used by the stack but can also be accessed by the application.

These peripherals must be reserved through the API before being used and the application must initialize them each time after reserving it. In fact, the stack may configure them differently.

It is also important to avoid long reservation of theses peripherals to let the stack use them for its own purpose.

<table>
<tr><th>Peripheral</th><th>Reservation ID (from @ref app_lib_hardware_peripheral_e)</th></tr>
<tr><td>ADC0</td><td>@ref APP_LIB_HARDWARE_PERIPHERAL_ADC</td></tr>
</table>

@subsection peripherals_available_for_the_application2 Peripherals available for the application

All the other peripherals not listed above are free to be used by the application.

Related Material
================

@anchor relmat3 [3] WP-RM-108 - OTAP Reference Manual

@anchor relmat4 [4] WP-RM-100 - Wirepas Mesh Dual-MCU API Reference Manual

*/


#endif /* API_DOC_SDK_ENVIRONMENT_H_ */
