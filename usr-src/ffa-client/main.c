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

#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>

_Alignas(4096) uint8_t kstack[4096];
static _Alignas(HF_MAILBOX_SIZE) uint8_t send_mailbox[HF_MAILBOX_SIZE];
static _Alignas(HF_MAILBOX_SIZE) uint8_t recv_mailbox[HF_MAILBOX_SIZE];

static hf_ipaddr_t send_addr = (hf_ipaddr_t)send_mailbox;
static hf_ipaddr_t recv_addr = (hf_ipaddr_t)recv_mailbox;
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
void echo_by_vmapi()
{
	ffa_vm_id_t vm_id = HF_VM_ID_OFFSET;
	ffa_vm_id_t target_id = vm_id;
	struct ffa_value r = ffa_rxtx_map(send_addr, recv_addr);
	dlog("ret value:%p\n %d", r.func, r.arg2);
	char msg[] = "hello, vm!";
	struct hf_msg_hdr *hdr = (struct hf_msg_hdr *)send_mailbox;
	hdr->dst_port = target_id;
	hdr->src_port = vm_id;
	memcpy(send_mailbox + sizeof(struct hf_msg_hdr), msg, sizeof(msg));
	r = ffa_msg_send(vm_id, target_id,
			 sizeof(struct hf_msg_hdr) + sizeof(msg), 0);
	dlog("ret value:%p %d\n", r.func, r.arg2);
	/*struct ffa_value r = ffa_msg_wait();
	if (r.func != FFA_MSG_SEND_32) {
		dlog("linux fail to recv, func: %p\n", (r.func));
	} else {
		dlog("linux recv: %s\n",
		     recv_mailbox + sizeof(struct hf_msg_hdr));
	}*/
}
int main()
{
	int pid = fork();
	if (pid == 0) {
		echo_by_socket();
		dlog("finish echo by socket!\n");
	} else {
		exit(0);
	}
	// echo_by_vmapi();
	// dlog("finish echo by vmapi!\n");
}
