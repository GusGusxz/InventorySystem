// shopping_cart.c
#include "shopping_cart.h"
#include "product_management.h"

void initializeCart(ShoppingCart* cart) {
    cart->itemCount = 0;
    cart->total = 0.0;
    cart->applied_coupon = NULL;
}

int addToCart(ShoppingCart* cart, Product* product, int quantity) {
    if (cart->itemCount >= MAX_CART) {
        printf("Cart is full!\n");
        return 0;
    }
    
    if (product->stock < quantity) {
        printf("Not enough stock available!\n");
        return 0;
    }
    
    // Check if product already in cart
    for (int i = 0; i < cart->itemCount; i++) {
        if (cart->items[i].product->id == product->id) {
            cart->items[i].quantity += quantity;
            cart->total += product->price * quantity;
            return 1;
        }
    }
    
    // Add new item to cart
    cart->items[cart->itemCount].product = product;
    cart->items[cart->itemCount].quantity = quantity;
    cart->total += product->price * quantity;
    cart->itemCount++;
    
    return 1;
}

void displayCart(ShoppingCart* cart) {
    printf("\n=== Shopping Cart ===\n");
    printf("Item\t\tQuantity\tPrice\tSubtotal\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < cart->itemCount; i++) {
        double subtotal = cart->items[i].product->price * cart->items[i].quantity;
        printf("%-16s%d\t\t$%.2f\t$%.2f\n",
               cart->items[i].product->name,
               cart->items[i].quantity,
               cart->items[i].product->price,
               subtotal);
    }
    
    printf("----------------------------------------\n");
    printf("Total: $%.2f\n", cart->total);
    
    if (cart->applied_coupon != NULL) {
        double discount = cart->total * (cart->applied_coupon->discount_percent / 100.0);
        printf("Discount: -$%.2f\n", discount);
        printf("Final Total: $%.2f\n", cart->total - discount);
    }
}

void applyCoupon(ShoppingCart* cart, Coupon* coupon) {
    // Simple coupon application - just checking expiration
    // In a real system, you'd want to validate the expiration date
    cart->applied_coupon = coupon;
    printf("Coupon applied successfully!\n");
}

void checkout(ShoppingCart* cart, Product* products, int productCount) {
    // Update stock levels
    for (int i = 0; i < cart->itemCount; i++) {
        cart->items[i].product->stock -= cart->items[i].quantity;
    }
    
    // Save updated product data
    saveProducts(products, productCount);
    
    // Clear cart
    initializeCart(cart);
    
    printf("Checkout completed successfully!\n");
}
