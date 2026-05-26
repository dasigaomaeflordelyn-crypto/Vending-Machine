#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 25

typedef struct {
    char category[50];
    char name[50];
    float price;
    int stock;
} Product;

Product machine[MAX_PRODUCTS];
int total_products = 0;

// The base wallet balance tracked cleanly in code memory
float student_cash = 500.00; 

char active_store_name[50] = "";
char active_store_category[50] = "";

// Function Declarations
void load_stock();
void save_stock();
void log_purchase_receipt(char *name, int qty, float price);
void choose_store_menu();
void show_store_dashboard();
void shop_store_products();
void view_inventory();

int main() {
    load_stock(); // Loads initial configuration structures
    choose_store_menu();
    return 0;
}

void load_stock() {
    FILE *file = fopen("stock.txt", "r");
    if (file == NULL) {
        printf("Error: stock.txt missing!\n");
        return;
    }
    total_products = 0;
    while (fscanf(file, "%s %s %f %d", 
                  machine[total_products].category, 
                  machine[total_products].name, 
                  &machine[total_products].price, 
                  &machine[total_products].stock) != EOF) {
        total_products++;
        if (total_products >= MAX_PRODUCTS) break;
    }
    fclose(file);
}

void save_stock() {
    FILE *file = fopen("stock.txt", "w");
    if (file == NULL) return;
    for (int i = 0; i < total_products; i++) {
        fprintf(file, "%s %s %.2f %d\n", 
                machine[i].category, machine[i].name, machine[i].price, machine[i].stock);
    }
    fclose(file);
}

void log_purchase_receipt(char *name, int qty, float price) {
    FILE *file = fopen("inventory.txt", "a"); 
    if (file != NULL) {
        // Appends 4 strictly mapped data fields to avoid rendering alignment offsets
        fprintf(file, "%s %s %d %.2f\n", active_store_category, name, qty, price);
        fclose(file);
    }
}

void choose_store_menu() {
    int choice;
    do {
        printf("\n===================================\n");
        printf("    SELECT A CAMPUS STOREFRONT\n");
        printf("===================================\n");
        printf("[1] Mae's Store         (Soft Drinks)\n");
        printf("[2] Camille's Store     (Snacks & Chips)\n");
        printf("[3] Alven's Store       (Pastries & Biscuits)\n");
        printf("[4] Louie's Store       (Healthy Options)\n");
        printf("[5] Shut Down System\n");
        printf("===================================\n");
        printf("Enter choice: ");
        
        // Input validation for menu selection character inputs
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Please enter a valid number.\n");
            while (getchar() != '\n'); // Purge bad characters from the scanner buffer
            choice = 0; 
            continue;
        }

        // Out-of-bounds menu integer validation
        if (choice < 1 || choice > 5) {
            printf("\nInvalid input! Option out of range. Choose [1-5].\n");
            continue;
        }

        switch (choice) {
            case 1: 
                strcpy(active_store_name, "Mae's Store");
                strcpy(active_store_category, "Drinks");
                show_store_dashboard();
                break; 
            case 2: 
                strcpy(active_store_name, "Camille's Store");
                strcpy(active_store_category, "Snacks");
                show_store_dashboard();
                break; 
            case 3: 
                strcpy(active_store_name, "Alven's Store");
                strcpy(active_store_category, "Pastries");
                show_store_dashboard();
                break; 
            case 4: 
                strcpy(active_store_name, "Louie's Store");
                strcpy(active_store_category, "Healthy");
                show_store_dashboard();
                break; 
        }
    } while (choice != 5);
    printf("\nShutting down vending hub. Goodbye!\n");
}

void show_store_dashboard() {
    int choice;
    do {
        printf("\n===================================\n");
        printf("    WELCOME TO: %s\n", active_store_name);
        printf("    Category Hub: %s\n", active_store_category);
        printf("===================================\n");
        printf("[1] Browse & Shop Products\n"); 
        printf("[2] View Personal Inventory & Cash\n"); 
        printf("[3] Go Back to Store Selection Menu\n");
        printf("===================================\n");
        printf("Enter choice: ");

        // Input validation for storefront dashboard selection
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Please enter a valid number.\n");
            while (getchar() != '\n'); 
            choice = 0;
            continue;
        }

        if (choice < 1 || choice > 3) {
            printf("\nInvalid input! Option out of range. Choose [1-3].\n");
            continue;
        }

        if (choice == 1) shop_store_products();
        if (choice == 2) view_inventory();

    } while (choice != 3);
    printf("\nExiting %s...\n", active_store_name);
}

void shop_store_products() {
    printf("\n-------------------------------------------------------\n");
    printf(" AVAILABLE PRODUCTS IN %s (%s)\n", active_store_name, active_store_category);
    printf("-------------------------------------------------------\n");
    printf("%-4s %-18s %-10s %-5s\n", "No.", "Item Name", "Price", "Stock");
    printf("-------------------------------------------------------\n");
    
    int display_num = 1; 
    int item_indices[MAX_PRODUCTS]; 
    
    // Safety init array bounds to avoid Segmentation Fault core dumps
    for(int k = 0; k < MAX_PRODUCTS; k++) {
        item_indices[k] = -1;
    }
    
    for (int i = 0; i < total_products; i++) {
        if (strcmp(machine[i].category, active_store_category) == 0) {
            printf("[%2d] %-18s ₱%-9.2f %-5d\n", display_num, machine[i].name, machine[i].price, machine[i].stock);
            item_indices[display_num] = i; 
            display_num++;
        }
    }
    printf("-------------------------------------------------------\n");

    if (display_num == 1) {
        printf("(This store currently has no products configured in stock.txt)\n");
        printf("Press Enter to go back...");
        while (getchar() != '\n');
        getchar();
        return;
    }

    int choice_no, qty;
    printf("Enter Item 'No.' to buy (or 0 to go back): ");
    
    // Validates if input item reference index is numeric
    if (scanf("%d", &choice_no) != 1) {
        printf("\nInvalid input! Please enter an integer number.\n");
        while (getchar() != '\n');
        return;
    }
    
    if (choice_no == 0) return;
    
    // Validates if the selected choice belongs to the displayed list items
    if (choice_no < 1 || choice_no >= display_num || item_indices[choice_no] == -1) {
        printf("\nInvalid input! Selected item number does not exist.\n");
        return;
    }

    printf("Enter quantity to purchase: ");
    
    // Validates numeric entry for purchase quantity
    if (scanf("%d", &qty) != 1) {
        printf("\nInvalid input! Quantity must be a solid number.\n");
        while (getchar() != '\n');
        return;
    }
    
    if (qty <= 0) {
        printf("\nInvalid input! Quantity must be at least 1.\n");
        return;
    }

    int db_index = item_indices[choice_no];

    if (machine[db_index].stock < qty) {
        printf("\nNot enough stock! Available: %d\n", machine[db_index].stock);
        return;
    }

    float cost = machine[db_index].price * qty;
    if (student_cash < cost) {
        printf("\nInsufficient money! Total Cost: ₱%.2f | Balance: ₱%.2f\n", cost, student_cash);
        return;
    }

    machine[db_index].stock -= qty;
    student_cash -= cost;

    save_stock();
    log_purchase_receipt(machine[db_index].name, qty, machine[db_index].price);

    printf("\nSuccess! Disbursed %d unit(s) of %s.\n", qty, machine[db_index].name);
    
    if (qty == 1) {
        printf("Total Amount Used: ₱%.2f\n", cost);
    } else {
        printf("Total Amount Used: ₱%.2f (₱%.2f each)\n", cost, machine[db_index].price);
    }
    
    printf("Remaining Wallet Funds: ₱%.2f\n", student_cash);
}

void view_inventory() {
    printf("\n=== STUDENT BALANCE & PURCHASED INVENTORY ===\n");
    printf("Remaining Wallet Funds: ₱%.2f\n", student_cash);
    
    FILE *file = fopen("inventory.txt", "r");
    if (file == NULL) {
        printf("\nNo transactions logged yet.\n");
        return;
    }

    char cat[50], name[50];
    int qty;
    float price;
    float grand_total = 0.00;

    printf("\n%-15s %-18s %-12s %-10s\n", "Category", "Purchased Item", "Total Qty", "Unit Price");
    printf("------------------------------------------------------------\n");

    // Seamless table parsing logic for four parameters per string database row
    while (fscanf(file, "%s %s %d %f", cat, name, &qty, &price) != EOF) {
        float item_line_total = price * qty;
        grand_total += item_line_total;
        printf("%-15s %-18s %-12d ₱%-10.2f\n", cat, name, qty, price);
    }
    fclose(file);

    printf("------------------------------------------------------------\n");
    printf("TOTAL AMOUNT SPENT IN HUB: ₱%.2f\n", grand_total);
}


