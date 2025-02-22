#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Supermarket.h"
#include "Product.h"
#include "Customer.h"
#include "Address.h"
#include "General.h"
#include "ShoppingCart.h"



int		initSuperMarket(SuperMarket* pMarket)
{
	pMarket->customerCount = 0;
	pMarket->customerArr = NULL;
	pMarket->productList = (LIST*)malloc(sizeof(LIST));
	if (!L_init(pMarket->productList))
		return 0;
	pMarket->name = getStrExactLength("Enter market name");
	return initAddress(&pMarket->location);
}

void	printSuperMarket(const SuperMarket* pMarket)
{
	printf("Super Market Name: %s\t", pMarket->name);
	printf("Address: ");
	printAddress(&pMarket->location);
	printf("\n");
	printAllProducts(pMarket);
	printf("\n");
	printAllCustomers(pMarket);
}

int		addProduct(SuperMarket* pMarket)
{
	char barcode[BARCODE_LENGTH + 1];
	Product* pProd = getProductFromUser(pMarket,barcode);
	if (pProd != NULL) //This barcode exist in stock
		updateProductCount(pProd);
	else { //new product to stock
		if(!addNewProduct(pMarket, barcode))
			return 0;
	}

	return 1;
}

int		addNewProduct(SuperMarket* pMarket,const char* barcode)
{
	Product* pProd = (Product*)calloc(1, sizeof(Product));
	if (!pProd)
		return 0;

	strcpy(pProd->barcode, barcode);
	initProductNoBarcode(pProd);

	return insertSortedProduct(pMarket, pProd);
}

int insertSortedProduct(SuperMarket* pMarket, Product* pProd)
{
	NODE* pointer = &pMarket->productList->head;

	while (pointer->next)
	{
		//compares new product with the key of following product, if the barcode of former
		//is bigger than the latter then we insert it at current pointer
		if (compareByBarcode(pProd, pointer->next->key) < 0)
			break;
		
		pointer = pointer->next;
	}
	L_insert(pointer, pProd);

}

int		addCustomer(SuperMarket* pMarket)
{
	Customer cust;
	if (!initCustomer(&cust))
		return 0;

	if (isCustomerInMarket(pMarket, &cust))
	{
		printf("This customer already in market\n");
		free(cust.name);
		return 0;
	}

	pMarket->customerArr = (Customer*)realloc(pMarket->customerArr, (pMarket->customerCount + 1) * sizeof(Customer));
	if (!pMarket->customerArr)
	{
		free(cust.name);
		return 0;
	}

	pMarket->customerArr[pMarket->customerCount] = cust;
	pMarket->customerCount++;
	return 1;
}

int		isCustomerInMarket(SuperMarket* pMarket, Customer* pCust)
{
	for (int i = 0; i < pMarket->customerCount; i++)
	{
		if (strcmp(pMarket->customerArr[i].name, pCust->name) == 0)
			return 1;
	}
	return 0;
}

int	doShopping(SuperMarket* pMarket)
{
	Customer* pCustomer = getCustomerShopPay(pMarket);
	if (!pCustomer)
		return 0;

	if (pCustomer->pCart == NULL)
	{
		pCustomer->pCart = (ShoppingCart*)malloc(sizeof(ShoppingCart));
		if (!pCustomer->pCart)
			return 0;
		initCart(pCustomer->pCart);
	}
	fillCart(pMarket, pCustomer->pCart);
	if (pCustomer->pCart->count == 0)
	{
		free(pCustomer->pCart);
		pCustomer->pCart = NULL;
	}
	else
	{
		printf("---------- Shopping ended ----------\n");
	}
	return 1;
}

Customer*	doPrintCart(SuperMarket* pMarket)
{
	Customer* pCustomer = getCustomerShopPay(pMarket);
	if (!pCustomer)
		return NULL;
	if (pCustomer->pCart == NULL)
	{
		printf("Customer cart is empty\n");
		return NULL;
	}
	printShoppingCart(pCustomer->pCart);
	return pCustomer;
}

int	doPayment(SuperMarket* pMarket)
{
	Customer* pCustomer = doPrintCart(pMarket);
	if(!pCustomer)
		return 0;
	pay(pCustomer);
	return 1;
}

Customer*	getCustomerShopPay(SuperMarket* pMarket)
{
	if (pMarket->customerCount == 0)
	{
		printf("No customer listed to market\n");
		return NULL;
	}

	if (pMarket->productList->head.next == NULL)
	{
		printf("No products in market - cannot shop\n");
		return NULL;
	}

	Customer* pCustomer = getCustomerWhoShop(pMarket);
	if (!pCustomer)
	{
		printf("this customer not listed\n");
		return NULL;
	}

	return pCustomer;
}

void	printAllProducts(const SuperMarket* pMarket)
{
	int size = L_countNodes(pMarket->productList);
	printf("There are %d products\n", size);
	printf("%-20s %-10s\t", "Name", "Barcode");
	printf("%-20s %-10s %s\n", "Type", "Price", "Count In Stoke");
	printf("--------------------------------------------------------------------------------\n");

	L_print(pMarket->productList, printProduct);
}

void	printAllCustomers(const SuperMarket* pMarket)
{
	printf("There are %d listed customers\n", pMarket->customerCount);
	for (int i = 0; i < pMarket->customerCount; i++)
		printCustomer(&pMarket->customerArr[i]);
}


Customer* getCustomerWhoShop(SuperMarket* pMarket)
{
	printAllCustomers(pMarket);
	char name[MAX_STR_LEN];
	getsStrFixSize(name, sizeof(name), "Who is shopping? Enter cutomer name\n");

	Customer* pCustomer = FindCustomerByName(pMarket, name);
	
	return pCustomer;
}


void fillCart(SuperMarket* pMarket,ShoppingCart* pCart)
{
	printAllProducts(pMarket);
	char op;
	while (1)
	{
		printf("Do you want to shop for a product? y/Y, anything else to exit!!\t");
		do {
			scanf("%c", &op);
		} while (isspace(op));
		getchar(); //clean the enter
		if (op != 'y' && op != 'Y')
			break;
		int count;
		Product* pProd = getProductAndCount(pMarket, &count);
		if(pProd)
		{
			if (!addItemToCart(pCart, pProd->barcode, pProd->price, count))
			{
				printf("Error adding item\n");
				return;
			}
			pProd->count -= count; //item bought!!!
		}
	}
}

Product* getProductAndCount(SuperMarket* pMarket, int* pCount)
{
	char barcode[BARCODE_LENGTH + 1];
	Product* pProd = getProductFromUser(pMarket, barcode);
	if (pProd == NULL)
	{
		printf("No such product\n");
		return NULL;
	} 
	
	if (pProd->count == 0)
	{
		printf("This product out of stock\n");
		return NULL;
	}
	
	int count;
	do {
		printf("How many items do you want? max %d\n", pProd->count);
		scanf("%d", &count);
	} while (count <= 0 || count > pProd->count);
	*pCount = count;
	return pProd;
}

void sortCustomers(SuperMarket* pMarket)
{
	eSortingType sortingType = getSortingTypeFromUser();

	switch (sortingType)
	{
	case eSortedByName:
		qsort(pMarket->customerArr, pMarket->customerCount, sizeof(Customer), compareCustomerByName);
		break;
	case eSortedByShoppingTimes:
		qsort(pMarket->customerArr, pMarket->customerCount, sizeof(Customer), compareCustomerByShoppingTimes);
		break;
	case eSortedByMoneySpent:
		qsort(pMarket->customerArr, pMarket->customerCount, sizeof(Customer), compareCustomerByMoneySpent);
		break;
	default:
		break;
	}

	pMarket->sortingType = sortingType;
}

void searchCustomerInSortedArr(SuperMarket* pMarket)
{
	if (pMarket->sortingType == eNotSorted)
	{
		printf("The search cannot be performed, the array is not sorted\n");
		return;
	}

	Customer toCompare;
	Customer* foundCust;

	switch (pMarket->sortingType)
	{
	case eSortedByName:
		
		printf("Enter customer name:\n");
		char* name;
		toCompare.name = myGets(name, MAX_STR_LEN);

		foundCust = (Customer*)bsearch(&toCompare ,pMarket->customerArr, pMarket->customerCount, sizeof(Customer), compareCustomerByName);
		break;
	case eSortedByShoppingTimes:

		printf("Enter amout of shopping times:\n");
		int shopTimes;
		scanf("%d", &shopTimes);
		////////////////////////////getchar();////////////////////////////////////
		toCompare.shopTimes = shopTimes;
		foundCust = (Customer*)bsearch(&toCompare, pMarket->customerArr, pMarket->customerCount, sizeof(Customer), compareCustomerByShoppingTimes);
		break;
	case eSortedByMoneySpent:

		printf("Enter amout of money spent:\n");
		float moneySpent;
		scanf("%f", &moneySpent);
		////////////////////////////getchar();////////////////////////////////////
		toCompare.totalSpend = moneySpent;
		foundCust = (Customer*)bsearch(&toCompare, pMarket->customerArr, pMarket->customerCount, sizeof(Customer), compareCustomerByMoneySpent);
		break;
	default:
		printf("The search cannot be performed, the array is not sorted\n");
		return;
	}
	
	if (foundCust == NULL)
	{
		printf("Customer not found. Sorry :(\n");
		return;
	}

	printf("Customer found:\t");
	printCustomer(foundCust);
}

int writeSupermarketToBinaryFile(SuperMarket* pMarket, const char* fileName)//// add close fp when !
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	if (!fp)
		return 0;

	int len = (int)strlen(pMarket->name) + 1;
	if (fwrite(&len, sizeof(int), 1, fp) != 1)
		return 0;

	if (fwrite(pMarket->name, sizeof(char), len, fp) != len)
		return 0;

	if (!writeAddressToBinaryFile(fp, &pMarket->location))
		return 0;

	////product count??? do we need it??///

	NODE* travelPointer = pMarket->productList->head.next;
	while (travelPointer)
	{
		if (!writeProductToBinaryFile(fp, travelPointer))
			return 0;
	}

	fclose(fp);
	return 1;
}

int readSupermarketFromBinaryFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
		return 0;

	int len;
	if (fread(&len, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return 0;
	}
	pMarket->name = (char*)malloc(len * sizeof(char));
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}

	if (fread(pMarket->name, sizeof(char), len, fp) != len)
	{
		fclose(fp);
		free(pMarket->name);
		return 0;
	}

	if (!readAddressFromBinaryFile(fp, &pMarket->location))
	{
		fclose(fp);
		return 0;
	}

	int prodCount;
	if (fread(&prodCount, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return 0;
	}

	pMarket->productList = (LIST*)malloc(sizeof(LIST));
	if (!L_init(pMarket->productList))
	{
		fclose(fp);
		return 0;
	}

	Product* tempProd;
	for (int i = 0; i < prodCount; i++)
	{
		tempProd = readProductFromBinaryFile(fp);
		if (tempProd == NULL)
		{
			L_free(pMarket->productList, freeProduct);
			free(pMarket->productList);
			fclose(fp);
			return 0;
		}

		insertSortedProduct(pMarket,tempProd);

	}

	fclose(fp);
	return 1;
}

void	printProductByType(SuperMarket* pMarket)
{
	if (pMarket->productList->head.next == NULL)
	{
		printf("No products in market\n");
		return;
	}
	eProductType	type = getProductType();
	int count = 0;
	NODE* toCompareWith = pMarket->productList->head.next;
	Product chosenType;
	chosenType.type = type;
	while (toCompareWith)
	{
		if (compareByType(toCompareWith->key, &chosenType))
		{
			printProduct(toCompareWith);
			count++;
		}
		toCompareWith = toCompareWith->next;
	}

	if (count == 0)
		printf("There are no product of type %s in market %s\n", getProductTypeStr(type), pMarket->name);
}

Product* getProductFromUser(SuperMarket* pMarket, char* barcode)
{
	getBorcdeCode(barcode);
	return getProductByBarcode(pMarket, barcode);
}

void	freeMarket(SuperMarket* pMarket)
{
	free(pMarket->name);
	freeAddress(&pMarket->location);
	L_free(pMarket->productList, freeProduct);
	free(pMarket->productList);
	for (int i = 0; i < pMarket->customerCount; i++)
		freeCustomer(&pMarket->customerArr[i]);
	free(pMarket->customerArr);
}

void	getUniquBarcode(char* barcode, SuperMarket* pMarket)
{
	int cont = 1;
	NODE* ifFound;
	Product temp;
	while (cont)
	{
		getBorcdeCode(barcode);
		strcpy(temp.barcode, barcode);
		ifFound = L_find(pMarket->productList->head.next, &temp, compareByBarcode);
		if (ifFound == NULL)
			cont = 0;
		else
			printf("This product already in market\n");
	}
}


Product* getProductByBarcode(SuperMarket* pMarket, const char* barcode)
{
	Product pKey;
	//Creating a key to search if exists in list
	strcpy(pKey.barcode, barcode);
	NODE* tempNodeForKey;//will store the found node that holds the specific product
	tempNodeForKey = L_find(pMarket->productList, &pKey, compareByBarcode);
	if (tempNodeForKey == NULL)
		return NULL;
	return tempNodeForKey->key;
	//find returns NULL if nothing is found so its enough to return without checking
}

Customer* FindCustomerByName(SuperMarket* pMarket, const char* name)
{
	for (int i = 0; i < pMarket->customerCount; i++)
	{
		if (isCustomer(&pMarket->customerArr[i], name))
			return &pMarket->customerArr[i];
	}
	return  NULL;
}

eSortingType getSortingTypeFromUser()
{
	int option;
	printf("\n\n");
	do {
		printf("Please enter one of the following types\n");
		for (int i = 0; i < eNumOfOptions-1; i++)//we dont want to give an option for not sorted therefore -1
			printf("%d for %s\n", i, sortTypeStr[i]);
		scanf("%d", &option);
	} while (option < 0 || option >= eNumOfOptions - 1);
	getchar();
	return (eSortingType)option;
}
