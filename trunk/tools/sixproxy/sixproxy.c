/*
 * sixproxy.c: a bluetooth hid proxy for the Playstation 3.
 * http://www.corbac.com/page43.html
 * version: 0.1
 */

#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int send_acl_data (int dd, uint16_t handle, uint8_t plen, char *data)
{
	uint8_t type = HCI_ACLDATA_PKT;
	hci_acl_hdr hc;
	struct iovec iv[3];
	int ivn;

	hc.handle = htobs(acl_handle_pack(handle, ACL_START));
	hc.dlen = plen;

	iv[0].iov_base = &type;
	iv[0].iov_len  = 1;
	iv[1].iov_base = &hc;
	iv[1].iov_len  = HCI_ACL_HDR_SIZE;
	ivn = 2;

	if (plen) {
		iv[2].iov_base = data;
		iv[2].iov_len  = plen;
		ivn = 3;
	}

	while (writev (dd, iv, ivn) < 0) {
		if (errno == EAGAIN || errno == EINTR)
			continue;
		return -1;
	}
	return 0;
}

int receive_acl_data (int dd, uint16_t *handle, char *data)
{
	struct hci_filter nf;
	char buf[HCI_MAX_EVENT_SIZE];
	int res;
	uint16_t *packed_handle;

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_ACLDATA_PKT, &nf);
	setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf));
	
	res = read (dd, buf, HCI_MAX_EVENT_SIZE);
	memcpy (data, buf+5, res-5);
	packed_handle = (uint16_t *) (buf+1);
	*handle = acl_handle (btohs (*packed_handle));
	
	return res-5;
}

int print_acl (char *data, int len)
{
	int a;
	
	for (a = 0; a < len; a++)
		printf ("%02hhx ", data[a]);
	printf ("\n");
}

struct
{
	int sixhc;	// Sixaxis horizontal center
	int sixhr;	// Sixaxis horizontal radius
	int sixvc;	// Sixaxis vertical center
	int sixvr;	// Sixaxis vertical radius
	int stkhc;	// Analog stick horizontal center
	int stkhr;	// Analog stick horizontal radius
	int stkvc;	// Analog stick vertical center
	int stkvr;	// Analog stick vertical radius
}
param;

int kill_deadzone (unsigned char *data)
{
	short val;
	
	// Sixaxis horizontal correction
	val = (short)(data[46]) << 8 | (short)(data[47]);
	if (val >= param.sixhc)
	{
		val += param.sixhr;
		if (val > 0x2ff)
			val = 0x2ff;
	}
	if (val < param.sixhc)
	{
		val -= param.sixhr;
		if (val < 0x100)
			val = 0x100;
	}
	data[46] = (char)((val>>8) & 0xff);
	data[47] = (char)(val & 0xff);
	
	// Sixaxis vertical correction
	val = (short)(data[48]) << 8 | (short)(data[49]);
	if (val >= param.sixvc)
	{
		val += param.sixvr;
		if (val > 0x2ff)
			val = 0x2ff;
	}
	if (val < param.sixvc)
	{
		val -= param.sixvr;
		if (val < 0x100)
			val = 0x100;
	}
	data[48] = (char)((val>>8) & 0xff);
	data[49] = (char)(val & 0xff);
	
	// Analog stick horizontal correction
	val = (short)(data[11]);
	if (val >= param.stkhc)
	{
		val += param.stkhr;
		if (val > 0xff)
			val = 0xff;
	}
	if (val < param.stkhc)
	{
		val -= param.stkhr;
		if (val < 0x00)
			val = 0x00;
	}
	data[11] = (char)(val & 0xff);
	
	// Analog stick vertical correction
	val = (short)(data[12]);
	if (val >= param.stkvc)
	{
		val += param.stkvr;
		if (val > 0xff)
			val = 0xff;
	}
	if (val < param.stkvc)
	{
		val -= param.stkvr;
		if (val < 0x00)
			val = 0x00;
	}
	data[12] = (char)(val & 0xff);
}

struct termios ots;

void exit_handler(int signum)
{
		printf("\nCaught signal %d, exiting...\n", signum);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &ots);
		exit(1);
}

void user_interface()
{
	static int initialized = 0;
	
	if (! initialized)
	{
		struct termios ts;
		
		tcgetattr(STDIN_FILENO, &ts);
		ots = ts;
		ts.c_lflag &= ~(ICANON | ECHO | ECHONL);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts);
		fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
		
		struct sigaction sa;
		
		memset(&sa, 0, sizeof(sa));
		sa.sa_handler = exit_handler;
		sigaction(SIGTERM, &sa, NULL);
		sigaction(SIGINT, &sa, NULL);
		sigaction(SIGSEGV, &sa, NULL);
		
		initialized = 1;
	}
	
	int key;
	
	key = fgetc (stdin);
	if (key != -1)
	{
		switch (key)
		{
			case 'a': param.sixhc++; break;
			case 'z': param.sixhr++; break;
			case 'e': param.sixvc++; break;
			case 'r': param.sixvr++; break;
			case 't': param.stkhc++; break;
			case 'y': param.stkhr++; break;
			case 'u': param.stkvc++; break;
			case 'i': param.stkvr++; break;
			case 'q': param.sixhc--; break;
			case 's': param.sixhr--; break;
			case 'd': param.sixvc--; break;
			case 'f': param.sixvr--; break;
			case 'g': param.stkhc--; break;
			case 'h': param.stkhr--; break;
			case 'j': param.stkvc--; break;
			case 'k': param.stkvr--; break;
		}
		printf ("sixhc %x sixhr %d sixvc %x sixvr %d stkhc %x stkhr %d stkvc %x stkvr %d\n", param.sixhc, param.sixhr, param.sixvc, param.sixvr, param.stkhc, param.stkhr, param.stkvc, param.stkvr);
	}
}

int main (int argc, char *argv[])
{
	int dd;
	uint16_t handle, hsix, hps3;
	bdaddr_t ps3_addr;
	unsigned char buf [HCI_MAX_EVENT_SIZE];
	char bdstr[18];
	int a, res, plen;

	param.sixhc = 0x203;
	param.sixhr = 7;
	param.sixvc = 0x205;
	param.sixvr = 10;
	param.stkhc = 0x80;
	param.stkhr = 20;
	param.stkvc = 0x80;
	param.stkvr = 20;
	
	assert (argc == 2);
	
	str2ba (argv[1], &ps3_addr);
	ba2str (&ps3_addr, bdstr);
	printf ("Using %s as PS3 address.\n", bdstr);

	puts ("Opening bluetooth adapter.");
	dd = hci_open_dev (0);
	assert (dd >= 0);

	puts ("Waiting for connection from controller...");
	plen = receive_acl_data (dd, &hsix, buf);
	handle = hsix;
	
	puts ("Connecting to PS3.");
	hci_write_class_of_dev (dd, 0x508, 0);
	res = hci_create_connection (dd, &ps3_addr, ACL_PTYPE_MASK, 0, 1, &hps3,  0);
	assert (res >= 0);
	
	puts ("Forwarding streams of packets.");
	for (;;)
	{
		if (handle == hsix)	// From controller to ps3
		{
			if (plen == 54)	// Crude heuristic to identify input report packets
			{
				kill_deadzone ((unsigned char *) buf);
			}
			
			send_acl_data (dd, hps3, plen, buf);
		}
		
		if (handle == hps3)	// From ps3 to controller
		{
			send_acl_data (dd, hsix, plen, buf);
		}

		plen = receive_acl_data (dd, &handle, buf);	// Get next packet
		
		user_interface();
	}
}
