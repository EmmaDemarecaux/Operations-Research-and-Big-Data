/*
 The program will load the graph in main memory and return the number of connected components as well as the fraction of nodes in the largest component. The program expects an argument `edgelist.txt` that should contain the graph: one edge on each line (two unsigned long (nodes' ID) separated by a space); and an argument `results.txt` for writing the results.
 
To compile:
"gcc connected_components.c -O3 -o connected_components".

To execute:
"./connected_components graphs/edgelist.txt results/edgelist_cc.txt".
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h> // to estimate the runing time

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
    // free(g->edges);
}

// freeing memory
void free_adjlist(adjlist *g){
    free(g->edges);
    free(g->cd);
    free(g->adj);
    free(g);
}

// BFS algorithm
unsigned long bfs(adjlist *g, unsigned int *marker, unsigned long s){
    // creating a FIFO to add and pop nodes easily
    unsigned long *fifo = malloc(g->n * sizeof(unsigned long));
    // initializing the number of nodes in the connected component to 0, the beginning and the end indexes of the FIFO to 0
    unsigned long counter = 0, b = 0, e = 0;
    // initializing index i for the for loop and two nodes u and v
    unsigned long i, u, v;
    // adding the source node to the FIFO and incrementing the end index of the FIFO
    fifo[e++] = s;
    // marking the source node
    marker[s] = 1;
    while (b != e) { // FIFO not empty
        // popping the first node of the FIFO and incrementing the start index of the FIFO
        u = fifo[b++];
        // incrementing the number of nodes in the connected component
        counter ++;
        for (i = g->cd[u]; i < g->cd[u+1]; i++){ // Loop on u's neighbors
            // v is a neighbor of u
            v = g->adj[i];
            if (marker[v] == 0) { // v not marked
                // adding the node v to the FIFO and incrementing the end index of the FIFO
                fifo[e++] = v;
                // marking the node v
                marker[v] = 1;
            }
        }
    }
    free(fifo);
    return counter;
}

// computing the number of connected components as well as the fraction of nodes in the largest connected component
void number_connected_components(adjlist *g, char *output){
    // to mark a node once it is visited
    unsigned int *marker = calloc(g->n, sizeof(unsigned long));
    // initialisation of the number of connected components and of the size of the largest connected component
    unsigned long number_connected_components = 0, max_size_component = 0;
    // initialisation of the source node and of the size of each component
    unsigned long s, size_component;
    for (s = 1; s < g->n; s++){
        if (marker[s] == 0){ // s not marked
            // incrementing the number of connected components
            number_connected_components++;
            // using BFS to compute the size of the connected component containing the node s
            size_component = bfs(g, marker, s);
            // setting the size of the largest connected component to the current size it the latter is larger that the former
            if (max_size_component < size_component){
                max_size_component = size_component;
            }
        }
    }
    // computing the fraction of nodes in the largest connected component
    float fraction_main_component = (float)max_size_component/((float)(g->n));
    // displaying results
    printf("Number of connected components: %lu\n", number_connected_components);
    printf("Fraction of nodes in the largest connected component: %lu/%lu = %f\n", max_size_component, g->n, fraction_main_component);
    // writing results in file
    printf("Writing in file %s\n", output);
    FILE *f = fopen(output, "w");
    fprintf(f,"Number of connected components: %lu\n", number_connected_components);
    fprintf(f,"Fraction of nodes in the largest connected component: %lu/%lu = %f\n", max_size_component, g->n, fraction_main_component);
    fclose(f);
    free(marker);
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
    // computing the number of connected components as well as the fraction of nodes in the largest connected component
    number_connected_components(g, argv[2]);
    free_adjlist(g);
    t2=time(NULL);
    printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    return 0;
}
