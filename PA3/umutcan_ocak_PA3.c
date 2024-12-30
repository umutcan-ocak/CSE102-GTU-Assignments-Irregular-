#include <stdio.h>
#include <math.h>

/* Function to remove spaces from the polynomial string */
void removeSpaces(char *polinom);

int main()
{   
    /* Open input and output files */
    FILE *fptr = fopen("values.txt", "r");  /* File containing x, y values */
    FILE *fp = fopen("polynomials.txt", "r"); /* File containing polynomial expressions */
    FILE *outfile = fopen("evaluations.txt", "w"); /* File to store evaluation results */
    
    double x = 0.0, y = 0.0;  /* Variables to store x and y values */
    char polinom[1000];  /* Array to store each polynomial expression */
    char values[200];    /* Array to store each pair of x, y values */
    double temp;          /* Temporary variable for parsing numbers */
    double result = 1.0, last_result = 0.0;  /* Variables to hold intermediate and final results */
    int square;  /* Variable to store exponent value */
    
    /* Check if files were successfully opened */
    if (fptr == NULL || fp == NULL || outfile == NULL) {
        printf("The file could not be opened.\n");
        return 1;  /* Exit if any file failed to open */
    }
    
    /* Read each line from the values file (x, y pairs) */
    while (fgets(values, sizeof(values), fptr) != NULL)
    {   
        /* Parse x and y from the current line */
        sscanf(values, "%lf %lf", &x, &y);
        
        /* Reset the polynomial file pointer to the beginning for each x, y pair */
        fseek(fp, 0, SEEK_SET);

        /* Read each polynomial expression from the polynomial file */
        while (fgets(polinom, sizeof(polinom), fp) != NULL)
        {
            /* Remove spaces from the polynomial expression */
            removeSpaces(polinom);
            
            int i = 0;
            last_result = 0.0;  /* Reset last result */
            result = 1.0;       /* Reset result for each polynomial */
            
            /* Iterate through the characters in the polynomial */
            while (polinom[i] != '\0') {    
                
                /* If the character is a number (e.g., coefficient) */
                if ((polinom[i] >= '0' && polinom[i] <= '9') || polinom[i] == '.') {
                    /* Parse the number and multiply it to the result */
                    sscanf(polinom + i, "%lf", &temp);
                    result *= temp;
                    
                    /* Skip over the digits and decimal point */
                    while ((polinom[i] >= '0' && polinom[i] <= '9') || polinom[i] == '.') {
                        i++;
                    }                                            
                }

                /* If the character is 'y' (variable y) */
                else if (polinom[i] == 'y') {              
                    i++;  /* Move past 'y' */
                    
                    /* If there's an exponent following 'y' */
                    if (polinom[i] == '^') {
                        i++;  /* Move past '^' */
                        
                        /* Parse the exponent and compute y raised to that power */
                        if ((polinom[i] >= '0' && polinom[i] <= '9') || polinom[i] == '.') {
                            sscanf(polinom + i, "%d", &square);
                            result *= pow(y, square);
                            
                            /* Skip over the digits of the exponent */
                            while ((polinom[i] >= '0' && polinom[i] <= '9') || polinom[i] == '.') {
                                i++;
                            }                                   
                        }                        
                    } else {
                        /* If no exponent, simply multiply by y */
                        result *= y;
                    }            
                }
                   
                /* If the character is 'x' (variable x) */
                else if (polinom[i] == 'x') {    
                    i++;  /* Move past 'x' */
                    
                    /* If there's an exponent following 'x' */
                    if (polinom[i] == '^') {
                        i++;  /* Move past '^' */
                        
                        /* Parse the exponent and compute x raised to that power */
                        if ((polinom[i] >= '0' && polinom[i] <= '9') || polinom[i] == '.') {
                            sscanf(polinom + i, "%d", &square);
                            result *= pow(x, square);
                            
                            /* Skip over the digits of the exponent */
                            while ((polinom[i] >= '0' && polinom[i] <= '9') || polinom[i] == '.') {
                                i++;
                            }                                   
                        }                        
                    } else {
                        /* If no exponent, simply multiply by x */
                        result *= x;
                    }    
                }
                
                /* If the character is a '+' (addition) */
                else if (polinom[i] == '+') {
                    /* Add the current result to the last result */
                    last_result += result;
                    result = 1.0;  /* Reset result for the next term */
                    i++;  /* Move past '+' */
                }
                /* If the character is a '-' (subtraction) */
                else if (polinom[i] == '-') {
                    /* Add the current result to the last result with a negative sign */
                    last_result += result;
                    result = -1.0;  /* Set result to negative for the next term */
                    i++;  /* Move past '-' */
                } else {
                    i++;  /* Skip other characters */
                }
            }
            
            /* Add the last computed result to the final result for the polynomial */
            last_result += result;
            
            /* Write the result of evaluating this polynomial to the output file */
            fprintf(outfile, "%.2lf ", last_result); 
        }
    }
    
    /* Close all opened files */
    fclose(outfile);
    fclose(fptr);
    fclose(fp);

    return 0;  /* Program executed successfully */
}

/* Function to remove spaces from the polynomial string */
void removeSpaces(char *polinom) {
    int i = 0, j = 0;
    
    /* Iterate through the string and remove spaces */
    while (polinom[i] != '\0') {
        if (polinom[i] != ' ') {
            polinom[j++] = polinom[i];  /* Copy non-space characters */
        }
        i++;
    }
    polinom[j] = '\0';  /* Null-terminate the modified string */
}

