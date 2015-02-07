![LLSIF Launcher](http://hnng.moe/f/sM)

[Video demonstration](http://hnng.moe/f/sN)

Introduction
==============
This is a little tool I made for School Idol Festival (android version) 
that allows you to easily manage your multiple accounts and log into the game with one of 
your accounts in a single click.
I only tested this in the Bluestacks emulator, but it should also work on a real device.

NOTE: the binaries are packed with UPX to minimize the size, and some 
anti-viruses might give you a false positive (although it's very unlikely).

Requirements
==============
Before you attempt using this tool, make sure you have the 
[Visual C++ Redist 2013](http://www.microsoft.com/it-it/download/details.aspx?id=40784).
Also, your emulator or device needs to be rooted.

First time set-up
==============
(you can ignore this if you already have xposed + rootcloak)
* Install the Xposed Framework (version 2.4.1) on your device/emulator.
  Download [this apk](http://dl-xda.xposed.info/modules/de.robv.android.xposed.installer_v25_36cbbc.apk).
  If you're doing this on a real device, you need to simply install the apk.
  If you're using the Bluestacks emulator, you will need to download BSMultiTool 
    from [here](http://www.mediafire.com/download/xmzvr6oo2oc62fe/BSMultiTool.rar) 
    which also includes a version of bluestacks that is guaranteed to work 
    (in the Installation folder).
    I highly recommend that you use the included version of bluestacks and root it 
    by following [this guide](http://www.thewally1987.com/how-to-root-bluestacks-2014.html). 
    Once you have BSMultiTool, copy the xposed apk you downloaded earlier to the 
    "Root Files" folder in BSMultiTool's folder and run BSMultiTool.exe
    Select the "X" option (xposed) and follow the instructions.
* Start the Xposed app and click Install/Update.
* Once it's done, click soft reboot.
* Install 
  [RootCloak Module for Xposed (version 1.3)](http://dl-xda.xposed.info/modules/com.devadvance.rootcloak_v4_5a7037.apk) 
  on your device/emulator.
* Open Xposed and click Modules.
* Tick "RootCloak [1.3]".
* Open RootCloak.
* Go to Add/Remove Apps.
* Click the + sign in the top right.
* Scroll down to SchoolIdol (klb.android.lovelive_en) and tap it.
* Reboot your device/emulator and make sure that the game still starts without any errors. 
  If not, double check the first time set-up instructions.
* Install "WiFi ADB" from the app store and make sure your device / emulator is connected 
  to the internet through the same network as your computer.
* Start WiFi ADB and turn the switch on.

Usage:
==============
(note: this assumes that you already know how multi-accounting works and you have all your 
 renamed GameEngineActivity.xml's in a folder)
(warning: this tool is designed not to erase your xml's but you should still make a back-up 
 and save your transfer codes in case something goes wrong)
- Start llsif_launcher and insert the full path of the folder where you stored your xml's 
  on your device.
- Click Load.
- Double click the desired xml or select and click start.
- The tool will automatically create a symbolic link to your xml and start the game without 
  renaming or moving any file.
- You can quickly terminate the game by clicking "Kill".

Advanced Users:
==============
If for some reason you don't want to use WiFi ADB or you need to change the ip it connects 
to because you want to use it on a real device, you can change the ConnectCommand parameter 
in settings.ini

Troubleshooting:
==============
If the tool can't seem to get the file list and you're 100% sure the path you provided is 
correct, try running fix_adb.bat and trying again. It will reset the adb connection.
If it still doesn't work, restart bluestacks and terminate adb from your task manager.

Source code:
==============
It's written in pure C + Win32API. The internal library I wrote to control LLSIF through adb 
is cross-platform and you can re-use it to make a linux frontend if you wish.
If you wish to play around with the source code, you can grab it at: 
[https://github.com/Francesco149/llsif_launcher](https://github.com/Francesco149/llsif_launcher)

Legal stuff:
==============
The copies of adb.exe, AdbWinApi.dll or AdbWinUsbApi.dll I shipped with this tool are made 
by Google and I do not own them.

Copyright 2014 Franc[e]sco (lolisamurai@tfwno.gf)
This file is part of llsif_launcher.
llsif_launcher is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
llsif_launcher is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with llsif_launcher. If not, see <http://www.gnu.org/licenses/>.
