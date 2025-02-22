#pragma once

typedef enum {eFruitVegtable, eFridge, eFrozen, eShelf, eNofProductType} eProductType;
static const char* typeStr[eNofProductType] = { "Fruit Vegtable", "Fridge", "Frozen", "Shelf" };

#define NAME_LENGTH 20
#define BARCODE_LENGTH 7

typedef struct
{
	char			name[NAME_LENGTH+1];
	char			barcode[BARCODE_LENGTH + 1];
	eProductType	type;
	float			price;
	int				count;
}Product;

void	initProduct(Product* pProduct);
void	initProductNoBarcode(Product* pProduct);
void	initProductName(Product* pProduct);
void	printProduct(const Product* pProduct);

void	getBorcdeCode(char* code);
int		isProduct(const Product* pProduct,const char* barcode);
eProductType getProductType();
const char*	getProductTypeStr(eProductType type);
int compareByType(void* v1, void* v2);
int compareByBarcode(void* v1, void* v2);

void	updateProductCount(Product* pProduct);
void	freeProduct(Product* pProduct);

int		writeProductToBinaryFile(FILE* fp, Product* pProduct);
Product* readProductFromBinaryFile(FILE* fp);
