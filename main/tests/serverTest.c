#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../include/common.h"
#include "../../include/error.h"
#include "../../include/productDB.h"
#include "../../include/utils.h"
#include "../../include/communicator.h"
#include "../../include/server.h"
#include "../../include/msg.h"


#define CLT_ID 1
#define INVALID_ID -1

#define SRV_RESP_LEN 2

static void fatal(char *s);
static void bye(int sig);
static void __connect();
static void __send(void * buf, int len);
static void __recv(void * buf, int len);
static void __disconnect();
static boolean __get_product(string name, boolean expecting_failure);
static boolean __remove_product(string name, boolean expecting_failure);
static boolean __write_product(string name, int quantity, boolean expecting_failure);
static boolean __handle_not_ok_resp(msg_type type, boolean expecting_failure);

int main(int argc, char **argv) {
	boolean passed;
	Product a = product_new("pen", 100)/*, b = product_new("stapler", 500)*/;

	db_init();
	db_save_product(a);
	//db_save_product(b);

	switch ( fork() ) {
		case -1:
			fatal("Fork error");
			break;
		case 0: /* child (client) */
			usleep(1000);
			passed = /*__get_product("pen", false)*/
				__write_product("rubber", 80, false)
				&& __remove_product("rubber", false)
				&& __get_product("rubber", true)
				&& __write_product("rubber", 1, false);
			printf("\n\nAll tests done: %s\n", !passed? "[FAILED]":"[OK]");
			//ipc_close(CLT_ID);
			break;
		default: /* parent (server) */
			srv_start();
			break;
	}
	return 0;
}

boolean __get_product(string name, boolean expecting_failure){
	char toSend[sizeof(product_name_msg)];
	msg_type type;
	char resp_body[sizeof(product_resp)];
	Product product;
	
	__connect();
	msg_serialize_product_name_msg(CLT_ID, GET_PRODUCT, name, toSend);
	__send(toSend, sizeof(product_name_msg));
	__recv(&type, sizeof(msg_type));
	
	if (type == OK_RESP) {
		__recv(resp_body, sizeof(Product));
		msg_deserialize_product(resp_body, &product);
		__disconnect();
		printf("Client: %s ...Received -- {[OK] - Product: {name: %s, quantity: %d}}\n", !expecting_failure? "[OK]":"[ERROR]", product.name, product.quantity);
		return !expecting_failure;
	}
	return __handle_not_ok_resp(type, expecting_failure);
}

boolean __remove_product(string name, boolean expecting_failure){
	char toSend[sizeof(product_name_msg)];
	msg_type type;
	char resp_body[sizeof(error_resp)];
	int code;
	
	__connect();
	msg_serialize_product_name_msg(CLT_ID, REMOVE_PRODUCT, name, toSend);
	__send(toSend, sizeof(product_name_msg));
	__recv(&type, sizeof(msg_type));
	
	if (type == OK_RESP) {
		__recv(resp_body, sizeof(int));
		msg_deserialize_code(resp_body, &code);
		__disconnect();
		printf("Client: %s ...Received -- {[OK] - Code: {%d}}\n", !expecting_failure? "[OK]":"[ERROR]", code);
		return !expecting_failure;
	}
	return __handle_not_ok_resp(type, expecting_failure);
}

boolean __write_product(string name, int quantity, boolean expecting_failure){
	char toSend[sizeof(product_msg)];
	msg_type type;
	char resp_body[sizeof(error_resp)];
	int code;
	
	__connect();
	msg_serialize_product_msg(CLT_ID, WRITE_PRODUCT, product_new(name, quantity), toSend);
	__send(toSend, sizeof(product_msg));
	__recv(&type, sizeof(msg_type));
	if (type == OK_RESP) {
		__recv(resp_body, sizeof(int));
		msg_deserialize_code(resp_body, &code);
		__disconnect();
		printf("Client: %s ...Received -- {[OK] - Code: {%d}}\n", !expecting_failure? "[OK]":"[ERROR]", code);
		return !expecting_failure;
	}
	return __handle_not_ok_resp(type, expecting_failure);
}

boolean __handle_not_ok_resp(msg_type type, boolean expecting_failure) {
	char resp_body[sizeof(int)];
	int code;
	__recv(resp_body, sizeof(int));
	msg_deserialize_code(resp_body, &code);
	__disconnect();
	switch(type) {
		case ERR_RESP:
			printf("Client: %s ...Received -- {[ERROR] - Code: {%d}}\n", expecting_failure? "[OK] (expected)":"[ERROR]", code);
			return expecting_failure;
		default:
			printf("Client: [ERROR] (type: %d)\n", type);
			return false;
	}
}

void fatal(char *s) {
	perror(s);
	exit(1);
}

void bye(int sig) {
	printf("Srv: received SIGPIPE\n");
	exit(0);
}

void __send(void * buf, int len) {
	printf("Client: wrote %d bytes of %d\n", ipc_send(CLT_ID, SRV_ID, buf, len), len);	
}

void __recv(void * buf, int len) {
	printf("Client: read %d bytes of %d\n", ipc_recv(CLT_ID, buf, len), len);
}

void __connect() {
	verify(ipc_connect(CLT_ID, SRV_ID) == OK, "Client: could not connect to srv [ERROR]");
	printf("Client: connected to srv\n");
}

void __disconnect() {
	verify(ipc_disconnect(CLT_ID, SRV_ID) == OK, "Client: could not diconnect from srv [ERROR]");
	printf("Client: diconnected from srv\n");
}