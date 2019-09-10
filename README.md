# Network_System_For_STM32F4
This is an embedded network system for STM32F4 realizing remote control for STM32F4 based devices.
-------------------------------------------------------------------------------
The project file is in "LWIP_TCP_MPU_CLIENT_FOR_STM32F4_V2\Project\LWIT_TCP_MPU_Client.uvprojx", make sure the debug configuration is correct before downloading the code.
The hardware connection is described in "Explorer STM32F4_V2.2_SCH.pdf"
-------------------------------------------------------------------------------
Version 0.9:
In this version, the network system only supports static IP addressing, which means the client IP address is fixed at 192.168.1.30.

This system support TCP connection and the STM32 device serves as TCP client.
For the remote TCP server, we need the corresponding IP address so the STM32 device is able to connect. Additionally, we need a network debugger supporting TCP connection.

For the STM32 device, we follow the instruction in the screen to start connection, firstly use "KEY_UP" and "KEY0" to set remote (server's) IP address, and press "KEY0" to select and connect. During that progress, we can terminate the connection by pressing "KEY_UP". Also, we can restart connection by pressing "KEY1" if the connection is off.

In the server side, we have a brief instruction set controlling the device. Sending an instruction, the device makes the corresponding action, this instruction can be viewed in /Project/Instruction_set.txt

----------------------------------------------------------------------------
