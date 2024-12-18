// owner.c
#include "owner.h"
#include <errno.h>
#include <string.h>

// ฟังก์ชันสร้างสินค้าใหม่
void createProduct(void) {
    Product newProduct;
    FILE *file = fopen("products.csv", "r");
    
    // Get the last product ID
    int lastId = 0;
    char line[500];
    while (file != NULL && fgets(line, sizeof(line), file)) {
        int currentId;
        if (sscanf(line, "%d,", &currentId) == 1) {
            if (currentId > lastId) {
                lastId = currentId;
            }
        }
    }
    if (file != NULL) {
        fclose(file);
    }
    
    // Open file for appending new product
    file = fopen("products.csv", "a+");
    if (file == NULL) {
        printf("Cannot open file: %s\n", strerror(errno));
        return;
    }
    
    newProduct.id = lastId + 1;
    
    printf("\n=== Add New Product ===\n");
    
    printf("Product Name: ");
    scanf(" %[^\n]", newProduct.name);
    
    printf("Product Description: ");
    scanf(" %[^\n]", newProduct.description);
    
    printf("Price: ");
    scanf("%lf", &newProduct.price);
    while (newProduct.price <= 0) {
        printf("Please enter a price greater than 0: ");
        scanf("%lf", &newProduct.price);
    }
    
    printf("Stock Quantity: ");
    scanf("%d", &newProduct.stock);
    while (newProduct.stock < 0) {
        printf("Please enter a non-negative quantity: ");
        scanf("%d", &newProduct.stock);
    }
    
    printf("Minimum Stock Threshold: ");
    scanf("%d", &newProduct.minimum_threshold);
    while (newProduct.minimum_threshold < 0) {
        printf("Please enter a non-negative quantity: ");
        scanf("%d", &newProduct.minimum_threshold);
    }
    
    if (fprintf(file, "%d,%s,%s,%.2f,%d,%d\n",
            newProduct.id,
            newProduct.name,
            newProduct.description,
            newProduct.price,
            newProduct.stock,
            newProduct.minimum_threshold) < 0) {
        printf("Error writing to file\n");
        fclose(file);
        return;
    }
    
    fflush(file);
    fclose(file);
    
    char logMessage[100];
    sprintf(logMessage, "Added new product: %s", newProduct.name);
    logAction(logMessage);
    
    printf("\nProduct added successfully\n");
    printf("Your product ID: %d\n", newProduct.id);
}

// ฟังก์ชันดูข้อมูลสินค้า
void viewProduct(void) {
    FILE *file = fopen("products.csv", "r");
    char line[500];
    
    if (file == NULL) {
        printf("Product not found\n");
        return;
    }
    
    // ข้ามบรรทัดแรก (header)
    fgets(line, sizeof(line), file);
    
    printf("\n=== All Products List ===\n");
    while (fgets(line, sizeof(line), file)) {
        Product product;
        // ลบ newline character ที่อาจติดมา
        line[strcspn(line, "\n")] = 0;
        
        if (sscanf(line, "%d,%[^,],%[^,],%lf,%d,%d",
                   &product.id,
                   product.name,
                   product.description,
                   &product.price,
                   &product.stock,
                   &product.minimum_threshold) == 6) {
            
            printf("\nID: %d\n", product.id);
            printf("Name: %s\n", product.name);
            printf("Description: %s\n", product.description);
            printf("Price: %.2f Baht\n", product.price);
            printf("Stock: %d pieces\n", product.stock);
            printf("Minimum Threshold: %d pieces\n", product.minimum_threshold);
            printf("-------------------------\n");
        } else {
            printf("Error reading line: %s\n", line);
        }
    }
    
    fclose(file);
}

// ฟังก์ชันบันทึกการกระทำ
void logAction(const char* action) {
    FILE *logFile = fopen("activity_log.txt", "a");
    time_t now = time(NULL);
    char timestamp[26];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(logFile, "[%s] %s\n", timestamp, action);
    fclose(logFile);
}

// ฟังก์ชันแก้ไขข้อมูลสินค้า
void updateProduct(void) {
    int targetId;
    const char* originalFile = "products.csv";
    const char* tempFile = "temp_update.csv";
    
    FILE *file = fopen(originalFile, "r");
    FILE *temp = fopen(tempFile, "w");
    Product product;
    char line[500];
    int found = 0;
    
    if (file == NULL || temp == NULL) {
        printf("Cannot open file!\n");
        return;
    }
    
    // คัดลอก header ไปยังไฟล์ temporary
    fgets(line, sizeof(line), file);
    fprintf(temp, "%s", line);
    
    printf("Enter product ID to edit: ");
    scanf("%d", &targetId);
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // ลบ newline
        
        if (sscanf(line, "%d,%[^,],%[^,],%lf,%d,%d",
                   &product.id,
                   product.name,
                   product.description,
                   &product.price,
                   &product.stock,
                   &product.minimum_threshold) == 6) {
            
            if (product.id == targetId) {
                found = 1;
            printf("\nCurrent Product Details:\n");
            printf("Name: %s\n", product.name);
            printf("Description: %s\n", product.description);
            printf("Price: %.2f\n", product.price);
            printf("Stock: %d\n", product.stock);
            printf("Minimum: %d\n", product.minimum_threshold);
            
            printf("\nEnter new details:\n");
            printf("Product Name: ");
            scanf(" %[^\n]s", product.name);
            printf("Description: ");
            scanf(" %[^\n]s", product.description);
            printf("Price: ");
            scanf("%lf", &product.price);
            printf("Stock Quantity: ");
            scanf("%d", &product.stock);
            printf("Minimum Quantity: ");
            scanf("%d", &product.minimum_threshold);
        }
        
            fprintf(temp, "%d,%s,%s,%.2f,%d,%d\n",
                                product.id, product.name, product.description,
                                product.price, product.stock, product.minimum_threshold);
                    }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found) {
        // ใช้ path เต็มในการลบและเปลี่ยนชื่อไฟล์
        remove(originalFile);
        rename(tempFile, originalFile);
        logAction("Edit Product Information");
        printf("Edit successful\n");
    } else {
        remove(tempFile);
        printf("Product not found\n");
    }
}

// ฟังก์ชันลบสินค้า
void deleteProduct(void) {
    int targetId;
    // Set full path for files
    const char* originalFile = "products.csv";
    const char* tempFile = "temp_products.csv";
    
    FILE *file = fopen(originalFile, "r");
    if (file == NULL) {
        printf("Cannot open products file!\n");
        return;
    }
    
    FILE *temp = fopen(tempFile, "w");
    if (temp == NULL) {
        printf("Cannot create temporary file!\n");
        fclose(file);
        return;
    }
    
    Product product;
    char line[500];
    int found = 0;
    
    printf("Enter product ID to delete: ");
    scanf("%d", &targetId);
    
    // Read and copy all data except the product to delete
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d,%[^,],%[^,],%lf,%d,%d",
               &product.id, product.name, product.description,
               &product.price, &product.stock, &product.minimum_threshold) == 6) {
            
            if (product.id == targetId) {
                found = 1;
                printf("Deleting product: %s\n", product.name);
                continue;  // Skip writing this product
            }
            
            // Write data to temporary file
            fprintf(temp, "%d,%s,%s,%.2f,%d,%d\n",
                    product.id, product.name, product.description,
                    product.price, product.stock, product.minimum_threshold);
        }
    }
    
    // Close both files before proceeding
    fclose(file);
    fclose(temp);
    
    if (found) {
        // Check file deletion and renaming
        if (remove(originalFile) != 0) {
            printf("Error deleting original file!\n");
            remove(tempFile);  // Delete created temporary file
            return;
        }
        
        if (rename(tempFile, originalFile) != 0) {
            printf("Error renaming file!\n");
            return;
        }
        
        char logMessage[100];
        sprintf(logMessage, "Deleted product ID %d", targetId);
        logAction(logMessage);
        printf("Product deleted successfully\n");
    } else {
        remove(tempFile);  // Delete temporary file if product not found
        printf("Product ID %d not found\n", targetId);
    }
}

// ฟังก์ชันค้นหาสินค้าตามชื่อ
void searchByName(const char* searchTerm) {
    FILE *file = fopen("products.csv", "r");
    Product products[100];  // สมมติว่าเก็บได้สูงสุด 100 รายการ
    int count = 0;
    char line[500];
    
    if (file == NULL) {
        printf("Cannot open file!\n");
        return;
    }
    
    printf("\nSearch results for '%s':\n", searchTerm);
    
    while (fgets(line, sizeof(line), file) && count < 100) {
        Product product;
        sscanf(line, "%d,%[^,],%[^,],%lf,%d,%d",
               &product.id, product.name, product.description,
               &product.price, &product.stock, &product.minimum_threshold);
        
        // ค้นหาคำในชื่อสินค้า (ไม่สนใจตัวพิมพ์เล็ก/ใหญ่)
        char lowerName[100], lowerSearch[100];
        strcpy(lowerName, product.name);
        strcpy(lowerSearch, searchTerm);
        for(int i = 0; lowerName[i]; i++) lowerName[i] = tolower(lowerName[i]);
        for(int i = 0; lowerSearch[i]; i++) lowerSearch[i] = tolower(lowerSearch[i]);
        
        if (strstr(lowerName, lowerSearch)) {
            products[count++] = product;
        }
    }
    
    fclose(file);
    displaySearchResults(products, count);
}

// ฟังก์ชันค้นหาสินค้าตามช่วงราคา
void searchByPriceRange(float minPrice, float maxPrice) {
    FILE *file = fopen("products.csv", "r");
    Product products[100];
    int count = 0;
    char line[500];
    
    if (file == NULL) {
        printf("Cannot open file!\n");
        return;
    }
    
    printf("\nSearch results for price range %.2f - %.2f Baht:\n", minPrice, maxPrice);
    
    while (fgets(line, sizeof(line), file) && count < 100) {
        Product product;
        sscanf(line, "%d,%[^,],%[^,],%lf,%d,%d",
               &product.id, product.name, product.description,
               &product.price, &product.stock, &product.minimum_threshold);
        
        if (product.price >= minPrice && product.price <= maxPrice) {
            products[count++] = product;
        }
    }
    
    fclose(file);
    displaySearchResults(products, count);
}

// ฟังก์ชันแสดงผลการค้นหา
void displaySearchResults(Product* products, int count) {
    if (count == 0) {
        printf("No products found matching search criteria\n");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        printf("\nID: %d\n", products[i].id);
        printf("Name: %s\n", products[i].name);
        printf("Description: %s\n", products[i].description);
        printf("Price: %.2f Baht\n", products[i].price);
        printf("Stock: %d pieces\n", products[i].stock);
        printf("-------------------------\n");
    }
    
    printf("Found %d products\n", count);
}

// เพิ่มฟังก์ชันบันทึกการขาย
void recordSale(int productId, int quantity, double price) {
    FILE *salesFile = fopen("sales_history.csv", "a");
    if (salesFile == NULL) {
        printf("Error opening sales history file\n");
        return;
    }

    time_t now = time(NULL);
    char date[11];
    strftime(date, sizeof(date), "%Y-%m-%d", localtime(&now));
    
    char productName[100];
    getProductName(productId, productName);
    
    fprintf(salesFile, "%s,%d,%s,%d,%.2f\n",
            date, productId, productName, quantity, price);
    
    fclose(salesFile);
}

// ฟังก์ชันสร้างรายงานประจำวัน
void generateDailyReport(void) {
    // เปิดไฟล์สำหรับอ่านข้อมูลการขาย - แก้ไข path ให้ถูกต้อง
    FILE *salesFile = fopen("daily_sales.csv", "r");
    if (salesFile == NULL) {
        printf("Cannot open daily_sales.csv\n");
        return;
    }

    // เปิดไฟล์สำหรับเขียนรายงาน - แก้ไข path ให้ถูกต้อง
    FILE *reportFile = fopen("daily_report.csv", "w");
    if (reportFile == NULL) {
        printf("Cannot create daily_report.csv\n");
        fclose(salesFile);
        return;
    }

    // สร้างส่วนหัวของรายงาน
    fprintf(reportFile, "Date,Product_ID,Product_Name,Quantity,Price,Total\n");

    char line[1024];
    char currentDate[11];
    double totalRevenue = 0.0;
    int totalItems = 0;

    // รับวันที่ปัจจุบัน
    time_t now = time(NULL);
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", localtime(&now));

    // อ่านข้อมูลการขายและเขียนลงในรายงาน
    while (fgets(line, sizeof(line), salesFile)) {
        char date[11];
        int productId;
        char productName[100];
        int quantity;
        double price;
        double total;

        // แยกข้อมูลจากแต่ละบรรทัด
        if (sscanf(line, "%[^,],%d,%[^,],%d,%lf",
                   date, &productId, productName, &quantity, &price) == 5) {
            
            // คำนวณยอดรวม
            total = price * quantity;
            
            // เขียนข้อมูลลงในรายงาน
            fprintf(reportFile, "%s,%d,%s,%d,%.2f,%.2f\n",
                    date, productId, productName, quantity, price, total);
            
            // สะสมยอดรวม
            totalRevenue += total;
            totalItems += quantity;
        }
    }

    // เขียนยอดรวมท้ายรายงาน
    fprintf(reportFile, "\nTotal Items,%d\n", totalItems);
    fprintf(reportFile, "Total Revenue,%.2f\n", totalRevenue);

    // ปิดไฟล์
    fclose(salesFile);
    fclose(reportFile);
    
    // แสดงสรุปบนหน้าจอ
    printf("\n=== Daily Report Generated ===\n");
    printf("Total Items Sold: %d\n", totalItems);
    printf("Total Revenue: %.2f Baht\n", totalRevenue);
    printf("Report has been saved to daily_report.csv\n");
}
// ฟังก์ชันแสดงรายงานยอดขาย
void showSalesReport(void) {
    FILE *reportFile = fopen("sales_history.csv", "r");
    if (reportFile == NULL) {
        printf("Error: Cannot open file!\n");
        return;
    }
    char line[1000];
    
    printf("\n=== Sales Report ===\n");
    printf("Date        | Total Sales | Total Revenue  | Best Selling Product\n");
    printf("--------------------------------------------------------\n");
    
    while (fgets(line, sizeof(line), reportFile)) {
        char productName[1000];  // เพิ่มการประกาศ productName
        char date[1000];        // เพิ่มขนาดที่เพียงพอ
        int totalSales = 0;     // กำหนดค่าเริ่มต้น
        float totalRevenue = 0.0; // กำหนดค่าเริ่มต้น
        int quantity = 0;       // กำหนดค่าเริ่มต้น
        
        if (sscanf(line, "%[^,],%d,%f,%[^,],%d",
                date, &totalSales, &totalRevenue, productName, &quantity) == 5) {
            printf("%-12s| %9d | %9.2f | %s (%d)\n",
                        date, totalSales, totalRevenue, productName, quantity);
        }
    }
    fclose(reportFile);
}
// ฟังก์ชันแสดงรายงานสต็อกสินค้า
void showStockReport(void) {
    FILE *file = fopen("products.csv", "r");
    char line[500];
    
    printf("\n============== Stock Report ==============\n");
    printf("ID   | Product Name       | Stock | Status\n");
    printf("------------------------------------------\n");
    
    while (fgets(line, sizeof(line), file)) {
        Product product;
        sscanf(line, "%d,%[^,],%*[^,],%*f,%d,%d",
               &product.id, product.name, &product.stock, &product.minimum_threshold);
        
        char status[50] = "Normal";
        if (product.stock <= product.minimum_threshold) {
            snprintf(status, sizeof(status), "Below Threshold");
        }
        
        printf("%-4d | %-12s       | %d  | %s\n",
            product.id,
            product.name,
            product.stock,
            status);
    }
    
    fclose(file);
}

// เพิ่มการทำงานของฟังก์ชันในไฟล์ owner.c
void getProductName(int productId, char* productName) {
    FILE *file = fopen("products.csv", "r");
    char line[500];
    
    if (file == NULL) {
        strcpy(productName, "Data not found");
        return;
    }
    
    while (fgets(line, sizeof(line), file)) {
        Product product;
        sscanf(line, "%d,%[^,],%*[^,],%*f,%*d,%*d",
               &product.id, product.name);
        
        if (product.id == productId) {
            strcpy(productName, product.name);
            fclose(file);
            return;
        }
    }
    
    strcpy(productName, "Data not found");
    fclose(file);
}

// ฟังก์ชันเติมสต็อกสินค้าด้วยตนเอง
void restockProduct(void) {
    int productId, amount;
    Product product;
    
    printf("\n=== Restock Product ===\n");
    printf("Enter Product ID: ");
    scanf("%d", &productId);
    
    // ตรวจสอบว่าสินค้ามีอยู่จริง
    if (!findProduct(productId, &product)) {
        printf("Invalid product ID %d\n", productId);
        return;
    }
    
    printf("Product: %s\n", product.name);
    printf("Current Stock: %d\n", product.stock);
    printf("Enter quantity to restock: ");
    scanf("%d", &amount);
    
    if (amount <= 0) {
        printf("Please enter a quantity greater than 0\n");
        return;
    }
    
    // อัพเดตสต็อก
    int newStock = product.stock + amount;
    if (updateProductStock(productId, newStock)) {
        if (!updateProductStock(productId, newStock)) {
            printf("Error updating stock\n");
            return;
        }
        // บันทึกประวัติการเติมสต็อก
        RestockRecord record = {
            .product_id = productId,
            .current_stock = newStock,
            .restock_amount = amount
        };
        time_t now = time(NULL);
        strftime(record.timestamp, sizeof(record.timestamp),
                "%Y-%m-%d %H:%M:%S", localtime(&now));
        
        logRestockActivity(record);
        printf("Stock updated successfully. New: %d\n", newStock);
    } else {
        printf("Error updating stock!\n");
    }
}

// ฟังก์ชันตรวจสอบและเติมสต็อกอัตโนมัติ
void autoRestockCheck(void) {
    FILE *file = fopen("products.csv", "r");
    char line[500];
    Product product;
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d,%[^,],%[^,],%lf,%d,%d",
               &product.id, product.name, product.description,
               &product.price, &product.stock, &product.minimum_threshold);
        
        // ตรวจสอบว่าสต็อกต่ำกว่าเกณฑ์หรือไม่
        if (product.stock <= product.minimum_threshold) {
            int restockAmount = (product.minimum_threshold * 2) - product.stock;
            int newStock = product.stock + restockAmount;
            
            if (updateProductStock(product.id, newStock)) {
                RestockRecord record = {
                    .product_id = product.id,
                    .current_stock = newStock,
                    .restock_amount = restockAmount
                };
                time_t now = time(NULL);
                strftime(record.timestamp, sizeof(record.timestamp),
                        "%Y-%m-%d %H:%M:%S", localtime(&now));
                
                logRestockActivity(record);
                printf("Automatic restock: %s quantity %d pieces\n",
                       product.name, restockAmount);
            }
        }
    }
    
    fclose(file);
}

// ฟังก์ชันบันทึกประวัติการเติมสต็อก
void logRestockActivity(RestockRecord record) {
    FILE *logFile = fopen("restock_history.txt", "a");
    fprintf(logFile, "%s,%d,%d,%d\n",
            record.timestamp,
            record.product_id,
            record.restock_amount,
            record.current_stock);
    fclose(logFile);
}

// ฟังก์ชันอัพเดตสต็อกสินค้า
int updateProductStock(int productId, int newStock) {
    const char* originalPath = "products.csv";
    const char* tempPath = "temp.csv";
    
    FILE *file = fopen(originalPath, "r");
    if (!file) {
        printf("Error: Cannot open products file\n");
        return 0;
    }
    
    FILE *tempFile = fopen(tempPath, "w");
    if (!tempFile) {
        printf("Error: Cannot create temp file\n");
        fclose(file);
        return 0;
    }
    
    char line[500];
    int success = 0;
    
    while (fgets(line, sizeof(line), file)) {
        Product product;
        sscanf(line, "%d,%[^,],%[^,],%lf,%d,%d",
               &product.id, product.name, product.description,
               &product.price, &product.stock, &product.minimum_threshold);
        
        if (product.id == productId) {
            product.stock = newStock;
            success = 1;
        }
        
        fprintf(tempFile, "%d,%s,%s,%.2f,%d,%d\n",
                product.id, product.name, product.description,
                product.price, product.stock, product.minimum_threshold);
    }
    
    fclose(file);
    fclose(tempFile);
    
    if (success) {
        if (remove(originalPath) != 0) {
            printf("Error: Cannot remove original file\n");
            remove(tempPath);
            return 0;
        }
        if (rename(tempPath, originalPath) != 0) {
            printf("Error: Cannot rename temp file\n");
            return 0;
        }
    } else {
        remove(tempPath);
    }
    
    return success;
}

// ฟังก์ชันค้นหาสินค้าจากรหัสสินค้า
int findProduct(int productId, Product* product) {
    FILE *file = fopen("products.csv", "r");
    char line[500];
    int found = 0;
    
    if (file == NULL) {
        return 0;
    }
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d,%[^,],%[^,],%lf,%d,%d",
               &product->id, product->name, product->description,
               &product->price, &product->stock, &product->minimum_threshold);
               
        if (product->id == productId) {
            found = 1;
            break;
        }
    }
    
    fclose(file);
    return found;
}

// ฟังก์ชันสร้างคูปองใหม่
void createCoupon(void) {
    FILE *file = fopen("coupons.txt", "a");
    Coupon coupon;
    
    printf("\n=== Create Discount Coupon ===\n");
    printf("Enter coupon code: ");
    scanf(" %[^\n]s", coupon.code);
    
    printf("Enter discount percentage (1-99): ");
    scanf("%f", &coupon.discount_percent);
    
    if (coupon.discount_percent <= 0 || coupon.discount_percent >= 100) {
        printf("Invalid discount percentage\n");
        fclose(file);
        return;
    }
    
    printf("Enter expiry date (YYYY-MM-DD): ");
    scanf(" %[^\n]s", coupon.expiry_date);
    
    coupon.is_active = 1;
    
    fprintf(file, "%s,%.2f,%s,%d\n",
            coupon.code, coupon.discount_percent,
            coupon.expiry_date, coupon.is_active);
    fclose(file);
    printf("Coupon created successfully\n");
}

// ฟังก์ชันแสดงรายการคูปองทั้งหมด
void listCoupons(void) {
    FILE *file = fopen("coupons.txt", "r");
    Coupon coupon;
    char line[100];
    
    printf("\n=== All Coupons List ===\n");
    printf("Coupon Code | Discount | Expiry Date | Status\n");
    printf("----------------------------------------\n");
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%f,%[^,],%d",
               coupon.code, &coupon.discount_percent,
               coupon.expiry_date, &coupon.is_active);
        
        printf("%s | %.0f%% | %s | %s\n",
               coupon.code, coupon.discount_percent,
               coupon.expiry_date,
               coupon.is_active ? "Active" : "Inactive");
    }
    
    fclose(file);
}

// ฟังก์ชันตรวจสอบความถูกต้องของคูปอง
int validateCoupon(const char* code, float* discount) {
    FILE *file = fopen("coupons.txt", "r");
    Coupon coupon;
    char line[100];
    int valid = 0;
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%f,%[^,],%d",
               coupon.code, &coupon.discount_percent,
               coupon.expiry_date, &coupon.is_active);
        
        if (strcmp(coupon.code, code) == 0) {
            if (coupon.is_active && !isCouponExpired(coupon.expiry_date)) {
                *discount = coupon.discount_percent;
                valid = 1;
            }
            break;
        }
    }
    
    fclose(file);
    return valid;
}

// ฟังก์ชันตรวจสอบว่าคูปองหมดอายุหรือยัง
int isCouponExpired(const char* expiryDate) {
    time_t now = time(NULL);
    struct tm tm_expiry = {0};
    
    // แปลงวันหมดอายุเป็น timestamp
    sscanf(expiryDate, "%d-%d-%d",
           &tm_expiry.tm_year, &tm_expiry.tm_mon, &tm_expiry.tm_mday);
    tm_expiry.tm_year -= 1900;
    tm_expiry.tm_mon -= 1;
    
    time_t expiry = mktime(&tm_expiry);
    
    return difftime(now, expiry) > 0;
}

// ฟังก์ชันยกเลิกคูปอง
void deleteCoupon(const char* code) {
    FILE *file = fopen("coupons.txt", "r");
    FILE *tempFile = fopen("temp.txt", "w");
    Coupon coupon;
    char line[100];
    int found = 0;
    
    if (file == NULL || tempFile == NULL) {
        printf("Error: Cannot open file!\n");
        return;
    }
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%f,%[^,],%d",
               coupon.code, &coupon.discount_percent,
               coupon.expiry_date, &coupon.is_active);
        
        // ถ้ารหัสคูปองไม่ตรงกับที่ต้องการลบ จะเก็บไว้ในไฟล์ชั่วคราว
        if (strcmp(coupon.code, code) != 0) {
            fprintf(tempFile, "%s,%.2f,%s,%d\n",
                    coupon.code, coupon.discount_percent,
                    coupon.expiry_date, coupon.is_active);
        } else {
            found = 1; // พบคูปองที่ต้องการลบ
        }
    }
    
    fclose(file);
    fclose(tempFile);

    // ลบไฟล์เดิมและเปลี่ยนชื่อไฟล์ชั่วคราว
    if (remove("coupons.txt") == 0 && rename("temp.txt", "coupons.txt") == 0) {
        if (found) {
            printf("Coupon %s deleted successfully\n", code);
        } else {
            printf("Coupon %s not found\n", code);
        }
    } else {
        printf("Error: File update failed!\n");
    }
}

// ฟังก์ชันอ่านข้อมูลจากไฟล์ CSV
void loadProductsFromCSV(void) {
    const char* CSV_PATH = "products.csv";  // กำหนดชื่อไฟล์เป็นค่าคงที่
    const char* DATA_PATH = "products.txt"; // กำหนดชื่อไฟล์เป้าหมายเป็นค่าคงที่
    
    FILE *csvFile = fopen(CSV_PATH, "r");
    if (csvFile == NULL) {
        perror("Error opening CSV file");
        printf("Cannot open file %s\n", CSV_PATH);
        printf("Please check:\n");
        printf("1. Is file %s in the same folder as the program?\n", CSV_PATH);
        printf("2. Does the program have file access permissions?\n");
        printf("3. Are the filename and extension correct?\n");
        return;
    }

    FILE *dataFile = fopen(DATA_PATH, "w");
    if (dataFile == NULL) {
        perror("Error opening data file");
        printf("Cannot open file %s\n", DATA_PATH);
        fclose(csvFile);  // ปิดไฟล์ CSV ก่อนออกจากฟังก์ชัน
        return;
    }

    char line[500];
    int lineCount = 0;
    int successCount = 0;

    // อ่านทีละบรรทัด
    while (fgets(line, sizeof(line), csvFile)) {
        lineCount++;
        
        // ข้าม header บรรทัดแรก
        if (lineCount == 1) {
            continue;
        }

        // ตรวจสอบความถูกต้องของข้อมูล
        if (strlen(line) > 0) {
            // ตัด newline ที่อาจมีอยู่ท้ายบรรทัด
            line[strcspn(line, "\n")] = 0;
            
            // เขียนข้อมูลลงในไฟล์
            fprintf(dataFile, "%s\n", line);
            successCount++;
        }
    }

    fclose(csvFile);
    fclose(dataFile);

    if (successCount > 0) {
        printf("Data loaded successfully: Imported %d items\n", successCount);
    } else {
        printf("No data found in CSV file or import failed\n");
    }
}

// ฟังก์ชันหา ID สินค้าล่าสุด
int getLastProductId(void) {
    FILE *file = fopen("products.csv", "r");
    char line[500];
    int lastId = 0;
    int currentId;
    
    if (file == NULL) {
        return 0;
    }
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d,", &currentId);
        if (currentId > lastId) {
            lastId = currentId;
        }
    }
    
    fclose(file);
    return lastId;
}

int findCoupon(const char* couponCode, Coupon* coupon) {
    FILE *file = fopen("coupons.txt", "r");
    if (file == NULL) return 0;
    
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        // อ่านข้อมูลคูปองจากไฟล์
        if (strstr(line, couponCode) != NULL) {
            sscanf(line, "%[^,],%f,%[^,],%d",
                coupon->code,
                &coupon->discount_percent,
                coupon->expiry_date,
                &coupon->is_active);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

// เพิ่มฟังก์ชัน deactivateCoupon
void deactivateCoupon(const char* code) {
    FILE *file = fopen("coupons.txt", "r");
    FILE *tempFile = fopen("temp.txt", "w");
    Coupon coupon;
    char line[100];
    int found = 0;
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%f,%[^,],%d",
               coupon.code, &coupon.discount_percent,
               coupon.expiry_date, &coupon.is_active);
        
        if (strcmp(coupon.code, code) == 0) {
            coupon.is_active = 0;  // ยกเลิกคูปอง
            found = 1;
        }
        
        fprintf(tempFile, "%s,%.2f,%s,%d\n",
                coupon.code, coupon.discount_percent,
                coupon.expiry_date, coupon.is_active);
    }
    
    fclose(file);
    fclose(tempFile);

    if (remove("coupons.txt") == 0 && rename("temp.txt", "coupons.txt") == 0) {
        if (found) {
            printf("Coupon %s deactivated successfully\n", code);
        } else {
            printf("Coupon %s not found\n", code);
        }
    } else {
        printf("Error: File update failed!\n");
    }
}
// ฟังก์ชันเติมสต็อกแบบ hybrid
void hybridRestock(void) {
    int productId;
    Product product;
    
    printf("\n=== Hybrid Restock System ===\n");
    printf("Enter Product ID: ");
    scanf("%d", &productId);
    
    // ตรวจสอบว่าพบสินค้าหรือไม่
    if (!findProduct(productId, &product)) {
        printf("Product ID %d not found\n", productId);
        return;
    }
    
    printf("\nProduct: %s\n", product.name);
    printf("Current Stock: %d\n", product.stock);
    printf("Minimum Threshold: %d\n", product.minimum_threshold);
    
    // แก้ไขการคำนวณจำนวนที่ต้องเติมอัตโนมัติ
    int autoRestockAmount = 0;
    // ถ้าสต็อกน้อยกว่าหรือเท่ากับเกณฑ์ขั้นต่ำ
    if (product.stock <= product.minimum_threshold) {
        // คำนวณจำนวนที่ต้องเติม = (เกณฑ์ขั้นต่ำ x 2) - สต็อกปัจจุบัน
        autoRestockAmount = (product.minimum_threshold * 2) - product.stock;
        printf("\nStock is below minimum threshold!");
        printf("\nAuto restock calculation:");
        printf("\n- Minimum threshold: %d", product.minimum_threshold);
        printf("\n- Target stock (2x threshold): %d", product.minimum_threshold * 2);
        printf("\n- Current stock: %d", product.stock);
        printf("\n- Auto restock amount: %d\n", autoRestockAmount);
    } else {
        printf("\nStock level is adequate (above minimum threshold)\n");
        printf("No automatic restocking needed\n");
    }
    
    // รับจำนวนที่ต้องการเติมด้วยตนเอง
    int manualRestockAmount = 0;
    printf("\nWould you like to add manual restock? (Enter amount, 0 for none): ");
    scanf("%d", &manualRestockAmount);
    
    if (manualRestockAmount < 0) {
        printf("Invalid manual restock amount\n");
        return;
    }
    
    // คำนวณจำนวนสต็อกใหม่
    int totalRestockAmount = autoRestockAmount + manualRestockAmount;
    if (totalRestockAmount == 0) {
        printf("No stock was added\n");
        return;
    }
    
    int newStock = product.stock + totalRestockAmount;
    
    // อัพเดตสต็อก
    if (updateProductStock(productId, newStock)) {
        // บันทึกประวัติ
        HybridRestockRecord record = {
            .product_id = productId,
            .manual_restock = manualRestockAmount,
            .auto_restock = autoRestockAmount,
            .final_stock = newStock
        };
        
        if (autoRestockAmount > 0 && manualRestockAmount > 0) {
            strcpy(record.restock_type, "HYBRID");
        } else if (autoRestockAmount > 0) {
            strcpy(record.restock_type, "AUTO");
        } else {
            strcpy(record.restock_type, "MANUAL");
        }
        
        time_t now = time(NULL);
        strftime(record.timestamp, sizeof(record.timestamp),
                "%Y-%m-%d %H:%M:%S", localtime(&now));
        
        logHybridRestock(record);
        
        printf("\nStock updated successfully!\n");
        printf("Auto restock: %d units\n", autoRestockAmount);
        printf("Manual restock: %d units\n", manualRestockAmount);
        printf("Total restock: %d units\n", totalRestockAmount);
        printf("New stock level: %d units\n", newStock);
    } else {
        printf("Error updating stock!\n");
    }
}

// ฟังก์ชันบันทึกประวัติการเติมสต็อกแบบ hybrid
void logHybridRestock(HybridRestockRecord record) {
    FILE *logFile = fopen("hybrid_restock_history.csv", "a");
    if (logFile == NULL) {
        printf("Error opening log file\n");
        return;
    }
    
    // บันทึกข้อมูลลงในไฟล์
    fprintf(logFile, "%s,%d,%d,%d,%s,%d\n",
            record.timestamp,
            record.product_id,
            record.manual_restock,
            record.auto_restock,
            record.restock_type,
            record.final_stock);
            
    fclose(logFile);
}

// ฟังก์ชันแสดงรายงานการเติมสต็อกแบบ hybrid
void showHybridRestockReport(void) {
    FILE *file = fopen("hybrid_restock_history.csv", "r");
    if (file == NULL) {
        printf("No restock history found\n");
        return;
    }
    
    char line[500];
    HybridRestockRecord record;
    
    printf("\n============= Hybrid Restock Report =============\n");
    printf("Time                | Product | Manual | Auto | Type   | Final Stock\n");
    printf("--------------------------------------------------------\n");
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%d,%d,%d,%[^,],%d",
               record.timestamp,
               &record.product_id,
               &record.manual_restock,
               &record.auto_restock,
               record.restock_type,
               &record.final_stock);
               
        // แสดงข้อมูลในรูปแบบตาราง
        printf("%-19s | %-7d | %-6d | %-4d | %-6s | %d\n",
               record.timestamp,
               record.product_id,
               record.manual_restock,
               record.auto_restock,
               record.restock_type,
               record.final_stock);
    }
    
    fclose(file);
}
// ฟังก์ชันเพิ่มสินค้าอัตโนมัติจากไฟล์ CSV
void automateAddProducts(void) {
    // ใช้ path เต็มเหมือนกับไฟล์อื่นๆ ในระบบ
    const char* outputPath = "adding_newproducts.csv";
    char inputPath[500];
    ImportResult result = {0, 0};
    
    printf("\n=== Automated Product Addition ===\n");
    printf("Note: CSV format should be: name,description,price,stock,minimum_threshold\n");
    printf("Enter input CSV file path: ");
    scanf(" %[^\n]", inputPath);
    
    // ตรวจสอบว่าไฟล์ input มีอยู่จริง
    FILE *inputFile = fopen(inputPath, "r");
    if (inputFile == NULL) {
        printf("Error: Cannot open input file '%s'\n", inputPath);
        printf("Please check:\n");
        printf("1. File path is correct\n");
        printf("2. File exists\n");
        printf("3. You have permission to read the file\n");
        return;
    }
    
    // เปิดไฟล์ products.csv เพื่อเพิ่มข้อมูล
    FILE *outputFile = fopen(outputPath, "a");
    if (outputFile == NULL) {
        printf("Error: Cannot open products file\n");
        fclose(inputFile);
        return;
    }
    
    char line[500];
    char header[500];
    // ข้าม header line
    fgets(header, sizeof(header), inputFile);
    
    // บันทึก log
    time_t now = time(NULL);
    char timestamp[26];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", localtime(&now));
    
    char logPath[600];
    snprintf(logPath, sizeof(logPath),
             "import_log_%s.txt",
             timestamp);
    
    FILE *logFile = fopen(logPath, "w");
    if (logFile == NULL) {
        printf("Warning: Cannot create log file\n");
        // ดำเนินการต่อแม้ไม่สามารถสร้างไฟล์ log ได้
    } else {
        fprintf(logFile, "Import started at: %s\n\n", timestamp);
    }
    
    // อ่านและเพิ่มสินค้า
    while (fgets(line, sizeof(line), inputFile)) {
        char name[100], description[200];
        double price;
        int stock, min_threshold;
        
        if (sscanf(line, "%[^,],%[^,],%lf,%d,%d",
                   name, description, &price, &stock, &min_threshold) == 5) {
            
            // ตรวจสอบความถูกต้องของข้อมูล
            if (price <= 0 || stock < 0 || min_threshold < 0) {
                result.fail_count++;
                if (logFile) {
                    fprintf(logFile, "Invalid data: %s\n", line);
                }
                continue;
            }
            
            // สร้าง ID ใหม่
            int newId = getLastProductId() + 1;
            
            // เขียนข้อมูลลงในไฟล์ products
            fprintf(outputFile, "%d,%s,%s,%.2f,%d,%d\n",
                    newId, name, description, price, stock, min_threshold);
            
            result.success_count++;
            if (logFile) {
                fprintf(logFile, "Added: %s (ID: %d)\n", name, newId);
            }
        } else {
            result.fail_count++;
            if (logFile) {
                fprintf(logFile, "Failed to parse: %s", line);
            }
        }
    }
    
    // สรุปผล
    printf("\nImport Summary:\n");
    printf("Successfully added: %d products\n", result.success_count);
    printf("Failed: %d products\n", result.fail_count);
    
    if (logFile) {
        fprintf(logFile, "\nFinal Summary:\n");
        fprintf(logFile, "Success: %d, Failed: %d\n",
                result.success_count, result.fail_count);
        fclose(logFile);
        printf("Detailed log saved to: %s\n", logPath);
    }
    
    // ปิดไฟล์ทั้งหมด
    fclose(inputFile);
    fclose(outputFile);
}
