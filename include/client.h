#ifndef _CLIENT_
#define _CLIENT_

#include "common.h"
#include "product.h"

typedef enum {
	PRODUCT_ALREADY_INIT = OK+1,
	NEGATIVE_QUANTITY
} clt_ret_code;

// displays available data for product of name == prodname
clt_ret_code show_product(product_name name);
// adds a new product to the stock - this is used as an initialization method
clt_ret_code add_product(product_name name, int quantity);
// removes a product from the stock altogether - the opposite of add_product
clt_ret_code remove_product(product_name name);
// Adds 'quantity' to product with name == prodname
clt_ret_code deposit_product(product_name name, int quantity);
// removes 'quantity' products with name == prodname - the opposite of deposit_product
clt_ret_code take_product(product_name name, int quantity);

#endif
