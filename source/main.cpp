/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"
#include "MicroBitI2C.h"

#define MES_DPAD_CONTROLLER                 1104
#define MES_DPAD_1_BUTTON_UP_ON             1
#define MES_DPAD_1_BUTTON_UP_OFF            2
#define MES_DPAD_1_BUTTON_DOWN_ON           3
#define MES_DPAD_1_BUTTON_DOWN_OFF          4
#define MES_DPAD_1_BUTTON_LEFT_ON           5
#define MES_DPAD_1_BUTTON_LEFT_OFF          6
#define MES_DPAD_1_BUTTON_RIGHT_ON          7
#define MES_DPAD_1_BUTTON_RIGHT_OFF         8
#define MES_DPAD_2_BUTTON_UP_ON             9
#define MES_DPAD_2_BUTTON_UP_OFF            10
#define MES_DPAD_2_BUTTON_DOWN_ON           11
#define MES_DPAD_2_BUTTON_DOWN_OFF          12
#define MES_DPAD_2_BUTTON_LEFT_ON           13
#define MES_DPAD_2_BUTTON_LEFT_OFF          14
#define MES_DPAD_2_BUTTON_RIGHT_ON          15
#define MES_DPAD_2_BUTTON_RIGHT_OFF         16

#define MOTER_ADDRESSS                      0x20

enum aMotors
{
    //% blockId="M1" block="M1"
    M1 = 0,
    //% blockId="M2" block="M2"
    M2 = 1
};

enum dir
{
    //% blockId="CW" block="CW"
    CW = 0x0,
    //% blockId="CCW" block="CCW"
    CCW = 0x1
};

enum LED
{
    //% blockId="LEDLeft" block="LEDLeft"
    ledLeft = 8,
    //% blockId="LEDRight" block="LEDRight"
    ledRight = 12
};

enum ledSwitch
{
    //% blockId="turnOn" block="turnOn"
    turnOn = 0x01,
    //% blockId="turnOff" block="turnOff"
    turnOff = 0x00
};

MicroBit uBit;

// we use events abd the 'connected' variable to keep track of the status of the Bluetooth connection
void onConnected(MicroBitEvent)
{
    uBit.display.print("C");
}

void onDisconnected(MicroBitEvent)
{
    uBit.display.print("D");
}

void writeLED(int led, int ledSwitch)
{
    if (led == ledLeft)
    {
        uBit.io.P8.setDigitalValue(ledSwitch);
      
    }
    else if (led == ledRight)
    {
        uBit.io.P12.setDigitalValue(ledSwitch);
    }
    else
    {
    };
}

void motorRun(int index, int dir, uint8_t speed)
{
    char buf[3];
    int ret;

    if (index == 0)
    {
        buf[0] = 0x00;
    }
    if (index == 1)
    {
        buf[0] = 0x02;
    }
    buf[1] = dir;
    buf[2] = speed;

    ret = uBit.i2c.write(MOTER_ADDRESSS, buf, 3);
    if (ret == MICROBIT_OK)
        uBit.display.print("O");
    else if (ret == MICROBIT_I2C_ERROR)
        uBit.display.print("E");
    else
    {
    };
}

void motorStopAll()
{
    char buf[3];

    buf[1] = 0;
    buf[2] = 0;
    buf[0] = 0x00;
    uBit.i2c.write(MOTER_ADDRESSS, buf, 3);

    buf[0] = 0x02;
    uBit.i2c.write(MOTER_ADDRESSS, buf, 3);
}

void onControllerEvent(MicroBitEvent e)
{
    switch (e.value)
    {
    case MES_DPAD_1_BUTTON_UP_ON:
        uBit.display.print("F");
        motorRun(M1, CW, 50);
        motorRun(M2, CW, 50);
        break;

    case MES_DPAD_1_BUTTON_UP_OFF:
        motorStopAll();
        break;

    case MES_DPAD_1_BUTTON_DOWN_ON:
        uBit.display.print("D");
        motorRun(M1, CCW, 50);
        motorRun(M2, CCW, 50);
        break;

    case MES_DPAD_1_BUTTON_DOWN_OFF:
        motorStopAll();
        break;

    case MES_DPAD_1_BUTTON_LEFT_ON:
        uBit.display.print("L");
        writeLED(ledLeft, turnOn);
        motorRun(M2, CW, 50);
        break;

    case MES_DPAD_1_BUTTON_LEFT_OFF:
        motorStopAll();
        break;

    case MES_DPAD_1_BUTTON_RIGHT_ON:
        uBit.display.print("R");
        writeLED(ledRight, turnOn);
        motorRun(M1, CW, 50);
        break;

    case MES_DPAD_1_BUTTON_RIGHT_OFF:
        motorStopAll();
        break;

    case MES_DPAD_2_BUTTON_LEFT_ON:
        writeLED(ledLeft, turnOff);
        break;

    case MES_DPAD_2_BUTTON_RIGHT_ON:
        writeLED(ledRight, turnOff);
        break;

    default:
        break;
    }

}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    // Configuration Tips
    //
    // config.json contains various Bluetooth related properties some of which are explained here:
    //
    // "dfu_service": 1,               // 1 will cause the DFU service to be instantiated
    // "event_service": 1,             // 1 causes the event service to be instantiated
    // "device_info_service": 1
    // "enabled": 1,                   // 1 means the Bluetooth stack will be included as standard. 0 means it will not.
    // "pairing_mode": 1,              // 1 means it's possible to go into pairing mode which will include bringing up the Bluetooth stack whilst in that mode.
    // "open": 0,                      // 1 means there's no Bluetooth security i.e. no need to pair the micro:bit with other devices that want to communicate with it.
    // "tx_power": 7,                  // Transmission power of the Bluetooth radio. A value of 0 - 7 with 0 the lowest power and 7 the highest power.
    // "gatt_table_size": "0x700"      // Amount of memory (in hex bytes) set aside for the Bluetooth GATT table
    // "nested_heap_proportion": 0.75, // Reducing this can sometimes help make enough memory available for all the Bluetooth services you want. Only experiment with this as a last resort.

    // MicrobitConfig.h in yotta_modules\microbit-dal\inc\core contains MICROBIT_BLE_SECURITY_LEVEL which can be set to SECURITY_MODE_ENCRYPTION_WITH_MITM for passkey authentication when
    // pairing or SECURITY_MODE_ENCRYPTION_NO_MITM to use Just Works pairing.

    // A cunning code to indicate during start-up the particular Bluetooth configuration in the build
    //
    // SERVICE CODES
    // A: Accelerometer Service
    // B: Button Service
    // D: Device Information Service
    // E: Event Service
    // F: DFU Service
    // I: IO Pin Service
    // L: LED Service
    // M: Magnetometer Service
    // T: Temperature Service
    // U: UART Service
    //
    // PAIRING CONFIG
    // Note that switching pairing on or off is achieved by setting "open" in config.json to 1 or 0 respectively

    // P: PASSKEY
    // J: Just Works
    // N: No Pairing Required
    //
    // TX Power Level
    // 0-7 taken from tx_power in config.json


    // Services/Pairing Config/Power Level
    uBit.display.scroll("Hello World!");

    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(MES_DPAD_CONTROLLER, 0, onControllerEvent); 

    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}
