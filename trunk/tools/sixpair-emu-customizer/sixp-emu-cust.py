import Tkinter
from Tkinter import *
import tkMessageBox
import usb
import usb.core
import os
import sys
import struct
from array import array
if sys.platform == "win32":
    import bluetooth as _bt
elif sys.platform == "linux2":
    import bluetooth._bluetooth as _bt

usb_devs = []
bd_devs = []

def read_usb_devices():
	buses = usb.busses()
	for bus in buses :
		for device in bus.devices :
			usb_devs.append((device.idVendor, device.idProduct))

def usb_set(didVendor, didProduct, idVendor, idProduct, bdaddr):
	dev = usb.core.find(idVendor=didVendor, idProduct=didProduct)
	if dev is None:
		tkMessageBox.showerror("Device not connected", "Vendor Id: 0x%04x\nProduct Id: 0x%04x" % (didVendor, didProduct))
		return
	try:
		dev.detach_kernel_driver(0)
	except:
		pass
	try:
		read = dev.ctrl_transfer(bmRequestType=0xA1, bRequest=0x01, wValue=0xdead, data_or_wLength=10)
	except:
		tkMessageBox.showerror("Device not connected", "Vendor Id: 0x%04x\nProduct Id: 0x%04x" % (didVendor, didProduct))
		return
	myArray = array('B', [idVendor&255, idVendor/256, idProduct&255, idProduct/256])
	myArray.fromlist(bdaddr)
	if myArray.tolist() != read.tolist():
		ret1 = dev.ctrl_transfer(bmRequestType=0x21, bRequest=0x09, wValue=0xdead, \
            data_or_wLength=myArray)
		if ret1 !=  10:
			tkMessageBox.showerror("Device error", "Set Request Failed!")
		else:
			tkMessageBox.showinfo("Device info", "Set Request Successful!\nUnplug The Device!")
	else:
		tkMessageBox.showwarning("Device info", "Nothing Changed!")

def read_local_bdaddr(hci_sock):
	old_filter = hci_sock.getsockopt( _bt.SOL_HCI, _bt.HCI_FILTER, 14)
	flt = _bt.hci_filter_new()
	opcode = _bt.cmd_opcode_pack(_bt.OGF_INFO_PARAM, 
			_bt.OCF_READ_BD_ADDR)
	_bt.hci_filter_set_ptype(flt, _bt.HCI_EVENT_PKT)
	_bt.hci_filter_set_event(flt, _bt.EVT_CMD_COMPLETE);
	_bt.hci_filter_set_opcode(flt, opcode)
	hci_sock.setsockopt( _bt.SOL_HCI, _bt.HCI_FILTER, flt )

	_bt.hci_send_cmd(hci_sock, _bt.OGF_INFO_PARAM, _bt.OCF_READ_BD_ADDR )

	pkt = hci_sock.recv(255)

	status,raw_bdaddr = struct.unpack("xxxxxxB6s", pkt)
	assert status == 0

	t = [ "%02X" % ord(b) for b in raw_bdaddr ]
	t.reverse()
	bdaddr = ":".join(t)

	# restore old filter
	hci_sock.setsockopt( _bt.SOL_HCI, _bt.HCI_FILTER, old_filter )
	return bdaddr

def read_bdaddrs():
	for dev_id in range(0,32):
		try:
			hci_sock = _bt.hci_open_dev(dev_id)
			if hci_sock > 0:
				bdaddr = read_local_bdaddr(hci_sock)
				bd_devs.append(bdaddr)
		except:
			pass

def refresh():
	del usb_devs[:]
	del bd_devs[:]
	read_usb_devices()
	read_bdaddrs()
	bd_devs.append("Custom bdaddr")
	liste1.delete(0, END)
	for usb_dev in usb_devs:
		if (usb_dev[0] == 0x054c and usb_dev[1] == 0x0268) or (usb_dev[0] == 0x03eb and usb_dev[1] == 0x2042):
			element = "VendorId: 0x%04x - ProductId: 0x%04x" % (usb_dev[0], usb_dev[1])
			liste1.insert(Tkinter.END, element)
	liste2.delete(0, END)
	for bd_dev in bd_devs:
		liste2.insert(Tkinter.END, bd_dev)

def apply():
	if liste1.curselection() == () :
		tkMessageBox.showerror("Teensy++", "Please select the Teensy++ device!")
		return
	dvid = int(liste1.get(liste1.curselection())[10:16], 16)
	dpid = int(liste1.get(liste1.curselection())[30:36], 16)
	if liste2.curselection() == () :
		tkMessageBox.showerror("Bluetooth device", "Please select the bluetooth device!")
		return
	bdaddr = liste2.get(liste2.curselection())
	if bdaddr == "Custom bdaddr" :
		bdaddr = entry1.get()
		if bdaddr == "00:00:00:00:00:00" :
			tkMessageBox.showerror("Bdaddr", "Invalid bdaddr: %s" % bdaddr)
			return
	if len(bdaddr) != 17 :
			tkMessageBox.showerror("Bdaddr", "Invalid bdaddr: %s" % bdaddr)
			return
	try :
		addr = []
		addr.append(int(bdaddr[0:2], 16))
		addr.append(int(bdaddr[3:5], 16))
		addr.append(int(bdaddr[6:8], 16))
		addr.append(int(bdaddr[9:11], 16))
		addr.append(int(bdaddr[12:14], 16))
		addr.append(int(bdaddr[15:17], 16))
	except :
		tkMessageBox.showerror("Bdaddr", "Invalid bdaddr: %s" % bdaddr)
		return
	if liste3.curselection() == () :
		tkMessageBox.showerror("VendorId - ProductID", "Please select the VendorId - ProductId!")
		return
	vendorid = liste3.get(liste3.curselection())[0:6]
	try :
		vid = int(vendorid, 16)
	except :
		tkMessageBox.showerror("Vendor Id", "Invalid Vendor Id: %s" % vendorid)
		return
	productid = liste3.get(liste3.curselection())[9:16]
	try :
		pid = int(productid, 16)
	except :
		tkMessageBox.showerror("Product Id", "Invalid Product Id: %s" % productid)
		return
	if not ((vid == 0x054c and pid == 0x0268) or (vid == 0x03eb and pid == 0x2042)):
		tkMessageBox.showerror("Product Id", "Invalid Product Id: %s" % productid)
		return
	if not tkMessageBox.askokcancel("Confirm", "Vendor Id: %s\nProduct Id: %s\nBdaddr: %s" % (vendorid, productid, bdaddr)) :
		return
	usb_set(dvid, dpid, vid, pid, addr)

if __name__ == "__main__":
	
	read_usb_devices()
	read_bdaddrs()
	
	bd_devs.append("Custom bdaddr")
	
	racine=Tkinter.Tk()
	racine.resizable(0,0)
	racine.title("Sixpair-emu customizer")
	
	frame1 = Frame(racine, padx=10)
	frame1.pack(side=Tkinter.LEFT)
	
	label1 = Label(frame1, text="Teensy++ device:")
	label1.pack(side=Tkinter.TOP, fill=Tkinter.Y)
	
	liste1=Tkinter.Listbox(frame1, width=36, height=len(usb_devs), selectmode=Tkinter.SINGLE, exportselection=0, cursor="hand2")
	liste1.pack(side=Tkinter.LEFT, fill=Tkinter.Y)
	
	frame2 = Frame(racine, padx=10)
	frame2.pack(side=Tkinter.LEFT)
	
	label2 = Label(frame2, text="Bluetooth device:")
	label2.pack(side=Tkinter.TOP, fill=Tkinter.Y)
	
	liste2=Tkinter.Listbox(frame2, width=17, height=len(bd_devs), selectmode=Tkinter.SINGLE, exportselection=0, cursor="hand2")
	liste2.pack(side=Tkinter.LEFT, fill=Tkinter.Y)
	
	frame3 = Frame(racine, padx=10)
	frame3.pack(side=Tkinter.LEFT)
	
	label3 = Label(frame3, text="Custom bdaddr")
	label3.pack(side=Tkinter.TOP)
	
	entry1 = Entry(frame3, bd =5)
	entry1.pack(side=Tkinter.TOP)
	
	label4 = Label(frame3, text="VendorId - ProductId")
	label4.pack(side=Tkinter.TOP)
	
	liste3=Tkinter.Listbox(frame3, width=15, height=2, selectmode=Tkinter.SINGLE, exportselection=0, cursor="hand2")
	liste3.pack(side=Tkinter.TOP, fill=Tkinter.Y)
	
	liste3.insert(Tkinter.END, ("0x054c - 0x0268"))
	liste3.insert(Tkinter.END, ("0x03eb - 0x2042"))
	
	entry1.insert(0, "00:00:00:00:00:00")
			
	frame4 = Frame(racine, padx=10)
	frame4.pack(side=Tkinter.LEFT)
	
	bouton=Tkinter.Button(frame4, text="Apply", command=apply)
	bouton.pack(side=Tkinter.BOTTOM)
	
	bouton=Tkinter.Button(frame4, text="Refresh", command=refresh)
	bouton.pack(side=Tkinter.BOTTOM)
	
	for usb_dev in usb_devs:
		if (usb_dev[0] == 0x054c and usb_dev[1] == 0x0268) or (usb_dev[0] == 0x03eb and usb_dev[1] == 0x2042):
			element = "VendorId: 0x%04x - ProductId: 0x%04x" % (usb_dev[0], usb_dev[1])
			liste1.insert(Tkinter.END, element)
	
	for bd_dev in bd_devs:
		liste2.insert(Tkinter.END, bd_dev)
	
	racine.mainloop()
