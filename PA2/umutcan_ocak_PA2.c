#include <stdio.h>

#define X 2  /* Segment size */
#define Y 3  /* Maximum unique segments */

int main() {
    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        printf("Failed to open input file\n");
        return 1;
    }

    int segment[X] = {0};  /* Array to store the current segment */
    int count = 0;         /* Counter for the number of read numbers */
    int numbers[Y][X] = {0}; /* Array to store up to Y unique segments */
    int counts[Y] = {0};   /* Array to store the count of each unique segment */
    int remembered = 0;    /* Number of unique segments remembered */
    int number;            /* Variable to store the current number being read */

    /* Read numbers from the input file */
    while (fscanf(file, "%d", &number) == 1) {
        segment[count % X] = number; /* Store number in the current segment */
        count++;

        /* Check if a full segment has been read */
        if (count >= X) {
            int isNewSegment = 1; /* Flag to check if the segment is new */

            /* Check if the current segment matches any remembered segment */
            for (int i = 0; i < remembered; i++) {
                int isMatch = 1;
                for (int j = 0; j < X; j++) {
                    /* Compare current segment with remembered segment */
                    if (numbers[i][j] != segment[(count - X + j) % X]) {
                        isMatch = 0;
                        break;
                    }
                }
                if (isMatch) {
                    counts[i]++; /* Increment count for matching segment */
                    isNewSegment = 0; /* Mark as not new */
                    break;
                }
            }

            /* If it's a new segment, add it to the list of remembered segments */
            if (isNewSegment) {
                if (remembered < Y) {
                    /* If there is space, add new segment directly */
                    for (int j = 0; j < X; j++) {
                        numbers[remembered][j] = segment[(count - X + j) % X];
                    }
                    counts[remembered] = 1;
                    remembered++;
                } else {
                    /* If no space, remove oldest segment and add new one */
                    for (int i = 1; i < Y; i++) {
                        for (int j = 0; j < X; j++) {
                            numbers[i - 1][j] = numbers[i][j];
                        }
                        counts[i - 1] = counts[i];
                    }
                    for (int j = 0; j < X; j++) {
                        numbers[Y - 1][j] = segment[(count - X + j) % X];
                    }
                    counts[Y - 1] = 1;
                }
            }
        }
    }

    fclose(file);

    /* Open output file for writing results */
    FILE *outfile = fopen("output.txt", "w");
    if (outfile == NULL) {
        printf("Failed to open output file\n");
        return 1;
    }

    /* Write unique segments and their counts to the output file */
    for (int i = 0; i < remembered; i++) {
        for (int j = 0; j < X; j++) {
            fprintf(outfile, "%d ", numbers[i][j]);
        }
        fprintf(outfile, ": %d\n", counts[i]);
    }

    fclose(outfile);
    return 0;
}
