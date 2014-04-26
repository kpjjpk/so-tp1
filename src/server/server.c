#include server.h 

#define SRV_ID 0
#define BUFFER_SIZE MAX_MSG_LENGTH

static void __handle_get_product(int client_id);
static void __handle_write_product(int client_id);
static void __handle_remove_product(int client_id);
static void __handle_invalid_call(int client_id);
static void __recv(void * buf, int len);
static void __send(int to_id, void * buf, int len);
static void __assert(int ret_status);

static void buf[BUFFER_SIZE];

void srv_start() {
	int from_id;
	msg_type type;

	__assert(ipc_init(SRV_ID));
	__assert(db_init());

	while(true) {
		usleep(700 * 1000);
		__recv(&from_id, sizeof(int));
		__recv(&type, sizeof(msg_type));
		switch(type) {
			case GET_PRODUCT:
				__handle_get_product(from_id);
				break;
			case WRITE_PRODUCT:
				__handle_write_product(from_id);
				break;
			case REMOVE_PRODUCT:
				__handle_remove_product(from_id);
				break;
			default:
				__handle_invalid_call(from_id);
				break;
		}
	}
}

srv_ret_code srv_get_product(product_name name, Product * productp) {
	return db_get_product_by_name(name, productp));
}

srv_ret_code srv_write_product(Product product) {
	int ret;
	if ((ret = db_update_product(product)) == NO_PRODUCT_FOR_NAME) {
		ret = db_save_product(product);
	}
	return ret;
}

srv_ret_code srv_remove_product(product_name name) {
	return db_delete_product();
}

void __handle_get_product(int client_id) {
	product_name_msg msg;
	Product product;
	void resp[sizeof(product_resp)]; //maybe buffer could be used here

	__recv(&buf, sizeof(msg) - sizeof(msg_type));

	if ((ret = msg_deserialize_product_name_msg(buf, &msg)) == OK 
		&& (ret = srv_get_product(msg.name, &product)) == OK) {
		__send(client_id, msg_serialize_product_resp(msg_product_msg_new(type, product), resp), sizeof(product_resp));
	} else {
		__send(client_id, msg_error_resp_new(ERR_RESP, ret), sizeof(error_resp));
	}
}

void __handle_write_product(int client_id) {
	product_msg msg;
	Product product;
	void resp[sizeof(error_resp)]; //maybe buffer could be used here

	__recv(&buf, sizeof(product_msg) - sizeof(msg_type) - sizeof(int));

	if ((ret = msg_deserialize_product_msg(buf, &msg)) == OK 
		&& (ret = srv_write_product(product)) == OK) {
		__send(client_id, msg_error_resp_new(OK_RESP, ret), sizeof(error_resp));
	} else {
		__send(client_id, msg_error_resp_new(ERR_RESP, ret), sizeof(error_resp));
	}
}

void __handle_remove_product(int client_id) {
	product_name_msg msg;
	void resp[sizeof(error_resp)]; //maybe buffer could be used here

	__recv(&buf, sizeof(product_name_msg) - sizeof(msg_type) - sizeof(int));

	if ((ret = msg_deserialize_product_name_msg(buf, &msg)) == OK 
		&& (ret = srv_remove_product(msg.name)) == OK) {
		__send(client_id, msg_error_resp_new(OK_RESP, ret), sizeof(error_resp));
	} else {
		__send(client_id, msg_error_resp_new(ERR_RESP, ret), sizeof(error_resp));
	}
}

void __handle_invalid_call(int client_id) {
	__send(client_id, msg_error_resp_new(ERR_RESP, INVALID_MSG), sizeof(error_resp))
}

void __recv(void * buf, int len) {
	ipc_recv(SRV_ID, buf, len);	
}

void __send(int to_id, void * buf, int len) {
	ipc_recv(SRV_ID, to_id, buf, len);	
}

void __assert(int ret_status) { //TODO: check
	if (ret_status != OK) {
		printf("Failed (%d)\n", ret_status);
		exit(1);
	}
}
