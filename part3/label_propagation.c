/*
The program is the implementation of the label propagation algorithm.

The program expects the following arguments:
- edgelist.txt that should contain the graph: one edge on each line (two unsigned long (nodes' ID) separated by a space);
- results.txt for writing the resulting partition: one node and its community id on each line.

To compile:
"gcc label_propagation.c -O3 -o label_propagation".

To execute:
"./label_propagation graphs/random_n400_c4_pq9_p0.9_q0.1.txt results/random_n400_c4_pq9_communities.txt".
*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h> // to estimate the runing time
#include <limits.h>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>

#define NLINKS 100000000 // maximum number of edges for memory allocation, will increase if needed

typedef struct {
    unsigned long s;
    unsigned long t;
} edge;

// edge list structure:
typedef struct {
    unsigned long n; // number of nodes
    unsigned long e; // number of edges
    edge *edges; // list of edges
    unsigned long *cd; // cumulative degree cd[0]=0 length=n+1
    unsigned long *adj; // concatenated lists of neighbors of all nodes
} adjlist;

// compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
    a=(a>b) ? a : b;
    return (a>c) ? a : c;
}

// reading the edgelist from file
adjlist* readedgelist(char* input){
    unsigned long e1=NLINKS;
    FILE *file=fopen(input,"r");
    adjlist *g=malloc(sizeof(adjlist));
    g->n=0;
    g->e=0;
    g->edges=malloc(e1*sizeof(edge));//allocate some RAM to store edges
    while (fscanf(file,"%lu %lu", &(g->edges[g->e].s), &(g->edges[g->e].t))==2) {
        g->n=max3(g->n,g->edges[g->e].s,g->edges[g->e].t);
        if (++(g->e)==e1) {//increase allocated RAM if needed
            e1+=NLINKS;
            g->edges=realloc(g->edges,e1*sizeof(edge));
        }
    }
    fclose(file);
    g->n++;
    g->edges=realloc(g->edges,g->e*sizeof(edge));
    return g;
}

// building the adjacency matrix
void mkadjlist(adjlist* g){
    unsigned long i,u,v;
    unsigned long *d=calloc(g->n,sizeof(unsigned long));
    for (i=0;i<g->e;i++) {
        d[g->edges[i].s]++;
        d[g->edges[i].t]++;
    }
    g->cd=malloc((g->n+1)*sizeof(unsigned long));
    g->cd[0]=0;
    for (i=1;i<g->n+1;i++) {
        g->cd[i]=g->cd[i-1]+d[i-1];
        d[i-1]=0;
    }
    g->adj=malloc(2*g->e*sizeof(unsigned long));
    for (i=0;i<g->e;i++) {
        u=g->edges[i].s;
        v=g->edges[i].t;
        g->adj[ g->cd[u] + d[u]++ ]=v;
        g->adj[ g->cd[v] + d[v]++ ]=u;
    }
    free(d);
}

// freeing memory
void free_adjlist(adjlist *g){
    free(g->edges);
    free(g->cd);
    free(g->adj);
    free(g);
}

// a utility function to swap array elements
void swap (unsigned long *a, unsigned long *b){
    unsigned long temp = *a;
    *a = *b;
    *b = temp;
}

// a function to generate a random permutation of arr[]
void randomize(unsigned long *arr, unsigned long n){
    // Use a different seed value so that we don't get same
    // result each time we run this program
    srand (time(NULL));
    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (unsigned long i = n-1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        unsigned long j = rand() % (i+1);
        // Swap arr[i] with the element at random index
        swap(&arr[i], &arr[j]);
    }
}

// a utility function to print an array
void print_array(unsigned long *arr, unsigned long n){
    for (unsigned long i = 0; i < n; i++)
        printf("%lu ", arr[i]);
    printf("\n");
}

// find the max frequency using linear traversal
unsigned long most_frequent(unsigned long *neigbors_labels, unsigned long size, unsigned long n, unsigned long curr_label){
    unsigned long i, index_res, res;
    unsigned long index = 0, to_keep = 0, max_frequence = 0;
    unsigned long *freq = calloc(n, sizeof(unsigned long));
    unsigned long *max_freq = malloc(size*sizeof(unsigned long));
    // computing label frequencies
    for (i=0; i<size; i++){
        freq[neigbors_labels[i]]++;
    }
    // finding labels with the highest label frequency
    for (i=0; i<n; i++){
        if (freq[i] > 0){
            if (freq[i] > max_frequence){
                index = 0;
                max_frequence = freq[i];
                max_freq[index] = i;
                index++;
            }
            else if (freq[i] == max_frequence){
                max_freq[index] = i;
                index++;
            }
        }
    }
    // looping over the label with the highest frequency
    for (i=0; i<index; i++){
        if (max_freq[i] == curr_label){ // if current label of the node has the highest frequency
            to_keep = 1; // keeping the current label
            res = curr_label;
        }
    }
    if (to_keep == 0){ // if current label of the node does not have the highest frequency
        index_res = rand() % index; // pick a label at random among the one with the highest frequency
        res = max_freq[index_res];
    }
    free(freq);
    free(max_freq);
    return res;
}

// label propagation Algorithm
void label_prop(adjlist* g, unsigned long *nodes, unsigned long *labels){
    // initialisation
    unsigned long i, j, u, v, index, size, label, new_label;
    unsigned long c = 0, degree_max = 0, counter = 0;
    unsigned long *neigbors_labels = malloc((g->n-1)*sizeof(unsigned long));
    while (counter < g->n){ // check if there exists a node with a label that does not have the highest frequency among its neighbours.
        // shuffling nodes
        randomize(nodes, g->n);
        // initializing number of nodes with a label that is the highest frequency among its neighbours
        counter = 0;
        // looping over the nodes in the random order
        for (i=0; i<g->n; i++){
            // node
            u = nodes[i];
            // degree of the node
            size = g->cd[u+1]-g->cd[u];
            // label of the node
            label = labels[u];
            if (size == 0){ // if 0 neighbor
                counter++; // the node has a label that is the highest frequency among its neighbours
            }
            else if (size == 1){ // if only 1 neighbor
                v = g->adj[g->cd[u]]; // v neighbor of u
                if (labels[v] == label) // if they already have the same label, do nothing and increment the counter
                    counter++;
                else
                    labels[u] = labels[v]; // otherwise, set the label of the node to its neighbor's
            }
            else{ // if the node has more than 1 neighbors
                index = 0;
                // looping over u's neighbors to collect the labels in "neigbors_labels"
                for (j=g->cd[u]; j<g->cd[u+1]; j++){
                    v = g->adj[j];
                    neigbors_labels[index] = labels[v];
                    index++;
                }
                // finding the most frequent label (the case of equality is handled in the function "most_frequent")
                new_label = most_frequent(neigbors_labels, size, g->n, label);
                if (new_label == label){ // if they already have the same label, do nothing and increment the counter
                    counter++;
                }
                else
                    labels[u] = new_label; // otherwise, set the label to the most frequent label
            }
        }
    }
    free(neigbors_labels);
}

int main(int argc, char** argv){
    // using the adjlist structure
    adjlist* g;
    time_t t1,t2;
    struct rusage r_usage;
    int ret;
    t1=time(NULL);
    printf("Reading edgelist from file %s\n",argv[1]);
    g=readedgelist(argv[1]);
    printf("Number of nodes: %lu\n",g->n);
    printf("Number of edges: %lu\n",g->e);
    printf("Building the adjacency list\n");
    mkadjlist(g);
    // allocating memory
    unsigned long *labels = malloc(g->n*sizeof(unsigned long));
    unsigned long *nodes = malloc(g->n*sizeof(unsigned long));
    unsigned long node = 0, max_print = 400;
    if (g->n < max_print)
        max_print = g->n;
    // giving a unique label to each node in the network
    for (node=0; node<g->n; node++){
        labels[node] = node;
        nodes[node] = node;
    }
    // printing initial labels
    printf("Initial labels\n");
    print_array(labels, max_print);
    // doing label propagation algorithm
    label_prop(g, nodes, labels);
    // writing results
    FILE *f = fopen(argv[2], "w");
    for (node=0; node<g->n; node++){
        fprintf(f,"%lu %lu\n", node, labels[node]);
    }
    fclose(f);
    // printing final labels
    printf("Final labels\n");
    print_array(labels, max_print);
    // freeing memory
    free(labels);
    free(nodes);
    free_adjlist(g);
    // time
    t2=time(NULL);
    printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    // memory consumption
    ret = getrusage(RUSAGE_SELF,&r_usage);
    if (ret == 0)
        printf("- Memory usage = %ld kilobytes\n", r_usage.ru_maxrss);
    return 0;
}
