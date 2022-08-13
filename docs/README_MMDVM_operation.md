
MMDVM operation mode
=========

QRadioLink can act as a SDR base station transceiver for DMR, System Fusion and D-Star
by connecting to mmdvm-sdr, a fork of MMDVM which runs on Linux operating systems.
QRadioLink can work either as a normal single channel transceiver or as a
multi-channel / multi-carrier transceiver. So far only DMR and System Fusion have been tested
and confirmed to work. System Fusion may not work properly yet in multi-carrier mode.
Multi-carrier and single carrier DMR is only supported with LimeSDR devices so far.
If you are interested in support for other devices like Ettus USRP or BladeRF please create a
pull request.

Installation
=======

1. QRadioLink.
Install normally (use the `next` git branch)

2. MMDVM
<pre>
git clone https://github.com/qradiolink/mmdvm-sdr
cd mmdvm-sdr
git checkout tdma_multi
mkdir build && cd build
cmake ..
make -j 2
</pre>

3. MMDVMHost
<pre>
git clone https://github.com/qradiolink/MMDVMHost-SDR
cd MMDVMHost-SDR
git checkout mmdvm_sdr_multi
make -j 4
</pre>

Running the transceiver
=======

1. QRadioLink
* Start the application normally. Configure the LimeSDR device, as well as all other options.
Use a device string like **driver=lime,serial=00583A34CAD205** (replace serial with your own).
You can find out the serial number by running:
<pre>
LimeUtil --find
</pre>
Click save to write the settings in the config file.

* Configure the RX frequency which will be the uplink from the radio. Set the TX offset (Split) such that the TX frequency of the SDR will be the RX frequency of the radio. Multi-carrier operation requires no other adjustment. Carrier number zero is on the TX frequency, while carrier 2, 3 etc. will be 25 and 50 kHz above the carrier 0 frquency. For example, a split of 3000 is a positive TX offset of 3 MHz.
* Set TX and RX gain as desired. It is recommended to start with a lower RX gain.
* Enable the Duplex button.
* Choose as operating mode either **MMDVM** or **MMDVM multi**
* Close the application, all the settings will be saved in the config
* Alternatively all the settings can be instead directly written in the config file which is located in:
**~/.config/qradiolink/qradiolink.cfg**
* Finally, start the application from the CLI using the **--mmdvm** switch:
<pre>
./qradiolink --mmdvm
</pre>
If everything is configured ok, you should see the SDR initialization and then continuous updates about FIFO size.

2. MMDVM
* From the build/ directory, start mmdvm in single channel mode using:
<pre>
./mmdvm
</pre>
This will create the symlink to the virtual pts, which you will need to put into MMDVM.ini, section Modem:
<pre>
M: 2022-08-13 11:37:34.779 virtual pts: /dev/pts/15 <> /home/user1/mmdvm-sdr/build/ttyMMDVM0
</pre>
* If you have started in single channel mode, the tty will be called **ttyMMDVM0**
* Alternatively, you can start multiple instances of mmdvm in multi channel mode, if that mode is also configured in QRadioLink. Start multiple mmdvm in different terminals like this (up to three channels so far):
<pre>
./mmdvm -c 1
./mmdvm -c 2
./mmdvm -c 3
</pre>
* Verify in mmdvm output that the ttyMMDVM has the correct number (1 to 3 in above example)

3. MMDVMHost
* Configure MMDVM.ini first. If using in multi-channel mode, create multiple files, for example MMDVM1.ini, MMDVM2.ini, MMDVM3.ini
* Each MMDVM.ini has to contain a unique identifier for the virtual pts. In single channel mode the name will always be ttyMMDVM0, otherwise it will be numbered from 1 to 3 depending on the id given after the **-c** switch. The path of the symlink will be located in the mmdvm-sdr build directory if started from there.
* Example config for DMR below:
<pre>
[General]
Callsign=YOUR_CALLSIGN
Id=YOUR_DMR_ID
Timeout=3600
Duplex=1
ModeHang=360
RFModeHang=360
NetModeHang=360
Display=None
Daemon=0

[Info]
RXFrequency=431500000
TXFrequency=433500000
Power=1
# The following lines are only needed if a direct connection to a DMR master is being used
Latitude=43.000
Longitude=22.000
Height=1
Location="My location"
Description="hotspot"
URL=dstar.su

[Log]
# Logging levels, 0=No logging
DisplayLevel=1
FileLevel=1
FilePath=.
FileRoot=MMDVM
FileRotate=1

[CW Id]
Enable=0
Time=10
Callsign=YOUR_CALLSIGN

[DMR Id Lookup]
File=DMRIds.dat
Time=24

[NXDN Id Lookup]
File=NXDN.csv
Time=24

[Modem]
Port=/home/user1/mmdvm-sdr/build/ttyMMDVM1
Protocol=uart
# Address=0x22
TXInvert=1
RXInvert=1
PTTInvert=0
TXDelay=0
RXOffset=0
TXOffset=0
DMRDelay=0
RXLevel=70
TXLevel=100
RXDCOffset=0
TXDCOffset=0
RFLevel=100
# CWIdTXLevel=50
# D-StarTXLevel=50
# DMRTXLevel=50
# YSFTXLevel=50
# P25TXLevel=50
# NXDNTXLevel=50
# POCSAGTXLevel=50
# FMTXLevel=50
RSSIMappingFile=RSSI.dat
UseCOSAsLockout=0
Trace=0
Debug=3

[Transparent Data]
Enable=0
RemoteAddress=127.0.0.1
RemotePort=40094
LocalPort=40095
# SendFrameType=0

[UMP]
Enable=0
# Port=\\.\COM4
Port=/dev/ttyACM1

[D-Star]
Enable=0
Module=C
SelfOnly=0
AckReply=1
AckTime=750
AckMessage=0
ErrorReply=1
RemoteGateway=0
# ModeHang=10
WhiteList=

[DMR]
Enable=1
Beacons=1
BeaconInterval=30
BeaconDuration=10
ColorCode=1
#SelfOnly=0
#EmbeddedLCOnly=0
DumpTAData=0
Id=YOUR_DMR_ID
CallHang=7
TXHang=30
ModeHang=60
# OVCM Values, 0=off, 1=rx_on, 2=tx_on, 3=both_on, 4=force off
# OVCM=0

[System Fusion]
Enable=0
LowDeviation=0
SelfOnly=0
TXHang=4
RemoteGateway=0
# ModeHang=10

[P25]
Enable=0
NAC=293
SelfOnly=0
OverrideUIDCheck=0
RemoteGateway=0
TXHang=5
# ModeHang=10

[NXDN]
Enable=0
RAN=1
SelfOnly=0
RemoteGateway=0
TXHang=5
# ModeHang=10

[POCSAG]
Enable=0
Frequency=439987500

[FM]
Enable=0
Callsign=YOUR_CALLSIGN
CallsignSpeed=20
CallsignFrequency=1000
CallsignTime=10
CallsignHoldoff=0
CallsignHighLevel=50
CallsignLowLevel=20
CallsignAtStart=1
CallsignAtEnd=1
CallsignAtLatch=0
RFAck=K
ExtAck=N
AckSpeed=20
AckFrequency=1750
AckMinTime=4
AckDelay=1000
AckLevel=50
# Timeout=180
TimeoutLevel=80
CTCSSFrequency=88.4
CTCSSThreshold=30
# CTCSSHighThreshold=30
# CTCSSLowThreshold=20
CTCSSLevel=20
KerchunkTime=0
HangTime=7
AccessMode=1
COSInvert=0
RFAudioBoost=1
MaxDevLevel=90
ExtAudioBoost=1

[D-Star Network]
Enable=0
GatewayAddress=127.0.0.1
GatewayPort=20010
LocalPort=20011
# ModeHang=3
Debug=0

[DMR Network]
Enable=1
# Type may be either 'Direct' or 'Gateway'. When Direct you must provide the Master's
# address as well as the Password, and for DMR+, Options also.
Type=Direct
Address=master.dmr.server.address
Port=62031
Local=62033
Jitter=300
Password=password
# Options=
Slot1=1
Slot2=1
# ModeHang=3
Debug=0

[System Fusion Network]
Enable=0
LocalAddress=127.0.0.1
LocalPort=3200
GatewayAddress=127.0.0.1
GatewayPort=4200
# ModeHang=3
Debug=0

[P25 Network]
Enable=0
GatewayAddress=127.0.0.1
GatewayPort=42020
LocalPort=32010
# ModeHang=3
Debug=0

[NXDN Network]
Protocol=Icom
Enable=0
LocalAddress=127.0.0.1
LocalPort=14021
GatewayAddress=127.0.0.1
GatewayPort=14020
# ModeHang=3
Debug=0

[POCSAG Network]
Enable=0
LocalAddress=127.0.0.1
LocalPort=3800
GatewayAddress=127.0.0.1
GatewayPort=4800
# ModeHang=3
Debug=0

[TFT Serial]
# Port=modem
Port=/dev/ttyAMA0
Brightness=50

[HD44780]
Rows=2
Columns=16

# For basic HD44780 displays (4-bit connection)
# rs, strb, d0, d1, d2, d3
Pins=11,10,0,1,2,3

# Device address for I2C
I2CAddress=0x20

# PWM backlight
PWM=0
PWMPin=21
PWMBright=100
PWMDim=16

DisplayClock=1
UTC=0

[Nextion]
# Port=modem
Port=/dev/ttyAMA0
Brightness=50
DisplayClock=1
UTC=0
#Screen Layout: 0=G4KLX 2=ON7LDS
ScreenLayout=2
IdleBrightness=20

[OLED]
Type=3
Brightness=0
Invert=0
Scroll=1
Rotate=0
Cast=0
LogoScreensaver=1

[LCDproc]
Address=localhost
Port=13666
#LocalPort=13667
DimOnIdle=0
DisplayClock=1
UTC=0

[Lock File]
Enable=0
File=/tmp/MMDVM_Active.lck

[Remote Control]
Enable=0
Address=127.0.0.1
Port=7642
</pre>

* Start MMDVMHost using the desired name of the config file. If using multi-channel mode,
start multiple instances of MMDVMHost from different terminals using the corresponding ini config files. Example for single channel mode:
<pre>
./MMDVMHost MMDVM.ini
</pre>
* Example for multi-channel mode:
<pre>
./MMDVMHost MMDVM1.ini
./MMDVMHost MMDVM2.ini
./MMDVMHost MMDVM3.ini
</pre>

* You can configure different networks or combination of talkgroups for each channel.
