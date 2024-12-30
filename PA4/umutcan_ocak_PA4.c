#include <stdio.h>
#include <string.h>

// Define a structure to represent a module with an identifier (iden) and a coefficient (coeff)
typedef struct {
    char iden[30];
    int coeff;  
} module_t;

// Function to remove spaces from a string
void remove_spaces(char* def) {
    int i, j = 0;
    for (i = 0; def[i] != '\0'; i++) {
        if (def[i] != ' ') {
            def[j++] = def[i]; // Copy non-space characters
        }
    }
    def[j] = '\0'; // Null-terminate the string
}

// Function to parse modules from a definition string
// The function splits the string into terms and extracts coefficients and identifiers
int parse_modules(char* def, module_t* array_of_modules) {
    int number_of_terms = 0; // Counter for terms
    int coeff; // Temporary variable to store the coefficient
    char iden[30]; // Temporary variable to store the identifier
    
    char *token = strtok(def, "+"); // Split the string by '+' operator
    while (token != NULL) {
        iden[0] = '\0'; // Initialize identifier to an empty string
        if (sscanf(token, "%d*%s", &coeff, iden) == 2) {
            // If term has both coefficient and identifier
            strcpy(array_of_modules[number_of_terms].iden, iden); // Copy identifier
            array_of_modules[number_of_terms].coeff = coeff; // Store coefficient
        } else {
            // If term has only a coefficient
            array_of_modules[number_of_terms].coeff = coeff;
            array_of_modules[number_of_terms].iden[0] = '\0'; // No identifier
        }
        number_of_terms++; // Increment the term counter
        token = strtok(NULL, "+"); // Get the next term
    }
    return number_of_terms; // Return the total number of terms parsed
}

// Recursive function to evaluate the value of a module
double eval(char* name) {
    printf("Define %s:\n", name); // Prompt user to define the module
    char def[100];
    fgets(def, sizeof(def), stdin); // Read user input
    def[strcspn(def, "\n")] = '\0'; // Remove newline character
    remove_spaces(def); // Remove spaces from the definition

    module_t array_of_modules[100]; // Array to store parsed modules
    int number_of_terms = parse_modules(def, array_of_modules); // Parse the input

    double sum = 0; // Initialize the sum of terms
    for (int i = 0; i < number_of_terms; i++) {
        if (strlen(array_of_modules[i].iden) > 0) {
            // If the term has an identifier, recursively evaluate it
            double result = eval(array_of_modules[i].iden);  
            sum += result * array_of_modules[i].coeff; // Add the evaluated result
        } else {
            // If the term has no identifier, add the coefficient directly
            sum += array_of_modules[i].coeff;
        }
    }
    return sum; // Return the calculated sum
}

int main() {
    char start_module[30];
    printf("Module name?:\n"); // Prompt user to enter the name of the starting module
    fgets(start_module, sizeof(start_module), stdin); // Read module name from user
    start_module[strcspn(start_module, "\n")] = '\0';  // Remove newline character

    double result = eval(start_module); // Evaluate the starting module
    printf("%.0f\n", result); // Print the final result
    printf("The last line is the result of %s", start_module); // Output the name of the starting module
    return 0;
}
