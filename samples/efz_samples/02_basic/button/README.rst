.. zephyr:code-sample:: button
   :name: Button
   :relevant-api: input_interface

   Handle GPIO button inputs using the input subsystem.

Overview
********

A simple button demo showcasing the use of buttons with the :ref:`input` APIs.
The sample prints a message to the console each time a button is pressed.

Requirements
************

The board hardware must have a device node capable of generating input KEY
events, typically a push button connected via a GPIO pin and defined in a
"gpio-keys" node. These are called "User buttons" on many of Zephyr's
:ref:`boards`.

The sample additionally supports an optional ``led0`` devicetree alias. If this
is provided, the LED will be turned on when the button is pressed, and turned
off when it is released.

Building and Running
********************

Build and flash it for the EFZ-ESP32S3 (on the ESP32-S3-DevKitC, use
``esp32s3_devkitc/esp32s3/procpu``):

.. zephyr-app-commands::
   :zephyr-app: samples/efz_samples/02_basic/button
   :board: efz_esp32s3/esp32s3/procpu
   :goals: build flash
   :compact:
