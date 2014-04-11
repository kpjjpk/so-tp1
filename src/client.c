#include <stdio.h>
#include <stdlib.h>
#include "../include/client.h"
#include "../include/dbx.h"

// displays available data for product of name == prodname
void show_product(char * prodname){
	Product product;
	product_init(&product);

	dbx_get_product_by_name(prodname, &product);
	printf("Name=%s, quantity=%d\n", product.name, product.quantity);
}

// adds a new product to the stock - this is used as an initialization method
void add_product(char * prodname, int quantity){
	// db_save_product
}

// removes a product from the stock altogether - the opposite of add_product
void remove_product(char * prodname){
	dbx_delete_product(prodname);

}

// adds 'quantity' products with name == prodname
void deposit_product(char * prodname, int quantity){
// db_ret_code db_update_product(Product product);
}

// removes 'quantity' products with name == prodname - the opposite of deposit_product
void take_product(char * prodname, int quantity){
// db_ret_code db_update_product(Product product);
}