
Release Note:

Central release version: SL_V4.0_Central_01.00.00

V4.0 - Hardware version
01.00.00 - Firmware version

Below are the firmware changes on Gecko central:
 
1.On power ON, led will blink.
2.On receiving a connect request from Cubi board, the LED will be switched ON.
3.When Gecko central sends a connect response to Cubi board, LED will be switched OFF.
4.On receiving connect request, the central won't send the advertisement packets.
5.Added uart test command, on receiving this command central will echo same to cubi as response.

Hardware changes on gecko central

Gecko's reset pin is connected to Cubie's IO pin. Wire is connected between Cubietruck - CN9.7 and Gecko central - TP19