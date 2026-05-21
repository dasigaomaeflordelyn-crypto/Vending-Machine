#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 10
#define MAX_NAME 50

// Structure to hold Product details
typedef struct {
    char name[MAX_NAME];
    float price;
    int stock;
} Product;

// Global Variables
Product machine[MAX_PRODUCTS];
int total_products = 0;
float student_cash = 500.00;

// Function Declarations
void load_stock();
void save_stock();
void load_inventory();
void add_to_inventory(char *item_name, float price);
void view_products();
void buy_product();
void view_inventory();

int main() {
    int choice;

    // Load initial data from files
    load_stock();
    load_inventory();

    do {
        printf("\n===================================\n");
        printf("    CANTEEN VENDING MACHINE\n");
        printf("===================================\n");
        printf("[1] View Available Products\n");
        printf("[2] Buy a Product\n");
        printf("[3] View Personal Inventory & Cash\n");
        printf("[4] Exit\n");
        printf("===================================\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                view_products();
                break;
            case 2:
                buy_product();
                break;
            case 3:
                view_inventory();
                break;
            case 4:
                printf("Thank you for using the vending machine. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Please select between 1 and 4.\n");
        }
    } while (choice != 4);

    return 0;
}

// Loads machine stock from stock.txt
void load_stock() {
    FILE *file = fopen("stock.txt", "r");
    if (file == NULL) {
        printf("Error: Could not open stock.txt. Using default setup.\n");
        return;
    }

    total_products = 0;
    while (fscanf(file, "%s %f %d", machine[total_products].name, 
                  &machine[total_products].price, 
                  &machine[total_products].stock) != EOF) {
        total_products++;
        if (total_products >= MAX_PRODUCTS) break;
    }
    fclose(file);
}

// Saves updated machine stock back to stock.txt
void save_stock() {
    FILE *file = fopen("stock.txt", "w");
    if (file == NULL) {
        printf("Error: Could not save stock data.\n");
        return;
    }

    for (int i = 0; i < total_products; i++) {
        fprintf(file, "%s %.2f %d\n", machine[i].name, machine[i].price, machine[i].stock);
    }
    fclose(file);
}

// Loads student cash from the first line of inventory.txt
void load_inventory() {
    FILE *file = fopen("inventory.txt", "r");
    if (file == NULL) {
        // If file doesn't exist, create it with default ₱500
        student_cash = 500.00;
        FILE *new_file = fopen("inventory.txt", "w");
        if (new_file) {
            fprintf(new_file, "%.2f\n", student_cash);
            fclose(new_file);
        }
        return;
    }

    // Read the first line for remaining cash
    if (fscanf(file, "%f", &student_cash) == EOF) {
        student_cash = 500.00;
    }
    fclose(file);
}

// Appends a new purchased item and updates cash in inventory.txt
void add_to_inventory(char *item_name, float price) {
    // 1. Read all existing items from the file temporarily
    char items[100][MAX_NAME];
    float temp_price;
    int item_count = 0;

    FILE *file = fopen("inventory.txt", "r");
    if (file != NULL) {
        float dummy_cash;
        fscanf(file, "%f", &dummy_cash); // skip cash line
        while (fscanf(file, "%s %f", items[item_count], &temp_price) != EOF) {
            item_count++;
        }
        fclose(file);
    }

    // 2. Rewrite the file with updated cash and old items + new item
    file = fopen("inventory.txt", "w");
    if (file == NULL) {
        printf("Error writing to inventory file.\n");
        return;
    }

    fprintf(file, "%.2f\n", student_cash); // Write new cash balance
    for (int i = 0; i < item_count; i++) {
        fprintf(file, "%s 1\n", items[i]); // Re-write old items
    }
    fprintf(file, "%s 1\n", item_name); // Append new item
    fclose(file);
}

// Display available menu items
void view_products() {
    printf("\n--- AVAILABLE PRODUCTS ---\n");
    printf("%-4s %-15s %-10s %-5s\n", "No.", "Item Name", "Price", "Stock");
    printf("-----------------------------------\n");
    for (int i = 0; i < total_products; i++) {
        printf("[%d]  %-15s ₱%-9.2f %-5d\n", i + 1, machine[i].name, machine[i].price, machine[i].stock);
    }
}

// Handle the purchase transaction
void buy_product() {
    int prod_num;
    view_products();
    
    printf("\nEnter product number to buy: ");
    if (scanf("%d", &prod_num) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }

    int index = prod_num - 1;

    // Validation checks
    if (index < 0 || index >= total_products) {
        printf("Invalid product selection!\n");
        return;
    }

    if (machine[index].stock <= 0) {
        printf("Sorry, %s is out of stock!\n", machine[index].name);
        return;
    }

    if (student_cash < machine[index].price) {
        printf("Insolvent! You don't have enough cash. Remaining: ₱%.2f\n", student_cash);
        return;
    }

    // Process Purchase
    machine[index].stock--;
    student_cash -= machine[index].price;

    // Save changes instantly to files
    save_stock();
    add_to_inventory(machine[index].name, machine[index].price);

    printf("\nSuccessfully purchased %s!\n", machine[index].name);
    printf("Remaining Balance: ₱%.2f\n", student_cash);
}

// Display what the student has bought so far
void view_inventory() {
    FILE *file = fopen("inventory.txt", "r");
    char item_name[MAX_NAME];
    int qty;
    float current_cash;

    printf("\n--- STUDENT PROFILE & INVENTORY ---\n");
    if (file == NULL) {
        printf("Current Cash: ₱500.00\nNo items purchased yet.\n");
        return;
    }

    // Read remaining cash
    fscanf(file, "%f", &current_cash);
    printf("Remaining Cash: ₱%.2f\n", current_cash);
    printf("\nPurchased Items:\n");
    printf("-----------------------------------\n");

    int has_items = 0;
    while (fscanf(file, "%s %d", item_name, &qty) != EOF) {
        printf("- %-15s (Qty: %d)\n", item_name, qty);
        has_items = 1;
    }

    if (!has_items) {
        printf("(No items purchased yet)\n");
    }
    fclose(file);
}