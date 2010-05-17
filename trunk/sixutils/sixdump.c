/*
 * Derived from sixpair.c version 2007-04-18
 * Compile with: gcc -o sixdump sixdump.c -lusb
 * Dump the get request response for wValue from 0 to 0xFFFF.
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <usb.h>

#define VENDOR 0x054c
#define PRODUCT 0x0268

#define USB_DIR_IN 0x80
#define USB_DIR_OUT 0

void fatal(char *msg) { perror(msg); exit(1); }

void generic_request(usb_dev_handle *devh, int itfnum, unsigned short wValue)
{
  char buf[64];
  memset(buf, 0xFF, sizeof(buf));
  int ret = usb_control_msg(devh, USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE, 0x01,
				wValue, itfnum, buf, sizeof(buf), 5000);
  int i;
  printf("wValue: 0x%04x read: %d data: {", wValue, ret);
  for(i=0; i<sizeof(buf); ++i)
  {
    printf("0x%02hhx,", buf[i]);
  }
  printf("}\n\n");
}

void process_device(int argc, char **argv, struct usb_device *dev,
		    struct usb_config_descriptor *cfg, int itfnum) {

  usb_dev_handle *devh = usb_open(dev);
  if ( ! devh ) fatal("usb_open");

  usb_detach_kernel_driver_np(devh, itfnum);

  int res = usb_claim_interface(devh, itfnum);
  if ( res < 0 ) fatal("usb_claim_interface");

  int i;
  for(i=0; i<0xFFFF; ++i)
  {
    generic_request(devh, itfnum, i);
  }
  
  usb_close(devh);
}

int main(int argc, char *argv[]) {  

  usb_init();
  if ( usb_find_busses() < 0 ) fatal("usb_find_busses");
  if ( usb_find_devices() < 0 ) fatal("usb_find_devices");
  struct usb_bus *busses = usb_get_busses();
  if ( ! busses ) fatal("usb_get_busses");

  int found = 0;

  struct usb_bus *bus;
  for ( bus=busses; bus; bus=bus->next ) {
    struct usb_device *dev;
    for ( dev=bus->devices; dev; dev=dev->next) {
      struct usb_config_descriptor *cfg;
      for ( cfg = dev->config;
	    cfg < dev->config + dev->descriptor.bNumConfigurations;
	    ++cfg ) {
	int itfnum;
	for ( itfnum=0; itfnum<cfg->bNumInterfaces; ++itfnum ) {
	  struct usb_interface *itf = &cfg->interface[itfnum];
	  struct usb_interface_descriptor *alt;
	  for ( alt = itf->altsetting;
		alt < itf->altsetting + itf->num_altsetting;
		++alt ) {
	    if ( dev->descriptor.idVendor == VENDOR &&
		 dev->descriptor.idProduct == PRODUCT &&
		 alt->bInterfaceClass == 3 ) {
	      process_device(argc, argv, dev, cfg, itfnum);
	      ++found;
	    }
	  }
	}
      }
    }
  }

  if ( ! found ) printf("No controller found on USB busses.\n");
  return 0;

}

