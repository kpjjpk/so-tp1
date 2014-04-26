#ifndef _PRODUCT_
#define _PRODUCT_

#include "common.h"

#define PRODUCT_MAX_NAME_LEN 1000

typedef char product_name[PRODUCT_MAX_NAME_LEN];

typedef struct {
	product_name name;
	int quantity;
} Product;

Product product_new(product_name name, int quantity);
void product_set_name(Product * product, product_name name);
void product_set_quantity(Product * product, int quantity);

#endif