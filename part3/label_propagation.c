/*
The program is the implementation of PageRank using the power iteration method.

The program expects the following arguments:
- edgelist.txt that should contain the graph: one edge on each line (two unsigned long (nodes' ID) separated by a space;
- degrees.txt for writing the degree of each node;
- results.txt for writing the results of the PageRank algorithm.
 
To compile:
"gcc k-core.c -O3 -o k-core".

To execute:
"./k-core graphs/tuto_graph.txt results/tuto_degrees.txt results/tuto_k-core.txt".
*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h> // to estimate the runing time
#include <limits.h>

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

// A utility function to swap array elements
void swap (unsigned long *a, unsigned long *b){
    unsigned long temp = *a;
    *a = *b;
    *b = temp;
}

// A function to generate a random permutation of arr[]
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

// A utility function to print an array
void printArray(unsigned long *arr, unsigned long n){
    for (unsigned long i = 0; i < n; i++)
        printf("%lu ", arr[i]);
    printf("\n");
}

unsigned long most_frequent(unsigned long *neigbors_labels, unsigned long size, unsigned long n, unsigned long curr_label){
    // find the max frequency using linear traversal
    unsigned long i, index_res, res;
    unsigned long index = 0, to_keep = 0, max_frequence = 0;
    unsigned long *freq = calloc(n, sizeof(unsigned long));
    unsigned long *max_freq = malloc(size*sizeof(unsigned long));
    for (i=0; i<size; i++){
        freq[neigbors_labels[i]]++;
    }
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
    for (i=0; i<index; i++){
        if (max_freq[i] == curr_label){
            to_keep = 1;
            res = curr_label;
        }
    }
    if (to_keep == 0){
        index_res = rand() % index;
        res = max_freq[index_res];
    }
    return res;
}

void label_prop(adjlist* g, unsigned long *nodes, unsigned long *labels){
    // initialisation
    unsigned long i, j, v, index, node, size, node_label, new_label;
    unsigned long c = 0, degree_max = 0, counter = 0;
    unsigned long *neigbors_labels = malloc((g->n-1)*sizeof(unsigned long));
    
    while (counter < g->n){
        randomize(nodes, g->n);
        counter = 0;
        for (i=0; i<g->n; i++){
            node = nodes[i];
            size = g->cd[node+1]-g->cd[node];
            node_label = labels[node];
            if (size == 0){
                counter++;
            }
            else if (size == 1){
                v = g->adj[g->cd[node]];
                if (labels[v] == node_label)
                    counter++;
                else
                    labels[node] = labels[v];
            }
            else{
                index = 0;
                for (j=g->cd[node]; j<g->cd[node+1]; j++){
                    v = g->adj[j];
                    neigbors_labels[index] = labels[v];
                    index++;
                }
                new_label = most_frequent(neigbors_labels, size, g->n, node_label);
                if (new_label == node_label){
                    counter++;
                }
                else
                    labels[node] = new_label;
            }
        }
    }
}

int main(int argc, char** argv){
    // using the adjlist structure
    adjlist* g;
    time_t t1,t2;
    t1=time(NULL);
    printf("Reading edgelist from file %s\n",argv[1]);
    g=readedgelist(argv[1]);
    printf("Number of nodes: %lu\n",g->n);
    printf("Number of edges: %lu\n",g->e);
    printf("Building the adjacency list\n");
    mkadjlist(g);
    
    unsigned long *labels = malloc(g->n*sizeof(unsigned long));
    unsigned long *nodes = malloc(g->n*sizeof(unsigned long));
    unsigned long node = 0, max_print = 100;
    if (g->n < max_print)
        max_print = g->n;
    for (node=0; node<g->n; node++){
        labels[node] = node;
        nodes[node] = node;
    }
    printf("Initial labels\n");
    printArray(labels, max_print);
    label_prop(g, nodes, labels);
    FILE *f = fopen(argv[2], "w");
    for (node=0; node<g->n; node++){
        fprintf(f,"%lu %lu\n", node, labels[node]);
    }
    fclose(f);
    printf("Final labels\n");
    printArray(labels, max_print);
    free_adjlist(g);
    t2=time(NULL);
    printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    return 0;
}
