#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define NUM_BIGRAMS 729 
#define MAX_FILENAME_LEN 50

typedef struct {
    char bigram[3];  // To store the bi-gram (e.g., "a " or "aa")
    int count;       // Frequency count of this bigram
} BigramCount;

// Find the index of the bigram in the array, return -1 if not found
int find_bigram(BigramCount *bigrams, int num_bigrams, const char *bigram) {
    for (int i = 0; i < num_bigrams; i++) {
        if (strcmp(bigrams[i].bigram, bigram) == 0) {
            return i;
        }
    }
    return -1;  // Not found
}

// Compute frequencies for the bigrams
void compute_frequencies(BigramCount *bigrams, int num_bigrams, int total_bigrams, double *frequencies) {
    // Reset frequencies
    for (int i = 0; i < NUM_BIGRAMS; i++) {
        frequencies[i] = 0.0;
    }

    // Compute frequency distribution for each bigram
    for (int i = 0; i < num_bigrams; i++) {
        char first_char = bigrams[i].bigram[0];
        char second_char = bigrams[i].bigram[1];

        int first_index;
        if (first_char == ' ') {
            first_index = 26;
        } 
        else {
            first_index = first_char - 'a';
        }
        int second_index;
        if (second_char == ' ') {
            second_index = 26;
        }else {
            second_index = second_char - 'a';
        }

        int index = first_index * 27 + second_index;
        frequencies[index] = (double)bigrams[i].count / total_bigrams;
    }
}

// Calculate the dissimilarity between two frequency distributions
double calculate_dissimilarity(double *freq_x, double *freq_y, int num_bigrams) {
    double dissimilarity = 0.0;
    for (int i = 0; i < num_bigrams; i++) {
        dissimilarity += fabs(freq_x[i] - freq_y[i]);
    }
    return dissimilarity;
}

// Calculate bigrams and their frequencies from the input file
void calculate_bigrams(FILE *file, BigramCount *bigrams, int *total_bigrams, int *num_bigrams) {
    char prev = '\0'; 
    char curr;

    while ((curr = fgetc(file)) != EOF) {
        curr = tolower(curr); // Convert to lowercase

        // Allow letters, spaces, and tab characters
        if ((curr < 'a' || curr > 'z') && curr != ' ' && curr != '\t') {
            continue;  // Ignore other characters
        }

        if (prev != '\0') {  
            // Skip if both characters are spaces, since "  " is not a valid bigram
            if (prev == ' ' && curr == ' ') {
                continue;
            }

            char bigram[3] = {prev, curr, '\0'};

            // Check if the bigram exists and update its frequency
            int index = find_bigram(bigrams, *num_bigrams, bigram);
            if (index == -1) {
                if (*num_bigrams >= NUM_BIGRAMS) {
                    fprintf(stderr, "Error: Too many unique bigrams.\n");
                    return;
                }
                strcpy(bigrams[*num_bigrams].bigram, bigram);
                bigrams[*num_bigrams].count = 1;
                (*num_bigrams)++;
            } else {
                bigrams[index].count++;
            }
            (*total_bigrams)++;
        }

        prev = curr; // Store current character for next iteration
    }
}
int main() {
    char *reference_file = "language_x.txt";  // Your reference file

    BigramCount bigrams_x[NUM_BIGRAMS] = {0};
    double freq_x[NUM_BIGRAMS] = {0};
    int total_bigrams_x = 0, num_bigrams_x = 0;

    // Open the reference file and calculate bigrams
    FILE *file_x = fopen(reference_file, "r");
    if (!file_x) {
        perror("Error opening reference file");
        return 1;
    }
    calculate_bigrams(file_x, bigrams_x, &total_bigrams_x, &num_bigrams_x);
    fclose(file_x);

    // Compute the frequencies for the bigrams
    compute_frequencies(bigrams_x, num_bigrams_x, total_bigrams_x, freq_x);

    // Print out the bigrams and their frequencies for the reference file


    int file_index = 1;
    while (1) {
        char comparison_file[MAX_FILENAME_LEN];
        snprintf(comparison_file, MAX_FILENAME_LEN, "language_%d.txt", file_index);

        FILE *file_i = fopen(comparison_file, "r");
        if (!file_i) {
            if (file_index == 1) {
                fprintf(stderr, "No comparison files found.\n");
                return 1;
            }
            break;
        }

        BigramCount bigrams_i[NUM_BIGRAMS] = {0};
        double freq_i[NUM_BIGRAMS] = {0};
        int total_bigrams_i = 0, num_bigrams_i = 0;

        calculate_bigrams(file_i, bigrams_i, &total_bigrams_i, &num_bigrams_i);
        fclose(file_i);

        // Compute the frequencies for the bigrams of the comparison file
        compute_frequencies(bigrams_i, num_bigrams_i, total_bigrams_i, freq_i);

        // Calculate and print the dissimilarity
        double dissimilarity = calculate_dissimilarity(freq_x, freq_i, NUM_BIGRAMS);
        printf("%.6f\n",dissimilarity);

        file_index++;
    }

    return 0;
}