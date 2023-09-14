
#ifndef _APPLICATION_API_H_
#define _APPLICATION_API_H_

/**

@page programming_interface Application API

@section application_api Application API

The Wirepas Mesh stack (hereafter referred to as the *stack*) runs on ARM Cortex
M based microcontrollers. Hence, the stack and the application both follow the
Procedure Call Standard for the ARM Architecture. The stack and the application
communicate with each other by calling functions on each other.

The protocol stack is in control of program execution. Application can request
to be called periodically, or when certain events happen. Applications can also
directly access hardware peripherals that are not used by the stack.
Applications can have interrupt handlers, but the service routines must be kept
short so they do not interfere with the timing of the Wirepas Mesh protocol.

To keep features in logically separate units, stack functions are collected into
<I>libraries</I>. The application opens each library it needs and calls
functions in the library via function pointers. A global list of functions is
given to the application when it is first called, so that it can open libraries
and find out details about the environment in which it is running. All libraries
exist on <code>@ref api "api/"</code> directory.

@subsection app_init Application startup

The Wirepas Mesh Single-MCU SDK low-level initialization code sets up the
application environment to run C code. The low-level setup is outside the scope
of this document, but once the setup is done, the application initialization
function <code>App_init()</code> will be run::

@code
    void App_init(const app_global_functions_t * functions)
    {
        ...
    }
@endcode

This <I>entry point function</I> <B>must</B> be implemented in every
application!

It is the responsibility of <code>App_init()</code> to open any libraries
needed, register any callbacks and initialize hardware. There is no return value
from <code>App_init()</code>. The sole parameter functions is a global list
of function pointers for the application.

The stack is not yet running when <code>App_init()</code> is called.
Depending on the stored settings and stack state, the stack may start right
after returning from <code>App_init()</code>.

@subsection application_and_library_versioning Application and Library
Versioning

To keep features in logically separate units and allow each unit to be updated
in a *backward- and forward-compatible* manner, stack functions are collected
into *libraries*. When an application requests a specific version of a library
and no exact version is found, the stack can do one of three things:

1. *Return a newer, compatible library*: This works if the newer library version
   has new features that have been added in such a way that the old API still
   works as expected. This is ideally the way new firmware releases add features
   to the libraries.

2. *Emulate the old library*: If the new firmware has the library, but it has an
   incompatible API, it is in theory possible to detect when an application
   calls <code>@ref app_open_library_f "openLibrary()"</code>
   with an old version number and return a special emulated version of the old
   library, which just calls functions in the new library. This solution is to
   be reserved for special cases only.

3. *Fail to open the library*: Old, obsoleted features can be phased out by
   simply failing to open a library that is too old. Stack firmware release
   notes will list the obsoleted libraries for each release.

In addition of library versioning, applications also have an API version number,
@ref APP_API_VERSION, placed in their header. The SDK places the version number
there to indicate which version of the low-level application API the application
supports. If there is a mismatch between the low-level API versions of the stack
and the application, the stack may choose to not run the application.

 */

#endif /* API_DOC_APPLICATION_API_H_ */
