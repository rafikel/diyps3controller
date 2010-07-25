
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <unistd.h>

int l2cap_connect(const char *bdaddr, int psm)
{
	int fd;
	struct sockaddr_l2 addr;
	struct linger l = { .l_onoff = 1, .l_linger = 5 };
	int opt;

	if ((fd = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) == -1)
		return -1;
	if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) < 0) {
		close(fd);
		return -2;
	}
        opt = 0;
        if (setsockopt(fd, SOL_L2CAP, L2CAP_LM, &opt, sizeof(opt)) < 0) {
		close(fd);
		return -3;
	}
	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm    = htobs(psm);
	str2ba(bdaddr, &addr.l2_bdaddr);
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		close(fd);
		return -4;
	}
	return fd;
}

int l2cap_send(int fd, const char* buf, int len, int blocking)
{
	return send(fd, buf, len, blocking ? 0 : MSG_DONTWAIT);
}

