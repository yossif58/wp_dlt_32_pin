
#ifndef SOURCE_APP_DOC_H_
#define SOURCE_APP_DOC_H_

/**
@mainpage Single-MCU Operation Overview

The Single-MCU operation allows an application to run on the same
chip with Wirepas Mesh stack.

@htmlonly
<img src="image2.png" usemap="#image2map">
<map name="image2map">
<area shape="rect" coords="360,0,1117,90" href="#application_firmware"></area>
<area shape="rect" coords="360,0,536,559" href="#application_firmware"></area>
<area shape="rect" coords="360,571,538,663" href="#application_hal"></area>
<area shape="rect" coords="548,157,1117,666" href="#stack"></area>
<area shape="rect" coords="540,93,1117,150" href="#single_mcu_api"></area>
<area shape="rect" coords="361,667,1117,765" href="#phy"></area>
</map>
@endhtmlonly

Main components are following:

@section application_firmware Application firmware

Application firmware includes the application logic. There can be multiple
applications (i.e. different <I>kind of devices</I>) in the same network. For
example lighting network may contain lighting control switches and LED drivers.

With the provided SDK, a customer can write own application, build it,
and update a Wirepas Mesh mesh network with the generated image
(application + Wirepas Mesh Stack).

@ref application_operation "Single-MCU API Operation Principle" describes
  the operation principle of the interface between the application and
  Wirepas Mesh stack. Memory partitioning and different regions are
  explained are described. Different ways the application is scheduled and the
  MCU accessed cooperatively.

@ref sdk_environment "SDK Environment" describes the SDK package contents
  and available free processor resources for application.


@ref how_to_develop "How to develop applications with Single-MCU SDK"
  describes guidance to write first application and various tips and
  recommendations for application development.



@section application_hal Application-specific Hardware Abstraction Layer (HAL)

This is commonly various software components for peripheral usage, such as
sensors/actuators and communication interfaces. Some of those are delivered as
part of the SDK. This also contains drivers made by user of the SDKs, commonly
shared between different _applications.

For HAL services offered by the SDK, see @ref mcu/hal_api "hal_api".

@section stack Wirepas Mesh stack
Stack includes the Wirepas Mesh communication
stack and Wirepas scheduler for enabling the application operation in the same
MCU. Wirepas Mesh HAL includes all hardware abstractions and drivers <I>needed by
the stack</I>, such as a radio driver. Note that drivers for perhipherals that
are not needed by the stack itself, are not implemented in stack.

Wirepas Mesh scheduler provides priority-based cooperative scheduling,
i.e. all the tasks are run to completion. The tasks are scheduled based
on their priorities and their execution times. The Wirepas Mesh stack
has strict real-time requirements (accurate synchronization of
messaging) and has the highest priority. Thus, it is not recommended to
do processing intensive (time consuming) tasks on the application side.
Real-time guarantees are not provided to the application.

@section single_mcu_api Wirepas Mesh Single-MCU API

Wirepas Mesh stack provides Wirepas Mesh Single-MCU API for application to
use <I>stack services</I> and run tasks on the MCU.

@ref programming_interface "Application API" describes the programming
  interface.


@section phy The physical layer (PHY)

This includes all the hardware of the device including the processor core,
radio for wireless communication and application-specific peripherals.

Peripherals can be grouped into three categories:
-# Peripherals used solely by the Wirepas Mesh Stack
-# Peripherals used solely by the application and
-# Peripherals shared between Wirepas Mesh Stack and application.

For details on this, check @ref efr32xg12 "EFR32 resources" or @ref nordic_nrf52832_nrf52840
"Nordic nRF52XXX resources" according to your architecture.


#endif /* SOURCE_APP_DOC_H_ */
