#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 1024    // Maximum allowed grid size
#define MAX_GROUPS 127   // Maximum number of unique labels (colors)

// Color palette for labeling connected components
int colors[127][3] = {
    {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {0, 255, 255}, {255, 0, 255},
    {128, 0, 0}, {0, 128, 0}, {0, 0, 128}, {128, 128, 0}, {0, 128, 128}, {128, 0, 128},
    {192, 0, 0}, {0, 192, 0}, {0, 0, 192}, {192, 192, 0}, {0, 192, 192}, {192, 0, 192},
    {64, 0, 0}, {0, 64, 0}, {0, 0, 64}, {64, 64, 0}, {0, 64, 64}, {64, 0, 64},
    {255, 128, 0}, {255, 0, 128}, {128, 255, 0}, {128, 0, 255}, {0, 255, 128}, {0, 128, 255},
    {255, 64, 0}, {255, 0, 64}, {64, 255, 0}, {64, 0, 255}, {0, 255, 64}, {0, 64, 255},
    {255, 192, 0}, {255, 0, 192}, {192, 255, 0}, {192, 0, 255}, {0, 255, 192}, {0, 192, 255},
    {128, 64, 0}, {128, 0, 64}, {64, 128, 0}, {64, 0, 128}, {0, 128, 64}, {0, 64, 128},
    {192, 64, 0}, {192, 0, 64}, {64, 192, 0}, {64, 0, 192}, {0, 192, 64}, {0, 64, 192},
    {128, 192, 0}, {128, 0, 192}, {192, 128, 0}, {192, 0, 128}, {0, 128, 192}, {0, 192, 128},
    {255, 255, 255}, {255, 255, 128}, {128, 255, 255}, {255, 128, 255}, {128, 128, 255}, {255, 128, 128},
    {128, 255, 128}, {255, 255, 0}, {0, 255, 255}, {255, 0, 255}, {255, 192, 128}, {192, 255, 255},
    {255, 192, 192}, {192, 192, 255}, {192, 128, 255}, {255, 128, 192}, {128, 255, 192},
    {255, 255, 192}, {192, 255, 128}, {128, 192, 255}, {255, 0, 128}, {0, 255, 255}, {255, 255, 128},
    {128, 255, 0}, {255, 0, 255}, {255, 255, 64}, {128, 255, 255}, {255, 64, 128}, {255, 128, 64},
    {255, 192, 128}, {128, 128, 192}, {192, 128, 255}, {192, 255, 64}, {128, 255, 255}, {64, 192, 128},
    {192, 192, 128}, {128, 255, 64}, {64, 255, 192}, {192, 64, 255}, {255, 64, 192}, {64, 255, 128},
    {128, 192, 128}, {255, 192, 64}, {192, 255, 128}, {64, 128, 255}, {255, 128, 192}, {128, 128, 64},
    {255, 255, 255}, {128, 255, 64}, {255, 64, 64}, {192, 128, 0}, {64, 192, 128}, {192, 0, 255}
};

// Union-Find data structure to track connected components
int uf[MAX_SIZE * MAX_SIZE];  // Parent array
int size[MAX_SIZE * MAX_SIZE];  // Size of each component

// Initialize Union-Find data structure
void uf_initialize(int n) {
    for (int i = 0; i < n; i++) {
        uf[i] = i;  // Each element is its own parent initially
        size[i] = 1;  // Each component starts with size 1
    }
}

// Find with path compression
int uf_find(int x) {
    while (uf[x] != x) {
        uf[x] = uf[uf[x]];  // Path compression: point directly to the grandparent
        x = uf[x];
    }
    return x;
}

// Union by size, attach smaller tree under the larger one
void uf_union(int x, int y) {
    int rootX = uf_find(x);
    int rootY = uf_find(y);

    if (rootX != rootY) {
        if (size[rootX] < size[rootY]) {
            uf[rootX] = rootY;  // Attach smaller tree to larger tree
            size[rootY] += size[rootX];
        } else {
            uf[rootY] = rootX;  // Attach smaller tree to larger tree
            size[rootX] += size[rootY];
        }
    }
}

// Hoshen-Kopelman algorithm for connected component labeling
void hoshen_kopelman(int **grid, int **labels, int n, int m) {
    uf_initialize(MAX_SIZE * MAX_SIZE);
    int label = 1;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (grid[i][j] == 1) {  // Only check for pixels with value 1
                int left = (j > 0 && grid[i][j - 1] == 1) ? labels[i][j - 1] : 0;
                int up = (i > 0 && grid[i - 1][j] == 1) ? labels[i - 1][j] : 0;

                if (left == 0 && up == 0) {
                    // New label (no neighbors)
                    labels[i][j] = label++;
                } else if (left != 0 && up == 0) {
                    labels[i][j] = left;
                } else if (left == 0 && up != 0) {
                    labels[i][j] = up;
                } else {
                    labels[i][j] = left;
                    uf_union(left, up);
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (labels[i][j] > 0) {
                labels[i][j] = uf_find(labels[i][j]);
            }
        }
    }
}

// Read PPM file in P1 format
void readPPM_P1(const char *filename, int ***grid, int *n, int *m) {
    FILE *inputFile = fopen(filename, "r");
    if (!inputFile) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    char format[3];
    fscanf(inputFile, "%s", format);
    if (strcmp(format, "P1") != 0) {
        fprintf(stderr, "Unsupported file format. Expecting P1 format.\n");
        exit(EXIT_FAILURE);
    }

    fscanf(inputFile, "%d %d", m, n);
    if (*n > MAX_SIZE || *m > MAX_SIZE || *n < 1 || *m < 1) {
        fprintf(stderr, "Grid dimensions out of allowed range [1, 1024]\n");
        exit(EXIT_FAILURE);
    }

    *grid = (int **)malloc(*n * sizeof(int *));
    for (int i = 0; i < *n; i++) {
        (*grid)[i] = (int *)malloc(*m * sizeof(int));
        for (int j = 0; j < *m; j++) {
            fscanf(inputFile, "%d", &(*grid)[i][j]);
        }
    }

    fclose(inputFile);
}

// Write labeled image to a PPM file in P3 format (color)
void writePPM_P3(const char *filename, int **labels, int n, int m) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fprintf(file, "P3\n%d %d\n255\n", m, n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (labels[i][j] == 0) {
                fprintf(file, "0 0 0 ");  // Background (black)
            } else {
                int colorIndex = (labels[i][j] - 1) % MAX_GROUPS;
                fprintf(file, "%d %d %d ", colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main() {
    int **grid = NULL, **labels = NULL;
    int n, m;

    readPPM_P1("input.ppm", &grid, &n, &m);

    labels = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        labels[i] = (int *)malloc(m * sizeof(int));
        memset(labels[i], 0, m * sizeof(int));  // Initialize all labels to 0
    }

    hoshen_kopelman(grid, labels, n, m);

    writePPM_P3("output.ppm", labels, n, m);

    for (int i = 0; i < n; i++) {
        free(grid[i]);
        free(labels[i]);
    }
    free(grid);
    free(labels);

    return EXIT_SUCCESS;
}
