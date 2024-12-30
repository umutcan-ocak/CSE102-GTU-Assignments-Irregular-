#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUTS 10
#define INITIAL_CAPACITY 10

// Enum for gate types
typedef enum { INPUT, OUTPUT, AND, OR, NOT, FLIPFLOP } GateType;

// Structure to represent a logic gate
typedef struct gate {
    GateType type;                       // Type of the gate
    char name[20];                       // Name of the gate
    struct gate* input_gates[MAX_INPUTS];// Pointers to input gates
    int number_of_inputs;                // Number of inputs
    int output;                          // Current output
    int former_out;                      // Previous output for FLIPFLOP
    short evaluated;                     // Flag to check if gate has been evaluated
    int (*characteristic_function)(int*, int); // Pointer to logic function
} Gate;

// Function prototypes
Gate* create_gate(GateType type, const char* name);
void create_connection(Gate** gates, int gate_count, const char* from, const char* to);
Gate* find_gate_by_name(Gate** gates, int gate_count, const char* name);
void reset_gates(Gate** gates, int gate_count);
int evaluate_gate(Gate* gate);
void free_gates(Gate** gates, int count);

// Logic functions for different gate types
int and_function(int* inputs, int n);
int or_function(int* inputs, int n);
int not_function(int* inputs, int n);
int flipflop_function(int* inputs, int n);

int main() {
    // Allocate memory for an array of gate pointers
    Gate** gates = malloc(INITIAL_CAPACITY * sizeof(Gate*));
    if (!gates) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }
    int gate_count = 0, capacity = INITIAL_CAPACITY;

    // Open and read the circuit configuration file
    FILE* circuit_file = fopen("circuit.txt", "r");
    if (!circuit_file) {
        fprintf(stderr, "Error: Could not open circuit.txt\n");
        free(gates);
        return EXIT_FAILURE;
    }

    char line[100], command[20], type[20], name[20];
    while (fgets(line, sizeof(line), circuit_file)) {
        if (sscanf(line, "%s %s %s", command, type, name) == 3) {
            if (strcmp(command, "GATE") == 0) {
                // Check if we need to expand the array
                if (gate_count >= capacity) {
                    capacity *= 2;
                    Gate** new_gates = realloc(gates, capacity * sizeof(Gate*));
                    if (!new_gates) {
                        fprintf(stderr, "Memory reallocation failed\n");
                        free_gates(gates, gate_count);
                        fclose(circuit_file);
                        return EXIT_FAILURE;
                    }
                    gates = new_gates;
                }
                // Create a new gate based on the type and name
                GateType gate_type = strcmp(type, "INPUT") == 0 ? INPUT :
                                     strcmp(type, "OUTPUT") == 0 ? OUTPUT :
                                     strcmp(type, "AND") == 0 ? AND :
                                     strcmp(type, "OR") == 0 ? OR :
                                     strcmp(type, "NOT") == 0 ? NOT : FLIPFLOP;
                gates[gate_count++] = create_gate(gate_type, name);
            } else if (strcmp(command, "CONNECTION") == 0) {
                // Create a connection between gates
                create_connection(gates, gate_count, type, name);
            }
        }
    }
    fclose(circuit_file);

    // Open and read input values for simulation
    FILE* input_file = fopen("input.txt", "r");
    if (!input_file) {
        fprintf(stderr, "Error: Could not open input.txt\n");
        free_gates(gates, gate_count);
        return EXIT_FAILURE;
    }

    while (fgets(line, sizeof(line), input_file)) {
        int input_index = 0;

        // Set input values for each INPUT gate
        for (int i = 0; i < gate_count; i++) {
            if (gates[i]->type == INPUT) {
                gates[i]->output = line[input_index++] - '0';
                gates[i]->evaluated = 0; 
            }
        }

        reset_gates(gates, gate_count);

        // Evaluate all OUTPUT gates and print their outputs
        for (int i = 0; i < gate_count; i++) {
            if (gates[i]->type == OUTPUT) {
                printf("%d", evaluate_gate(gates[i]));
            }
        }
        printf("\n"); // New line for each set of outputs
    }
    fclose(input_file);

    // Clean up allocated memory
    free_gates(gates, gate_count);
    return 0;
}

// Create and initialize a new gate
Gate* create_gate(GateType type, const char* name) {
    Gate* new_gate = malloc(sizeof(Gate));
    new_gate->type = type;
    strncpy(new_gate->name, name, 20);
    new_gate->number_of_inputs = 0;
    new_gate->output = 0;
    new_gate->former_out = 0;
    new_gate->evaluated = 0;

    switch (type) {
        case AND: new_gate->characteristic_function = and_function; break;
        case OR: new_gate->characteristic_function = or_function; break;
        case NOT: new_gate->characteristic_function = not_function; break;
        case FLIPFLOP: new_gate->characteristic_function = flipflop_function; break;
        default: new_gate->characteristic_function = NULL; break;
    }
    return new_gate;
}

// Create connection from one gate to another
void create_connection(Gate** gates, int gate_count, const char* from, const char* to) {
    Gate* from_gate = find_gate_by_name(gates, gate_count, from);
    Gate* to_gate = find_gate_by_name(gates, gate_count, to);

    if (from_gate && to_gate) {
        to_gate->input_gates[to_gate->number_of_inputs++] = from_gate;
    } else {
        printf("Error: Connection Failed: %s -> %s\n", from, to);
    }
}

// Find gate by its name in the array of gates
Gate* find_gate_by_name(Gate** gates, int gate_count, const char* name) {
    for (int i = 0; i < gate_count; i++) {
        if (strcmp(gates[i]->name, name) == 0) return gates[i];
    }
    return NULL;
}

// Reset gates for new evaluation, keeping INPUT gates' outputs
void reset_gates(Gate** gates, int gate_count) {
    for (int i = 0; i < gate_count; i++) {
        gates[i]->evaluated = 0; // Reset only the evaluated flag
        // Do NOT reset output here for INPUT gates!
        if (gates[i]->type != INPUT) {
            gates[i]->output = 0; // Reset output for non-INPUT gates
        }
    }
}

// Evaluate the output of a gate recursively
int evaluate_gate(Gate* gate) {
    // If the gate has already been evaluated, return the cached result
    if (gate->evaluated) {
        return gate->output;
    }

    // Handle INPUT gates - they return their set output directly
    if (gate->type == INPUT) {
        gate->evaluated = 1;
        return gate->output;
    }

    int inputs[MAX_INPUTS + 1]; // +1 for FLIPFLOP's former_out
    int num_inputs = gate->number_of_inputs;

    // Evaluate all input gates recursively
    for (int i = 0; i < num_inputs; i++) {
        inputs[i] = evaluate_gate(gate->input_gates[i]);
    }

    // Special handling for FLIPFLOP gates
    if (gate->type == FLIPFLOP) {
        inputs[num_inputs] = gate->former_out;
        num_inputs++; // Include former_out in the count
    }

    // Use the characteristic function to evaluate the gate
    if (gate->characteristic_function) {
        gate->output = gate->characteristic_function(inputs, num_inputs);
    } else if (gate->type == OUTPUT) {
        // OUTPUT gates should directly return the result of their input
        if (num_inputs > 0) {
            gate->output = inputs[0]; // Assuming OUTPUT gates have one input
        } else {
            // If no inputs, set to 0 or handle as an error
            gate->output = 0;
        }
    } else {
        // This should not happen if gates are correctly initialized, but as a fallback:
        gate->output = 0;
    }

    // Update FLIPFLOP state if applicable
    if (gate->type == FLIPFLOP) {
        gate->former_out = gate->output;
    }

    // Mark the gate as evaluated
    gate->evaluated = 1;

    return gate->output;
}

// Free all allocated memory for gates
void free_gates(Gate** gates, int count) {
    for (int i = 0; i < count; i++) free(gates[i]);
    free(gates);
}

// Logic Functions

// AND gate: returns 1 if all inputs are 1, otherwise 0
int and_function(int* inputs, int n) {
    for (int i = 0; i < n; i++) if (!inputs[i]) return 0;
    return 1;
}

// OR gate: returns 1 if at least one input is 1, otherwise 0
int or_function(int* inputs, int n) {
    for (int i = 0; i < n; i++) if (inputs[i]) return 1;
    return 0;
}

// NOT gate: inverts the single input
int not_function(int* inputs, int n) {
    return !inputs[0];
}

// FLIPFLOP gate: toggles state based on input and previous state
int flipflop_function(int* inputs, int n) {
    int a = inputs[0];         // Current input
    int former_out = inputs[1]; // Previous state
    return (a == 0) ? former_out : !former_out;
}