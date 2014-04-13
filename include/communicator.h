#ifndef _COMMUNICATOR_
#define _COMMUNICATOR_

int ipc_init(int from_id);
//from_id to_id from the processes
int ipc_send(int from_id, int to_id, void * buf, int len);
//who called recv could easily go into the message (if necessary)
//from_id is the reader's id
int ipc_recv(int from_id, void * buf, int len);
int ipc_close(int from_id);

#endif
