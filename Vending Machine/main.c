#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 25
#define MAX_NAME 50

typedef struct {
    char category[MAX_NAME];
    char name[MAX_NAME];
    float price;
    int stock;
} Product;

Product machine[MAX_PRODUCTS];
int total_products = 0;
float student_cash = 500.00;

char active_store_name[MAX_NAME] = "";
char active_store_category[MAX_NAME] = "";

// Function Declarations
void load_stock();
void save_stock();
void load_inventory();
void add_to_inventory(char *category, char *item_name, int qty, float price);
void choose_store_menu();
void show_store_dashboard();
void shop_store_products(); // Matches perfectly now!
void execute_purchase(int index, int qty); 
void view_inventory();

int main() {
    load_stock();
    load_inventory();
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
        fprintf(file, "%s %s %.2f %d\n", machine[i].category, machine[i].name, machine[i].price, machine[i].stock);
    }
    fclose(file);
}

void load_inventory() {
    FILE *file = fopen("inventory.txt", "r");
    if (file == NULL) {
        student_cash = 500.00;
        return;
    }
    if (fscanf(file, "%f", &student_cash) == EOF) {
        student_cash = 500.00;
    }
    fclose(file);
}

void add_to_inventory(char *category, char *item_name, int qty, float price) {
    char item_cats[100][MAX_NAME];
    char item_names[100][MAX_NAME];
    int item_qtys[100];
    float item_prices[100];
    int item_count = 0;

    FILE *file = fopen("inventory.txt", "r");
    if (file != NULL) {
        float dummy_cash;
        fscanf(file, "%f", &dummy_cash);
        while (fscanf(file, "%s %s %d %f", item_cats[item_count], item_names[item_count], &item_qtys[item_count], &item_prices[item_count]) != EOF) {
            item_count++;
        }
        fclose(file);
    }

    int found = 0;
    for (int i = 0; i < item_count; i++) {
        if (strcmp(item_names[i], item_name) == 0) {
            item_qtys[i] += qty;
            found = 1;
            break;
        }
    }

    file = fopen("inventory.txt", "w");
    if (file == NULL) return;

    fprintf(file, "%.2f\n", student_cash);
    for (int i = 0; i < item_count; i++) {
        fprintf(file, "%s %s %d %.2f\n", item_cats[i], item_names[i], item_qtys[i], item_prices[i]);
    }
    
    if (!found) {
        fprintf(file, "%s %s %d %.2f\n", category, item_name, qty, price);
    }
    fclose(file);
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
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n'); 
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
            case 5:
                printf("\nShutting down vending hub. Goodbye!\n");
                break;
            default: 
                printf("Invalid store choice!\n");
        }
    } while (choice != 5);
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

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n'); 
            continue;
        }

        switch (choice) {
            case 1: 
                shop_store_products(); // Fixed function name call
                break; 
            case 2: 
                view_inventory(); 
                break; 
            case 3: 
                printf("\nExiting %s...\n", active_store_name);
                break;
            default: 
                printf("Invalid choice!\n");
        }
    } while (choice != 3);
}

void execute_purchase(int index, int qty) {
    if (qty <= 0) {
        printf("\nInvalid quantity amount!\n");
        return;
    }
    if (machine[index].stock < qty) {
        printf("\nNot enough stock! Available: %d\n", machine[index].stock);
        return;
    }
    float total_cost = machine[index].price * qty;
    if (student_cash < total_cost) {
        printf("\nInsufficient balance! Total Cost: ₱%.2f | Wallet: ₱%.2f\n", total_cost, student_cash);
        return;
    }

    machine[index].stock -= qty;
    student_cash -= total_cost;

    save_stock();
    add_to_inventory(machine[index].category, machine[index].name, qty, machine[index].price);

    printf("\nSuccess! Dispensing %d pcs of %s.\n", qty, machine[index].name);
    printf("New Balance: ₱%.2f\n", student_cash);
}

void shop_store_products() { // Fixed function name definition
    int shopping = 1;

    while (shopping) {
        printf("\n-------------------------------------------------------\n");
        printf(" AVAILABLE PRODUCTS IN %s (%s)\n", active_store_name, active_store_category);
        printf("-------------------------------------------------------\n");
        printf("%-4s %-18s %-10s %-5s\n", "No.", "Item Name", "Price", "Stock");
        printf("-------------------------------------------------------\n");
        
        int display_num = 1; 
        for (int i = 0; i < total_products; i++) {
            if (strcmp(machine[i].category, active_store_category) == 0) {
                printf("[%2d] %-18s ₱%-9.2f %-5d\n", 
                       display_num, machine[i].name, machine[i].price, machine[i].stock);
                display_num++;
            }
        }
        printf("-------------------------------------------------------\n");

        int prod_num, qty;
        printf("Enter the item 'No.' to buy (or 0 to go back to Store Menu): ");
        if (scanf("%d", &prod_num) != 1) {
            while (getchar() != '\n');
            continue;
        }
        
        if (prod_num == 0) {
            shopping = 0;
            continue;
        }
        
        printf("Enter quantity to buy: ");
        if (scanf("%d", &qty) != 1) {
            while (getchar() != '\n');
            continue;
        }

        int target_index = -1;
        int current_match = 1;
        for (int i = 0; i < total_products; i++) {
            if (strcmp(machine[i].category, active_store_category) == 0) {
                if (current_match == prod_num) {
                    target_index = i;
                    break;
                }
                current_match++;
            }
        }

        if (target_index == -1) {
            printf("\nInvalid selection for this store list!\n");
        } else {
            execute_purchase(target_index, qty);
        }
        
        printf("\nDo you want to continue shopping in this store? (1 = Yes / 0 = No): ");
        int loop_again;
        if (scanf("%d", &loop_again) != 1 || loop_again != 1) {
            while (getchar() != '\n');
            shopping = 0; 
        }
    }
}

void view_inventory() {
    FILE *file = fopen("inventory.txt", "r");
    char cat[MAX_NAME], name[MAX_NAME];
    int qty;
    float price;
    float current_cash;
    float total_amount_spent = 0.00; 

    printf("\n=== STUDENT BALANCE & PURCHASED INVENTORY ===\n");
    if (file == NULL) {
        printf("Remaining Cash: ₱500.00\nNo historical purchases.\n");
        return;
    }

    fscanf(file, "%f", &current_cash);
    printf("Remaining Wallet Funds: ₱%.2f\n", current_cash);
    printf("\n%-15s %-18s %-12s %-10s\n", "Category", "Purchased Item", "Total Qty", "Unit Price");
    printf("------------------------------------------------------------\n");

    int items_exist = 0;
    while (fscanf(file, "%s %s %d %f", cat, name, &qty, &price) != EOF) {
        float combined_item_cost = price * qty;
        total_amount_spent += combined_item_cost; 
        
        printf("%-15s %-18s %-12d ₱%-10.2f\n", cat, name, qty, price);
        items_exist = 1;
    }
    
    if (!items_exist) {
        printf("(No matching purchases recorded yet)\n");
    } else {
        printf("------------------------------------------------------------\n");
        printf("TOTAL AMOUNT SPENT: ₱%.2f\n", total_amount_spent); 
    }
    fclose(file);
}