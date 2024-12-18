// product_management.c
#include "product_management.h"

void loadProducts(Product* products, int* productCount) {
    FILE* file = fopen("products.csv", "r");
    if (file == NULL) {
        printf("Error opening products file\n");
        return;
    }
    
    *productCount = 0;
    char line[1024];
    
    // Skip header line
    fgets(line, 1024, file);
    
    while (fgets(line, 1024, file) && *productCount < MAX_PRODUCTS) {
        Product* p = &products[*productCount];
        sscanf(line, "%d,%[^,],%[^,],%lf,%d,%[^,\n]",
               &p->id, p->name, p->description,
               &p->price, &p->stock, p->category);
        (*productCount)++;
    }
    
    fclose(file);
}

void saveProducts(Product* products, int productCount) {
    FILE* file = fopen("products.csv", "w");
    if (file == NULL) {
        printf("Error opening products file for writing\n");
        return;
    }
    
    fprintf(file, "id,name,description,price,stock,category\n");
    
    for (int i = 0; i < productCount; i++) {
        fprintf(file, "%d,%s,%s,%.2f,%d,%s\n",
                products[i].id, products[i].name, products[i].description,
                products[i].price, products[i].stock, products[i].category);
    }
    
    fclose(file);
}

void displayProducts(Product* products, int productCount) {
    printf("\n==================== Available Products ====================\n");
    printf("ID\tName\t\t\t\t\t\t\tPrice\tStock\tCategory\n");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < productCount; i++) {
        printf("%d\t%-16s\t\t\t\t$%.2f\t%d\t\t%s\n",
               products[i].id, products[i].name, products[i].price,
               products[i].stock, products[i].category);
    }
}

void searchProducts(Product* products, int productCount, char* searchTerm) {
    printf("\n=== Search Results ===\n");
    printf("ID\tName\t\t\t\t\tPrice\tStock\tCategory\n");
    printf("----------------------------------------------------\n");
    
    for (int i = 0; i < productCount; i++) {
        if (strstr(products[i].name, searchTerm) ||
            strstr(products[i].category, searchTerm)) {
            printf("%d\t%-16s\t\t$%.2f\t%d\t\t%s\n",
                   products[i].id, products[i].name, products[i].price,
                   products[i].stock, products[i].category);
        }
    }
}

void filterByPrice(Product* products, int productCount, double minPrice, double maxPrice) {
    printf("\n==== Products within price range $%.2f - $%.2f ====\n",
           minPrice, maxPrice);
    printf("ID\tName\t\t\t\t\tPrice\tStock\tCategory\n");
    printf("----------------------------------------------------\n");
    
    for (int i = 0; i < productCount; i++) {
        if (products[i].price >= minPrice && products[i].price <= maxPrice) {
            printf("%d\t%-16s\t\t$%.2f\t%d\t\t%s\n",
                   products[i].id, products[i].name, products[i].price,
                   products[i].stock, products[i].category);
        }
    }
}
