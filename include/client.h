#ifndef _CLIENT_
#define _CLIENT_

#include "common.h"
#include "product.h"
#include "communicator.h"
#include "productDB.h"

// displays available data for product of name == prodname
// db_ret_code show_product(product_name name);

typedef enum {
	PRODUCT_ALREADY_INIT = OK+1,
	NEGATIVE_QUANTITY
} clt_ret_code;

// displays available data for product of name == prodname
clt_ret_code clt_show_product(product_name name);

clt_ret_code clt_add_product(product_name name, int quantity);
// removes a product from the stock altogether - the opposite of clt_add_product
clt_ret_code clt_remove_product(product_name name);
// Adds 'quantity' to product with name == prodname
clt_ret_code clt_deposit_product(product_name name, int quantity);
// removes 'quantity' products with name == prodname - the opposite of clt_deposit_product
clt_ret_code clt_take_product(product_name name, int quantity);

#endif