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

unsigned long* degree(adjlist *g, char *output){
    unsigned long *degrees = calloc(g->n, sizeof(unsigned long));
    unsigned long l, i;
    for (l=0; l<g->e; l++){
        degrees[g->edges[l].s]++;
        degrees[g->edges[l].t]++;
    }
    FILE *f = fopen(output, "w");
    for (i=0; i<g->n; i++){
        fprintf(f,"%lu %lu\n", i, degrees[i]);
    }
    fclose(f);
    return degrees;
}

void order_nodes_by_degree(adjlist* g, unsigned long *degrees, unsigned long *ordered_nodes, unsigned long *start_degrees, unsigned long *nodes_index, unsigned long degree_max){
    // initialisation
    unsigned long j, d, index, node;
    unsigned long i = 0;
    // allocate memory for arrays
    // -- ordered array of nodes by degree (one line per degree)
    unsigned long *big_ordered_nodes = malloc(degree_max*g->n*sizeof(unsigned long));
    // -- to keep track of the size of each degree line in big_ordered_nodes
    unsigned long *size_degrees = calloc(degree_max, sizeof(unsigned long));
    // constructing big_ordered_nodes and size_degrees tables
    for (node=0; node<g->n; node++){
        // degree
        d = degrees[node];
        // index in big_ordered_nodes
        index = d*g->n + size_degrees[d];
        big_ordered_nodes[index] = node;
        // increment size of the line of degree d
        size_degrees[d] ++;
    }
    // constructing ordered_nodes, nodes_index and start_degrees
    for (d=0; d<degree_max; d++){
        start_degrees[d] = i;
        for (j=0; j<size_degrees[d]; j++){
            index = d*g->n + j;
            ordered_nodes[i] = big_ordered_nodes[index];
            nodes_index[big_ordered_nodes[index]] = i;
            i++;
        }
    }
    // freeing big_ordered_nodes and size_degrees tables
    free(big_ordered_nodes);
    free(size_degrees);
}

// A utility function to swap array elements
void swap(unsigned long *a, unsigned long *b){
    unsigned long temp = *a;
    *a = *b;
    *b = temp;
}

unsigned long core_decomposition(adjlist* g, unsigned long *degrees, char *output){
    // initialisation
    unsigned long i, j, a, b, v, node, node_degree, v_degree, visited_neighbor;
    unsigned long c = 0, degree_max = 0;
    // computing maximum degree
    for (i=0; i<g->n; i++){
        if (degrees[i] > degree_max){
            degree_max = degrees[i];
        }
    }
    degree_max++;
    // allocate memory for arrays
    // -- ordered list of nodes by degree asc
    unsigned long *ordered_nodes_by_degree = malloc(g->n*sizeof(unsigned long));
    // -- to keep track of location of indexes where each degree starts
    unsigned long *start_degrees_index = malloc(degree_max*sizeof(unsigned long));
    // -- to keep track of location of each node in ordered_nodes_by_degree
    unsigned long *nodes_index_ordering = malloc(g->n*sizeof(unsigned long));
    // -- to keep track of the core of each node
    unsigned long *nodes_core = calloc(g->n, sizeof(unsigned long));
    // ordering nodes by degree asc
    order_nodes_by_degree(g, degrees, ordered_nodes_by_degree, start_degrees_index, nodes_index_ordering, degree_max);
    // loop of the list of ordered nodes by degree
    for (i=0; i<g->n; i++){
        // node
        node = ordered_nodes_by_degree[i];
        // node degree
        node_degree = degrees[node];
        // initializing variable that detects if we visited at least 1 neighbor for this node
        visited_neighbor = 0;
        // updating max core value
        if (node_degree > c){
            c = node_degree;
        }
        // setting core value of the node
        nodes_core[node] = c;
        // setting node degree to inf so that it will not be considered anymore
        degrees[node] = ULONG_MAX;
        // removing node from graph
        if (node_degree > 0){
            // loop over its neighbors to decrease their degree
            for (j=g->cd[node]; j<g->cd[node+1]; j++){
                // v neighbor of node
                v = g->adj[j];
                // v degree
                v_degree = degrees[v];
                // considering only nodes with degree < inf
                if (v_degree != ULONG_MAX){
                    // decrement v degree as we remove node from the graph
                    degrees[v]--;
                    // swap node v in ordered_nodes_by_degree to "reorder" the list
                    if (i < start_degrees_index[v_degree]){
                        a = start_degrees_index[v_degree];
                        b = nodes_index_ordering[v];
                        swap(&ordered_nodes_by_degree[a], &ordered_nodes_by_degree[b]);
                        swap(&nodes_index_ordering[ordered_nodes_by_degree[a]],
                             &nodes_index_ordering[ordered_nodes_by_degree[b]]);
                        start_degrees_index[v_degree]++;
                        if (visited_neighbor != 1){
                            start_degrees_index[node_degree]++;
                        }
                    }
                    else{
                        a = i+1;
                        b = nodes_index_ordering[v];
                        swap(&ordered_nodes_by_degree[a], &ordered_nodes_by_degree[b]);
                        swap(&nodes_index_ordering[ordered_nodes_by_degree[a]],
                             &nodes_index_ordering[ordered_nodes_by_degree[b]]);
                        start_degrees_index[v_degree] = start_degrees_index[v_degree] + 2;
                        start_degrees_index[v_degree - 1] ++;
                    }
                    // set visited_neighbor to 1 as we just visited one neighbor of node
                    visited_neighbor = 1;
                }
            }
        }
    }
    // freeing memory
    free(ordered_nodes_by_degree);
    free(start_degrees_index);
    free(nodes_index_ordering);
    // writing results
    FILE *f = fopen(output, "w");
    fprintf(f,"Graph core value = %lu\n", c);
    for (i=0; i<g->n; i++){
        fprintf(f,"%lu %lu\n", i, nodes_core[i]);
    }
    fclose(f);
    return c;
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
    // computing degree of each node
    unsigned long *degrees = degree(g, argv[2]);
    // computing core value of each node
    unsigned long core_value = core_decomposition(g, degrees, argv[3]);
    printf("Graph core value = %lu\n", core_value);
    free(degrees);
    free_adjlist(g);
    t2=time(NULL);
    printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    return 0;
}
