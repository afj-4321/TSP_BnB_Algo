// C program to solve Traveling Salesman Problem
// using Branch and Bound.
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

// number of vertices in the graph
int N; 

// adjacency matrix for the graph where adj[i][j] gives edge weight between i and j
// adj[i][j] = 0 means no path between the i and j
int ** adj;

// final_path[] stores the final solution ie, the
// path of the salesman.
int * final_path;

// visited[] keeps track of the already visited nodes
// in a particular path
bool *visited;

// Stores the final minimum weight of shortest tour.
int final_res = INT_MAX;

// Function to copy temporary solution to
// the final solution
void copyToFinal(int curr_path[]){
	for (int i=0; i<N; i++)
		final_path[i] = curr_path[i];
	final_path[N] = curr_path[0];
}

// Function to find the minimum edge cost
// having an end at the vertex i
int firstMin(int i){
	int min = INT_MAX;
	for (int k=0; k<N; k++)
		if (adj[i][k]<min && i != k)
			min = adj[i][k];
	return min;
}

// function to find the second minimum edge cost
// having an end at the vertex i
int secondMin(int i){
	int first = INT_MAX, second = INT_MAX;
	for (int j=0; j<N; j++){
		if (i == j)
			continue;

		if (adj[i][j] <= first){
			second = first;
			first = adj[i][j];
		}
		else if (adj[i][j] <= second && adj[i][j] != first)
			second = adj[i][j];
	}
	return second;
}

// function that takes as arguments:
// curr_bound -> lower bound of the root node
// curr_weight-> stores the weight of the path so far
// level-> current level while moving in the search
//		 space tree
// curr_path[] -> where the solution is being stored which
//			 would later be copied to final_path[]
void TSPRec(int curr_bound, int curr_weight, int level, int curr_path[]){
	// base case is when we have reached level N which
	// means we have covered all the nodes once
	if (level==N){
		// check if there is an edge from last vertex in
		// path back to the first vertex
		if (adj[curr_path[level-1]][curr_path[0]] != 0){
			// curr_res has the total weight of the
			// solution we got
			int curr_res = curr_weight +
					adj[curr_path[level-1]][curr_path[0]];

			// Update final result and final path if
			// current result is better.
			if (curr_res < final_res){
				copyToFinal(curr_path);
				final_res = curr_res;
			}
		}
		return;
	}

	// for any other level iterate for all vertices to
	// build the search space tree recursively
	for (int i=0; i<N; i++){
		// Consider next vertex if it is not same (diagonal
		// entry in adjacency matrix and not visited
		// already)
		if (adj[curr_path[level-1]][i] != 0 && visited[i] == false){
			int temp = curr_bound;
			curr_weight += adj[curr_path[level-1]][i];

			// different computation of curr_bound for
			// level 2 from the other levels
			if (level==1)
				curr_bound -= ((secondMin(curr_path[level-1]) +
							secondMin(i))/2);
			else
				curr_bound -= ((firstMin(curr_path[level-1]) +
							secondMin(i))/2);

			// curr_bound + curr_weight is the actual lower bound
			// for the node that we have arrived on
			// If current lower bound < final_res, we need to explore
			// the node further
			if (curr_bound + curr_weight < final_res){
				curr_path[level] = i;
				visited[i] = true;

				// call TSPRec for the next level
				TSPRec(curr_bound, curr_weight, level+1, curr_path);
			}

			// Else we have to prune the node by resetting
			// all changes to curr_weight and curr_bound
			curr_weight -= adj[curr_path[level-1]][i];
			curr_bound = temp;

			// Also reset the visited array
			for(int j =0 ; j < N; j++)	visited[j] = false;
			for (int j=0; j<=level-1; j++)
				visited[curr_path[j]] = true;
		}
	}
}

// This function sets up final_path[]
void TSP()
{
	int curr_path[N+1];

	// Calculate initial lower bound for the root node
	// using the formula 1/2 * (sum of first min +
	// second min) for all edges.
	// Also initialize the curr_path and visited array
	int curr_bound = 0;
	memset(curr_path, -1, sizeof(curr_path));
	memset(visited, 0, sizeof(curr_path));

	// Compute initial bound
	for (int i=0; i<N; i++)
		curr_bound += (firstMin(i) + secondMin(i));

	// Rounding off the lower bound to an integer
	curr_bound = (curr_bound&1)? curr_bound/2 + 1 : curr_bound/2;

	// We start at vertex 1 so the first vertex
	// in curr_path[] is 0
	visited[0] = true;
	curr_path[0] = 0;

	// Call to TSPRec for curr_weight equal to
	// 0 and level 1
	TSPRec(curr_bound, 0, 1, curr_path);
}

// reads the input file with filename
// returns 1 if input is read successfully and input is correct
// returns 2 if input is not in correct format in inputfile
// returns 0 if inputfile is not present
int readInput(char filename[]){
	
	FILE * fp = fopen(filename, "r");
	// File present or not
	if(fp == NULL)	return 0;
	
	// Reading input
	// Allocating memory to adj matrix
	// Check for valid input
	if(fscanf(fp, "%d", &N) != 1)	return 2;
	
	adj = (int**)malloc(N * sizeof(int*));
	for(int i = 0; i < N; i++)	adj[i] = (int*)malloc(N * sizeof(int));
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(fscanf(fp, "%d", &adj[i][j]) != 1)	return 2;
			if(i == j && adj[i][j] != 0)	return 2;
		}
	}
	// if file has not reached EOF then invalid input in file
	int t;
	if(fscanf(fp, "%d", &t) != EOF)	return 2;	
	
	// allocate memry to final_path array and visited array if input is valid
	final_path = (int *)malloc((N + 1) * sizeof(int));
	visited = (bool *)malloc(N * sizeof(bool));
	
	// close file
	fclose(fp);
	return 1;
}

void printAdjMat(){
	printf("\nAdjacency matrix for the graph is : \n\n");
	printf("   |");
	for(int i = 0; i < N; i++)	printf("%4d", i);
	printf("\n");
	for(int i = 0; i < N + 1; i++)	printf("%s", "----");
	printf("\n");
	
	for(int i = 0; i < N; i++){
		printf("%3d|", i);
		for(int j = 0;j < N; j++){
			printf("%4d", adj[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void printTakenEdges(){
	for(int i = 0; i < N; i++){
		adj[final_path[i]][final_path[i + 1]] *= -1;
	}
	printf("\nIncluded edges in the TSP tour are marked in green : \n\n");
	printf("   |");
	for(int i = 0; i < N; i++)	printf("%4d", i);
	printf("\n");
	for(int i = 0; i < N + 1; i++)	printf("%s", "----");
	printf("\n");
	
	for(int i = 0; i < N; i++){
		printf("%3d|", i);
		for(int j = 0;j < N; j++){
			if(adj[i][j] < 0){
				adj[i][j] *= -1;
				printf (GREEN "%4d" RESET, adj[i][j]);
			}
			else printf("%4d", adj[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}


int main(){
	
	clock_t tic = clock();
	char inputfile[50];
	printf("Enter input filename : ");
	scanf("%s", inputfile);
	
	int ecode;
	if((ecode = readInput(inputfile)) == 1){
		
		printAdjMat();
		TSP();
		printf("Minimum cost : %d\n\n", final_res);
		printf("Path Taken : ");
		for (int i=0; i<=N; i++)
			printf(GREEN "%d" RESET RED " %s " RESET, final_path[i], (i == N) ? "" : "->");
		printf("\n");
		
		printTakenEdges();
	
	}
	else{
		if(ecode == 0)	printf(RED "No such file present... Exiting...\n" RESET);
		else{
			printf(RED "Invalid input in input file. Please check input file\n" RESET);
		}
	}
	clock_t toc = clock();
    printf("Time taken for program to run: %.f Microseconds\n\n", ((double)(toc - tic) / CLOCKS_PER_SEC)*1000000);
	return 0;
}