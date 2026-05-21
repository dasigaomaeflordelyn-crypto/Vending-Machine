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

// Function Declarations
void load_stock();
void save_stock();
void load_inventory();
void add_to_inventory(char *category, char *item_name, int qty, float price);
int select_category_menu();
void view_products(int is_buying_mode); 
void execute_purchase(int index, int qty); 
void view_inventory();

int main() {
    int choice;
    load_stock();
    load_inventory();

    do {
        printf("\n===================================\n");
        printf("    CAMPUS MULTI-CATEGORY VENDING\n");
        printf("===================================\n");
        printf("[1] View Available Products\n");
        printf("[2] Buy a Product\n"); 
        printf("[3] View Personal Inventory & Cash\n");
        printf("[4] Exit\n");
        printf("===================================\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n'); 
            continue;
        }

        switch (choice) {
            case 1: 
                view_products(0); // Mode 0: Viewing only
                break; 
            case 2: 
                view_products(1); // Mode 1: Forces purchasing interaction path
                break; 
            case 3: 
                view_inventory(); 
                break; 
            case 4: 
                printf("Exiting system. Goodbye!\n"); 
                break;
            default: 
                printf("Invalid choice!\n");
        }
    } while (choice != 4);

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

int select_category_menu() {
    int cat_choice;
    printf("\n--- SELECT A CATEGORY ---\n");
    printf("[1] Soft Drinks\n");
    printf("[2] Snacks & Chips\n");
    printf("[3] Pastries & Biscuits\n");
    printf("[4] Healthy Options\n");
    printf("[5] Go Back to Main Menu\n");
    printf("-------------------------\n");
    printf("Enter category number: ");
    
    if (scanf("%d", &cat_choice) != 1) {
        while (getchar() != '\n');
        return -1;
    }
    return cat_choice;
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

void view_products(int is_buying_mode) {
    int keep_viewing = 1;
    int force_buy_action = is_buying_mode; 

    while (keep_viewing) {
        int cat_choice = select_category_menu();
        char target_cat[MAX_NAME];

        if (cat_choice == 5) {
            return; 
        }

        switch (cat_choice) {
            case 1: strcpy(target_cat, "Drinks"); break;
            case 2: strcpy(target_cat, "Snacks"); break;
            case 3: strcpy(target_cat, "Pastries"); break;
            case 4: strcpy(target_cat, "Healthy"); break;
            default:
                printf("Invalid category selection!\n");
                continue; 
        }

        int show_category_loop = 1;

        while (show_category_loop) {
            printf("\n-------------------------------------------------------\n");
            printf(" AVAILABLE PRODUCTS IN FILTER: %s\n", target_cat);
            printf("-------------------------------------------------------\n");
            printf("%-4s %-18s %-10s %-5s\n", "No.", "Item Name", "Price", "Stock");
            printf("-------------------------------------------------------\n");
            
            int display_num = 1; 
            for (int i = 0; i < total_products; i++) {
                if (strcmp(machine[i].category, target_cat) == 0) {
                    printf("[%2d] %-18s ₱%-9.2f %-5d\n", 
                           display_num, machine[i].name, machine[i].price, machine[i].stock);
                    display_num++;
                }
            }
            printf("-------------------------------------------------------\n");

            int sub_choice;
            if (force_buy_action) {
                sub_choice = 1;
            } else {
                printf("\nWhat would you like to do next?\n");
                printf("[1] Buy an item from this category\n");
                printf("[2] View a different category\n");
                printf("[3] Go back to Main Menu\n");
                printf("Enter choice: ");
                
                if (scanf("%d", &sub_choice) != 1) {
                    while (getchar() != '\n');
                    sub_choice = 3; 
                }
            }

            if (sub_choice == 1) {
                int prod_num, qty;
                printf("\nEnter the exact item 'No.' to buy (or 0 to cancel): ");
                if (scanf("%d", &prod_num) != 1) {
                    while (getchar() != '\n');
                    force_buy_action = 0;
                    continue;
                }
                
                // Process cancel request smoothly
                if (prod_num == 0) {
                    printf("\nPurchase cancelled. Returning to category selection...\n");
                    show_category_loop = 0;
                    force_buy_action = 0;
                    continue;
                }
                
                printf("Enter quantity to buy: ");
                if (scanf("%d", &qty) != 1) {
                    while (getchar() != '\n');
                    force_buy_action = 0;
                    continue;
                }

                int target_index = -1;
                int current_match = 1;
                for (int i = 0; i < total_products; i++) {
                    if (strcmp(machine[i].category, target_cat) == 0) {
                        if (current_match == prod_num) {
                            target_index = i;
                            break;
                        }
                        current_match++;
                    }
                }

                if (target_index == -1) {
                    printf("Invalid selection for this category list!\n");
                    force_buy_action = 0;
                } else {
                    execute_purchase(target_index, qty);
                }
                
                printf("\nDo you want to buy another product from this same category? (1 = Yes / 0 = No): ");
                int loop_again;
                if (scanf("%d", &loop_again) == 1 && loop_again == 1) {
                    force_buy_action = 1; 
                } else {
                    while (getchar() != '\n');
                    show_category_loop = 0; 
                    keep_viewing = 0; 
                }
            }
            else if (sub_choice == 2) {
                show_category_loop = 0; 
                force_buy_action = 0;
            }
            else if (sub_choice == 3) {
                show_category_loop = 0;
                keep_viewing = 0; 
            }
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