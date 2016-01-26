USB Emulation In Python With No Additional Hardware
=============

Hack fixes for mouse hid work with Linux 4.2 (and Windows 10)

keyboard hid added.

in Linux use:<br>
  python hid-xxxx.py<br>
  and<br>
  usbip attach -r 127.0.0.1 -b 1-1<br>
  (remember to load kernel modules usbip-core and vhci-hcd)<br>   

for list device use:<br>
  usbip list -r 127.0.0.1 

More details on: <br>
http://breaking-the-system.blogspot.com/2014/08/emulating-usb-devices-in-python-with-no.html <br>

This code uses the USB/IP project: http://usbip.sourceforge.net/ to emulate usb devices. <br>
In this example I showed how to emulate HID mouse device. <br>
This code can be easily changed to support any usb device. <br>
This code is very experimental, use with care. <br>
In the HID example there is a bug with USB/IP driver that causes blue screen on de-attach: <br>
http://sourceforge.net/p/usbip/discussion/418507/thread/7ff86875/?page=0 <br>
It doesnt happen with other devices I tested. <br>


