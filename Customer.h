#pragma once
#include "ShoppingCart.h"

typedef struct
{
	char*			name;
	int				shopTimes;
	float			totalSpend;
	ShoppingCart*	pCart;
}Customer;

int		initCustomer(Customer* pCustomer);
void	printCustomer(const Customer* pCustomer);

int		isCustomer(const Customer* pCust, const char* name);

void	pay(Customer* pCustomer);

int		compareCustomerByName(void* v1, void* v2);
int		compareCustomerByShoppingTimes(void* v1, void* v2);
int		compareCustomerByMoneySpent(void* v1, void* v2);

void	freeCustomer(Customer* pCust);



