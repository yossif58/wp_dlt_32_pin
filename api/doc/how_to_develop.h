#ifndef _HOW_TO_DEVELOP_H_
#define _HOW_TO_DEVELOP_H_

/**
@page how_to_develop How to Develop Application with SDK

The Single-MCU feature allows an application to be executed on the same
chip as the Wirepas Connectivity stack. Wirepas provides an SDK
containing multiple applications examples. Each application describes a
different aspect and can be used as a starting point.

This document will focus on a practical
approach of writing an application. This document will highlight some
crucial point like the minimal steps to follow in the initialization of
an application and will give some practical advices to correctly write
an application.

<table style="width:0%">
<tr><td>@image html alert.jpg ""</td><td>Sections preceded by this symbol will
highlight important points that must be understood and respected.</td></tr>
</table>

@section how_to_create_and_test_a_custom_application How to create and test a \
custom application?

The provided SDK contains several applications. They can be used as a
starting point (Especially the custom_app application).

@subsection build Build

The build can be divided in three steps:

1. Build the application and generate a binary and an OTAP image for the
   application

2. The binary is combined with the Wirepas Mesh stack binary provided by
   Wirepas to generate an OTAP image is generated to update both stack
   and app at the same time.

3. The OTAP image is combined with the Wirepas Mesh bootloader binary
   provided by Wirepas to generate an image that can be flashed directly
   to a blank device.

@image html image8.png

@subsection test_application Test application

To test the application, a minimum of two boards is needed. One of them
must be configured as a sink and the other one as a node. In a first
step, the sink can be connected to a PC running the Wirepas Mesh
Terminal.

The application runs on the board configured as a node. Even if it is
technically possible to run the application on a sink, it implies that
the board has another network connection (WiFi, Ethernet,...) and
everything is managed by the same MCU. In this basic configuration, it
is assumed that it is not the case.

@subsection flashing_the_device Flashing the device

When using the board for the first time, the image named
<code>final_image_<*app_name*>.hex</code> generated by the build process must be
flashed. This image contains the Wirepas bootloader and a scratchpad
composed by the Wirepas Mesh stack and the application. This step must
be done on each new board.

On next boot, the stack and the application will be flashed from the
scratchpad to their dedicated flash area.

To start, the stack needs the four following attributes to be
configured: device role, device address, network address and network
channel. These attributes can be set by the application thanks to the
Single-MCU API during the initialization step. Device role, network
address and network channel can be hardcoded in the application image.
As the node address must be unique in the network it cannot be encoded
in the image itself. If a unique identifier is present on the device,
the application can read it at run time and generate its own unique
address from it. If it is not the case, the address can be set manually
through Wirepas Mesh Terminal.

Using the flash mechanism is a convenient way to develop the application
as long as the devices are accessible and not in high volumes. When
updating a large network, using OTAP is more practical.

@subsection using_otap Using OTAP

For a detail description of the OTAP mechanism, see \ref relmat1 "[1]".

<code><app_name>.otap</code> and <code><app_name>_wc_stack.otap</code> images
can be used to update the whole network with the new application.

@section development_of_a_new_application Development of a new application


In this chapter we will create a new application named <code>new_app</code>.

It describes the initial steps to start writing a new application.

@subsection copy_of_an_application_example Copy of an application example

To quickly start the development of a new application you can copy an
already existing application and use it as a template. Any application
from <code>source/</code> folder can be used as a starting point.

As <code>custom_app</code> was tested in previous chapter, you can copy the full
folder of <code>custom_app</code> to a new folder named <code>new_app</code>.

The new application can be built with the following command::

@code
    make app_name=new_app
@endcode


@subsection change_default_network_address_and_channel Change default network
address and channel

To form a network, all nodes must share the same network address and
network channel.

This information can be changed in file <code>config.mk</code> in the new
application folder created, by setting the variables <code>
default_network_address</code> and <code>default_network_channel</code>:

@code
    # Define default network settings

    default_network_address ?= 0x67EB4A

    default_network_channel ?= 12
@endcode

These variables are defined in the application <code>makefile</code> that can
also be found inside the application folder, as constants <code>
NETWORK_ADDRESS</code> and <code>NETWORK_CHANNEL</code>, respectively:

@code
    # Define default network settings

    CFLAGS += -DNETWORK_ADDRESS=$(default_network_address)

    CFLAGS += -DNETWORK_CHANNEL=$(default_network_channel)
@endcode

Both variables can be overwritten from the command line. It is an easy
way to generate a different binary, to create two distinct networks with
the same application::

@code
    make app_name=new_app default_network_address=0x123456

    make app_name=new_app default_network_address=0x789ABC
@endcode

@subsection change_of_app_area_id Change of app_area_id

Wirepas network supports <I>Over The Air</I> (OTAP) updates of devices on the
network.

A network can contain heterogenous devices (different types of sensors,
for example), so various different kinds of applications can coexist
simultaneously in a network.

Even when upgrading a subset of nodes in the network (just nodes with a
specific type of sensor, for example), all nodes will receive the update
image. This is to ensure that all nodes on the network receive the
update image, due to the multi-hop nature of the network. Consequently,
it is crucial for a node to know if the received image, called a
<I>scratchpad</I>, contains any updates for the node in question. This is the
purpose of the application area ID.

More information about <I>Over The Air Protocol</I> can be found in \ref
relmat1 "[1]".

Each new application <B>must</B> define its own
<code>app_specific_area_id</code> in its <code>config.mk</code> file. It is a
random 24-bit field that must have its most significant bit set to 1 (MSB to 0
is reserved for Wirepas):

@code
    # Define a specific application area_id

    app_specific_area_id=0x83744C
@endcode

This specific area ID will be used in two different places:

- The <I>bootloader</I>, which contains a list of area IDs that it
  accepts, when a new scratchpad is processed. All the nodes that will
  be flashed with the image generated from an
  application build (containing the bootloader) will *only* accept
  updates of the application matching the specific area ID defined in
  <code>config.mk</code>.

- In the <I>scratchpad generation tool</I> (\ref genscratchpad.py),
  where it identifies the application scratchpad image. All the
  <code>*.otap</code> images generated from this application build will be
  only accepted by nodes flashed with a bootloader matching this area
  ID.

<table style="width:0%">
<tr><td>\image html alert.jpg ""</td><td>It is up to each customer to maintain
its own set of <code>app_area_id</code> to avoid mismatch between its
applications.</td><td>
</table>

@subsection helper_function_for_API_libraries Helper function for API libraries

The stack offers services to the application through C function calls.
These functions are logically grouped in libraries.

To use one of the function from a library, the library must be opened
first:

@code

     // The System library
     static const app_lib_system_t * lib_system = NULL;

     ...

     void App_init(const app_global_functions_t * functions)
     {
         lib_system = functions->openLibrary(APP_LIB_SYSTEM_NAME,
                                             APP_LIB_SYSTEM_VERSION);
         if (lib_system == NULL)
         {
             // Could not open the System library
             return;
         }
         ...
     }
@endcode

To facilitate the usage of these libraries, helper functions are present
in the SDK under <code>util/</code> folder. It allows the opening of all
libraries with a single call:

@code
    #include "api.h"

    ...

    void App_init(const app_global_functions_t * functions)
    {
        API_Open();
        // All libraries can be used after this point
    }
@endcode

Calling \ref API_Open "API_Open()" will open all the libraries, and any source
files from the application that includes <code>api.h</code> will be able to
access the libraries directly after the \ref API_Open "API_Open()" call.

As \ref app_init "App_Init()" is the entry point of the application, it is the
most logical place to call \ref API_Open "API_Open()".

Some libraries will be opened by the \ref API_Open "API_Open()" call only if a C
preprocessor constant is defined. In fact, some libraries are only
present on some architectures. For example, the \ref beacon_tx.h "Beacon TX
library" is only available on 2.4 Ghz devices:

@code

    #ifdef WITH_BEACON
    const app_lib_beacon_tx_t lib_beacon_tx;
    #endif

    ...

    #ifdef WITH_BEACON
        lib_beacon_tx = functions->openLibrary(APP_LIB_BEACON_TX_NAME,
                                               APP_LIB_BEACON_TX_VERSION);
    #endif
@endcode


To be able to use API helper in an application, the application makefile
must include the <code>util/</code> folder specific makefile, as depicted in the
figure below. It will also build some other utility functions that will
be added to binary by the linker only if they are referenced from the
application code:

@code
    # Generic util functions are needed (api.c)
    include $(UTIL_PATH)makefile
@endcode

@subsection configuration_of_a_node Configuration of a node

The \ref app_init "App_Init()" function is the entry point for the application.
It is called by the stack after each boot.

The Wirepas Connectivity stack is in the stopped state during this call.
All the API calls that require the stack to be in the stopped state,
like configuring node settings, must be done in this function.

The code below shows the minimal steps for an application to configure
a node and start the stack.

@code

    void App_init(const app_global_functions_t * functions)
    {
        // Open Wirepas public API
        API_Open(functions);

        // Basic configuration of the node with a unique node address
        if (configureNode(getUniqueAddress(),
                          NETWORK_ADDRESS,
                          NETWORK_CHANNEL) != APP_RES_OK)
        {
            // Could not configure the node. It should not happen
            // except if one of the config value is invalid
            return;
        }

        ...

        lib_state->startStack();
    }
@endcode

A newly flashed device starts with its role set to \ref
APP_LIB_SETTINGS_ROLE_HEADNODE and with the flag \ref
APP_LIB_SETTINGS_ROLE_FLAG_AUTOROLE, by default.

To be able to join a network, the application must set at least a unique
node address, a common network address and a common network channel.

This is the role of the \ref configureNode "configureNode()" function available
in \ref node_configuration.h "util/node_configuration.h". It sets the node
address, network address and network channel, but **ONLY** if these settings are
missing from the node. This is the case on first boot after flashing but not
after any reboots after that, unless the settings are explicitly cleared by the
application.

Note that the @ref app_lib_state_stop_stack_f "lib_state->stopStack()" function
will cause a reboot of the device and the \ref app_init "App_Init()" function
will be called again.

Once the node is correctly configured, the stack must be started.

This initialization is just an example and can be something different,
depending on the use case. For example, the application can wait for
configuration via another interface (UART, SPI, NFC, ...). This is the
case with the <code>dualmcu_app</code> application, for example.

<table style="width:0%">
<tr><td>\image html alert.jpg ""</td><td>Application **must** call @ref
app_state_t.startStack "lib_state->startStack()". It can be at the end of \ref
app_init "App_Init()" function or in a deferred context but it must be called.
</td><td>
</table>

Without this call, the node will not be part of any network.

<table style="width:0%">
<tr><td>\image html alert.jpg ""</td><td> The Remote API built-in feature of the
stack (described in \ref relmat7 "[7]") allows the change of a setting remotely.
This change can happen any time from the application point of view and will
generate a reboot and a new call to \ref app_init "App_Init()" after the update.
</td></tr>
</table>

Consequently, it is important to check if a node setting (role, node
address, etc.) is already set before updating it from application
initialization code. Otherwise it would break the Remote API, as the
remotely updated value would be overwritten in \ref app_init "App_Init()".

<table style="width:0%">
<tr><td>\image html alert.jpg ""</td><td> Some node settings must be the same
across all the nodes in the network.</td></tr>
</table>

More general information about node configuration can be found in @ref relmat4
"[4]".

@subsection adding_new_source_files_to_the_application Adding new source files
to the application

For better code readability and organization, the application can be
split in to multiple source files. Adding a new source file is as simple
as declaring it in the application specific <code>makefile</code>, as shown in
the following figure:

@code
    # You can add more sources here if needed
    SRCS += $(APP_SRCS_PATH)new_source.c
    INCLUDES +=
@endcode

The file named <code>new_source.c</code> is created alongside <code>app.c</code>
in this example.

By default, the <code><app_folder>/include</code> folder is added to the list of
paths to check for header files. Any additional folders can be added to
the <code>INCLUDES</code> variable in the application <code>makefile</code>.

@section recommendations Recommendations

This chapter contains various recommendations and best practices to use
with application development.

@subsection security_for_scratchpad_images Security for scratchpad images

All scratchpad images received <I>Over The Air</I> are compressed and
encrypted.

The bootloader authenticates the image and decrypts it, so
authentication and encryption keys used to generate a scratchpad must
match the ones stored in the bootloader.

The list of keys stored in bootloader can be configured at the end of
<code>tools/scratchpad_<mcu>.ini</code> file. The bootloader can contain several
keys and will decrypt a received scratchpad, if it can be authenticated
with any of the listed keys.

By default, each application uses the same <code>.\ini</code> file stored in
</code>tools/scratchpad_<mcu>.ini</code> but it can be copied to the application
folder and modified as needed. The new <code>.ini</code> file can be specified
from the application <code>config.mk</code> as following:

@code
    # Define a specific scratchpad ini

    INI_FILE = $(APP_SRCS_PATH)/scratchpad_custom.ini
@endcode

The scratchpad generated from an application build use the key named
</code>default</code> from the <code>ini</code> file. It can be changed by
adding the </code>--keyname</code> option to the invocation of
<code>genscratchpad.py</code> script from makefile.

<table style="width:0%">
<tr><td>\image html alert.jpg ""</td><td>All released SDK have the same default
keys (known by all customers), so these keys cannot be considered secure.</td>
</tr>
</table>

It is highly recommended to modify these keys before any deployment, to
reduce the risk of malicious network access.

@subsection optimization_of_network_throughput Optimization of network
throughput

The throughput of a Wirepas Connectivity network is expressed in packet
per seconds. To optimize this throughput, it is important to fill the
packet to the maximum available PDU size when possible.

It is even more important when operating in Low Energy mode. The network
will handle the same number of packets independently of its payload
size.

@subsection free_resources Free resources

All hardware resources that are not used by the Wirepas Connectivity
stack can be used freely by the application.

<table style="width:0%">
<tr><td>\image html alert.jpg ""</td><td>All the hardware that is not used by
the stack is left in its initial boot state. It must be configured by the
application as needed.</td></tr>
</table>

For example, unused GPIOs must be properly configured by the
application, to avoid unnecessary power consumption due to pull-up or
pull-down resistors.

@subsection power_consumption Power consumption

The Wirepas Connectivity stack will try to enter the deepest possible
sleep state of the platform, to optimize power consumption.

But as the application may require staying in a higher power state (to
keep a peripheral clock enabled for example), the application can ask
the stack to prevent entering the deep sleep state.

Please see the \ref app_lib_system_disable_deep_sleep
"lib_system->disableDeepSleep()" function in the \ref system.h "System library".


Related Material
================

@anchor relmat1 [1] WP-RM-108 - OTAP Reference Manual

@anchor relmat4 [4] WP-RM-100 - Wirepas Connectivity Dual-MCU API Reference
Manual

@anchor relmat7 [7] WP-RM-117 - Wirepas Connectivity Remote API Reference Manual


 */

#endif /* API_DOC_HOW_TO_DEVELOP_H_ */
