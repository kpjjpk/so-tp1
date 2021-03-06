#include "../../include/communicator.h"
#include "../../include/common.h"
#include "../../include/key.h"
#include "../../include/error.h"
#include "../../include/utils.h"
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define MSQ_MSG_DATA_SIZE (sizeof(__msq_pkg)-sizeof(long))
#define MSQ_MSG_LEN 200

typedef struct {
	long mtype;
	int len;
	char data[MSQ_MSG_LEN];
} __msq_pkg;

static int __get_msq(int owner_id);
static int __destr_msq(int from_id);

static int __to_read = -1, __read = 0;
static long __conn = 0;
static char __msg_data_buf[MSQ_MSG_LEN];


int ipc_init(int from_id) {
    ipc_close(from_id);
    printf("%s: Initializing...\n", (from_id == SRV_ID)? "Srv":"Clt");
	return OK;
}

int ipc_connect(int from_id, int to_id){
	printf("%s: Connecting...\n", (from_id == SRV_ID)? "Srv":"Clt");
	__conn = 0;
	__to_read = -1;
	__read = 0;
	return OK;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	__msq_pkg msg;
	int sent = min(len, MSQ_MSG_LEN);
    printf("\n(Try) %s: Sending %d bytes to %d through %d...\n", (from_id == SRV_ID)? "Srv":"Clt", len, to_id, __get_msq(to_id));

	msg.mtype = from_id;
	msg.len = sent;
	memcpy(msg.data, buf, sent);

	printf("{ from_id=%d, len=%d, data='%.*s' }\n", from_id, sent, sent, (string) buf);

	verify(msgsnd(__get_msq(to_id), &msg, MSQ_MSG_DATA_SIZE, 0)!=-1, "Send error");
	printf("%s: Sent %d bytes through %d...\n", (from_id == SRV_ID)? "Srv":"Clt", sent, __get_msq(to_id));
	return sent;
}

int ipc_recv(int from_id, void * buf, int len) {
	__msq_pkg msg;

    printf("\n(Try) %s: Receiving %d bytes through %d...\n", (from_id == SRV_ID)? "Srv":"Clt", len, __get_msq(from_id));
	
	if (__to_read == -1) {
		verify(msgrcv(__get_msq(from_id), &msg, MSQ_MSG_DATA_SIZE, __conn, 0) != -1, "Receive error");
		__conn = msg.mtype;
		memcpy(__msg_data_buf, msg.data, MSQ_MSG_LEN);
		__to_read = msg.len;
		__read = 0;
	}

	if (__read + len < MSQ_MSG_LEN) {
		memcpy(buf, __msg_data_buf+__read, len);
	} else {
		memcpy(buf, __msg_data_buf+(__read%MSQ_MSG_LEN), MSQ_MSG_LEN - (__read % MSQ_MSG_LEN));
		memcpy(buf, __msg_data_buf, (__read+len-MSQ_MSG_LEN) % MSQ_MSG_LEN);
	}
	__read += len;

	if (__read == __to_read) {
		__to_read = -1;
		__read = 0;
		printf("%s: Received %d bytes through %d... \n", (from_id == SRV_ID)? "Srv":"Clt", len, __get_msq(from_id));
	} else {
	    printf("%s: Received %d bytes through %d... \n", (from_id == SRV_ID)? "Srv":"Clt", len, __get_msq(from_id));
	}
	return len;
}

int ipc_disconnect(int from_id, int to_id){
    printf("%s: Disconnecting... from %d\n", (from_id == SRV_ID)? "Srv":"Clt", __get_msq(from_id));
	switch(from_id) {
	case SRV_ID:
		return OK;
	default:
		return __destr_msq(from_id);
	}
}

int ipc_close(int from_id) {
    printf("%s: Closing...\n", (from_id == SRV_ID)? "Srv":"Clt");
	switch(from_id) {
	case SRV_ID:
		return __destr_msq(from_id);
	default:
		return OK;	
	}
}

int __get_msq(int owner_id) {
	key_t key;
	int msq_id;
	
	verify((key = key_get(owner_id)) != (key_t)-1, "Error getting msg queue");
	verify((msq_id = msgget(key, IPC_CREAT | 0666)) != -1, "Error getting msg queue");
	return msq_id;
}

int __destr_msq(int from_id) {
	return msgctl(__get_msq(from_id), IPC_RMID, NULL);
}
