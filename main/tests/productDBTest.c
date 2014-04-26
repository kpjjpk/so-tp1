#include <stdio.h>
#include <stdlib.h>
#include "../../include/productDB.h"
#include "../../include/common.h"
#include "../../include/product.h"
#include "../../include/client.h"

int main(void) {
	int testAns;

	Product a = product_new("pen", 100), b = product_new("stapler", 500), c = product_new("rubber", 80);

	db_init();

	db_save_product(a);
	db_save_product(b);

	//show_product("pen");
	//remove_product("pen");
	
	//add_product("rubber",70);
	//deposit_product("rubber",80);
	//take_product("rubber",10);

	testAns = db_get_product_by_name("pen", &c);
	if (testAns == OK){
		printf("Name=%s, quantity=%d (OK)\n", c.name, c.quantity);
	} else {
		printf("Error (NOT OK) %d\n",testAns);
		return 0;
	}

	testAns = db_get_product_by_name("pencil", &c);
	if (testAns == NO_PRODUCT_FOR_NAME){
	 	printf("Expected error (OK)\n");
	} else {
		printf("Unexpected error (NOT OK): %d\n", testAns);
		return 0;
	}

	product_set_quantity(&a, 99);
	testAns = db_update_product(a);
	if(testAns != OK){
		printf("Update error: %d\n", testAns);
	 	return 0;
	}
	testAns = db_get_product_by_name("pen", &c);
	if(testAns == OK){
		printf("Name=%s, quantity=%d (OK)\n", c.name, c.quantity);
	} else {
		printf("Unexpected error (NOT OK): %d\n", testAns);
	 	return 0;
	}

	testAns = db_delete_product(b.name);
	if(testAns != OK) {
	 	printf("Delete error: %d\n", testAns);
	 	return 0;
	}
	testAns = db_get_product_by_name("stapler", &c);
	if (testAns == NO_PRODUCT_FOR_NAME){
	 	printf("Expected error (OK)\n");
	} else {
	 	printf("NO Error (NOT OK): %d\n", testAns);
	 	return 0;
	}
	printf("All tests passed\n");

	return 0;
}