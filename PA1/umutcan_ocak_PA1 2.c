#include <stdio.h>
#include <math.h>

#define X 10  /* Threshold for discarding points based on distance difference percentage */
#define N 5   /* Number of points to be entered */

/* Function to calculate Euclidean distance between two points (x1, y1) and (x2, y2) */
float calculate_distance(float x1, float y1, float x2, float y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

/* 
   Function to check if a point should be discarded based on the distance difference.
   If the relative difference between distances is smaller than X, the point will be discarded.
*/
int check_discard(float d1, float d2, float d3) {
    if ((fabs(d2 - d1) / ((d1 + d2) / 2)) * 100 < X || 
        (fabs(d3 - d1) / ((d1 + d3) / 2)) * 100 < X) {
        return 1;  /* Point should be discarded */
    }
    return 0;  /* Point should not be discarded */
}

int main() {
    float x1, x2, x3, y1, y2, y3;  /* Cluster center coordinates */
    float x, y;  /* Input point coordinates */
    float distance1, distance2, distance3;  /* Distances from point to each cluster center */
    int number_of_points1 = 1, number_of_points2 = 1, number_of_points3 = 1;  /* Number of points in each cluster */

    /* Read the coordinates of the three cluster centers */
    scanf("%f %f/n", &x1, &y1);
    scanf("%f %f/n", &x2, &y2);
    scanf("%f %f", &x3, &y3);

    /* Iterate over the number of points to be processed */
    for (int i = 0; i < N; i++) {
        printf("Enter coordinates: ");
        scanf("%f %f", &x, &y);

        /* Calculate distances from the point to each cluster center */
        distance1 = calculate_distance(x1, y1, x, y);
        distance2 = calculate_distance(x2, y2, x, y);
        distance3 = calculate_distance(x3, y3, x, y);

        /* Determine the closest cluster and check if the point should be discarded */
        if (distance1 <= distance2 && distance1 <= distance3) {
            if (check_discard(distance1, distance2, distance3)) {
                printf("Point %f, %f was discarded\n", x, y);
            } else {
                printf("Point %f, %f was included in Cluster 1\n", x, y);
                number_of_points1++;
                x1 = (x1 * number_of_points1 + x) / (number_of_points1 + 1);  /* Update cluster 1 center */
                y1 = (y1 * number_of_points1 + y) / (number_of_points1 + 1);
            }
        } 
        else if (distance2 <= distance1 && distance2 <= distance3) {
            if (check_discard(distance2, distance1, distance3)) {
                printf("Point %f, %f was discarded\n", x, y);
            } else {
                printf("Point %f, %f was included in Cluster 2\n", x, y);
                number_of_points2++;
                x2 = (x2 * number_of_points2 + x) / (number_of_points2 + 1);  /* Update cluster 2 center */
                y2 = (y2 * number_of_points2 + y) / (number_of_points2 + 1);
            }
        } 
        else if (distance3 <= distance1 && distance3 <= distance2) {
            if (check_discard(distance3, distance1, distance2)) {
                printf("Point %f, %f was discarded\n", x, y);
            } else {
                printf("Point %f, %f was included in Cluster 3\n", x, y);
                number_of_points3++;
                x3 = (x3 * number_of_points3 + x) / (number_of_points3 + 1);  /* Update cluster 3 center */
                y3 = (y3 * number_of_points3 + y) / (number_of_points3 + 1);
            }
        }
    }

    /* Output final cluster centers and number of points in each cluster */
    printf("\nCluster 1: %f %f %d\n", x1, y1, number_of_points1);
    printf("Cluster 2: %f %f %d\n", x2, y2, number_of_points2);
    printf("Cluster 3: %f %f %d\n", x3, y3, number_of_points3);
    printf("\n--------------------------------");
    printf("\nExiting...\n");

    return 0;
}
