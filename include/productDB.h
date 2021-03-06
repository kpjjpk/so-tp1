#ifndef _PRODUCT_DB_
#define _PRODUCT_DB_

#include "common.h"
#include "product.h"

#define DB_ROOT_PATH "so-db"
#define DB_TABLE_NAME "product"

#define DB_TABLE_PATH DB_ROOT_PATH"/"DB_TABLE_NAME
#define DB_BUFFER_SIZE 100

typedef enum {
	DB_NOT_INITIALIZED = OK+1,
	CANNOT_CREATE_DATABASE,
	CANNOT_CREATE_TABLE,
	NO_PRODUCT_FOR_NAME,
	PRODUCT_EXISTS,
	UNEXPECTED_DELETE_ERROR,
	UNEXPECTED_UPDATE_ERROR
} db_ret_code;

db_ret_code db_init();
db_ret_code db_save_product(Product product);
db_ret_code db_get_product_by_name(product_name name, Product * productp);
db_ret_code db_update_product(Product product);
db_ret_code db_delete_product(product_name name);

#endif