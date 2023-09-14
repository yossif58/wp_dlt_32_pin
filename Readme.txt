/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

This SDK targets the board(s) brd4158a.

Some applications uses GPIOs that may be different if you are using a different
board than the one specified above.

You can add a new board by defining your own board.h in board/<new_board>/board.h

This SDK contains multiple applications that you can build with the following
command:

	> make app_name=<name of the app folder under source folder> target_board=<name of the board>

Some application may not be available for some board (if a given sensor is not present for example).

Each application must have its own app_area_id. It allows multiple applications
to coexist in the same network and avoid mismatch during OTAP.
To modify the app_area_id of an application, app_specific_area_id variable must
be set in application config.mk file. It is a 24 bits variable and its most
significant bit must be one.

For example:
	app_specific_area_id=0x800001

The full app_area_id is a 32 bits value. The 8 less significant bits are
automatically added by the build system and dependant on the MCU. It avoids
mismatch during OTAP when updating network with different hardware.

List of application and their default area id:

 - appconfig_app
	app_area_id = 0x84BEBD08 compatible with ['brd4158a'] 
 - dualmcu_app
	app_area_id = 0x846B7408 compatible with ['brd4158a'] 
 - custom_app
	app_area_id = 0x83744C08 compatible with ['brd4158a'] 
 - minimal_app
	app_area_id = 0x8CEC7908 compatible with ['brd4158a'] 
