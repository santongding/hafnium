#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hf/call.h"
#include "hf/dlog.h"
#include "hf/ffa.h"
#include "hf/memiter.h"
#include "hf/socket.h"
#include "hf/std.h"
#include "hf/transport.h"

#include "hf-usr-protocol.h"
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define MAX_BUF_SIZE 256
void echo_by_socket()
{
	ffa_vm_id_t vm_id = HF_VM_ID_OFFSET + 1;
	int port = 10;
	int socket_id;
	struct hf_sockaddr addr;
	const char send_buf[] = "The quick brown fox jumps over the lazy dogs.";
	size_t send_len = sizeof(send_buf);
	char resp_buf[MAX_BUF_SIZE];
	ssize_t recv_len;

	/* Create Hafnium socket. */
	socket_id = socket(PF_HF, SOCK_DGRAM, 0);
	if (socket_id == -1) {
		dlog("Socket creation failed: %s\n", strerror(errno));
		return;
	}
	dlog("Socket created successfully.\n");

	/* Connect to requested VM & port. */
	addr.family = PF_HF;
	addr.vm_id = vm_id;
	addr.port = port;
	if (connect(socket_id, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		dlog("Socket connection failed: %s\n", strerror(errno));
		return;
	}
	dlog("Socket to secondary VM %d connected on port %d.\n", vm_id, port);

	/*
	 * Send a message to the secondary VM.
	 * Enable the confirm flag to try again in case port is busy.
	 */
	if (send(socket_id, send_buf, send_len, MSG_CONFIRM) < 0) {
		dlog("Socket send() failed: %s\n", strerror(errno));
		return;
	}
	dlog("Packet with length %d sent.\n", send_len);

	/* Receive a response, which should be an echo of the sent packet. */
	recv_len = recv(socket_id, resp_buf, sizeof(resp_buf) - 1, 0);

	if (recv_len == -1) {
		dlog("Socket recv() failed: %s\n", strerror(errno));
		return;
	}
	dlog("Packet with length %d received.\n", recv_len);
	return;
}
void echo_direct()
{
	int f = open("/proc/hf-usr-pipe", O_WRONLY);
	struct hf_usr_protocol proto = {
		.vm_id = 0x8002,
	};
	if (f == -1) {
		dlog("fail to open proc file");
	} else {
		int ret = write(f, (void *)&proto,
				sizeof(struct hf_usr_protocol));
		dlog("ret: %d\n", ret);
	}
}
int main()
{
	// echo_by_socket();
	//	dlog("finish echo by socket!\n");
	// echo_by_vmapi();
	// dlog("finish echo by vmapi!\n");
	echo_direct();
	dlog("finish echo direct!\n");
}
