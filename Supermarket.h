#pragma once
#include "Address.h"
#include "Customer.h"
#include "Product.h"
#include "ShoppingCart.h"
#include "listGen.h"

typedef enum { eSortedByName, eSortedByShoppingTimes, eSortedByMoneySpent, eNotSorted, eNumOfOptions } eSortingType;
static const char* sortTypeStr[eNumOfOptions] = { "By Name", "By Shopping Times", "By Money Spent", "Not Sorted" };

typedef struct
{
	char*			name;
	Address			location;
	Customer*		customerArr;
	int				customerCount;
	eSortingType	sortingType;
	LIST*			productList;
}SuperMarket;


int		initSuperMarket(SuperMarket* pMarket);
void	printSuperMarket(const SuperMarket* pMarket);
int		addProduct(SuperMarket* pMarket);
int		addNewProduct(SuperMarket* pMarket, const char* barcode);
int		addCustomer(SuperMarket* pMarket);
int		isCustomerInMarket(SuperMarket* pMarket, Customer* pCust);
int		doShopping(SuperMarket* pMarket);
Customer* doPrintCart(SuperMarket* pMarket);
int		doPayment(SuperMarket* pMarket);
Customer*		getCustomerShopPay(SuperMarket* pMarket);
void fillCart(SuperMarket* pMarket, ShoppingCart* pCart);

void	printProductByType(SuperMarket* pMarket);
void	printAllProducts(const SuperMarket* pMarket);
void	printAllCustomers(const SuperMarket* pMarket);


Product* getProductByBarcode(SuperMarket* pMarket, const char* barcode);
Product* getProductFromUser(SuperMarket* pMarket, char* barcode);
void	getUniquBarcode(char* barcode, SuperMarket* pMarket);
Customer* FindCustomerByName(SuperMarket* pMarket, const char* name);
eSortingType getSortingTypeFromUser();
Customer* getCustomerWhoShop(SuperMarket* pMarket);
Product* getProductAndCount(SuperMarket* pMarket, int* pCount);

void sortCustomers(SuperMarket* pMarket);
void searchCustomerInSortedArr(SuperMarket* pMarket);

int writeSupermarketToBinaryFile(SuperMarket* pMarket, const char* fileName);
int readSupermarketFromBinaryFile(SuperMarket* pMarket, const char* fileName);
int insertSortedProduct(SuperMarket* pMarket, Product* pProd);

void	freeMarket(SuperMarket* pMarket);

