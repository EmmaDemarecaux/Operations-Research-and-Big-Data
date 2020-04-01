/*
 The program will load the graph in main memory and compute a good lower bound to the diameter of a graph. The program expects an argument `edgelist.txt` that should contain the graph: one edge on each line (two unsigned long (nodes' ID) separated by a space).
 
To compile:
"gcc diameter.c -O3 -o diameter".

To execute:
"./diameter graphs/edgelist.txt".
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h> // to estimate the runing time

#define NLINKS 100000000 // maximum number of edges for memory allocation, will increase if needed
#define NITER 20

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
unsigned long bfs(adjlist *g, unsigned long s, unsigned long *diameter){
    // creating a FIFO to add and pop nodes easily
    unsigned long *fifo = malloc(g->n * sizeof(unsigned long));
    // to mark a node once it is visited
    unsigned int *marker = calloc(g->n, sizeof(unsigned long));
    // creating a array to store node distance from the source node
    unsigned long *distances = calloc(g->n, sizeof(unsigned long));
    // initializing the beginning and the end indexes of the FIFO to 0, and the farthest node from s to s
    unsigned long b = 0, e = 0, w = s;
    // initializing index i for the for loop and two nodes u and v
    unsigned long i, u, v;
    // adding the source node to the FIFO and incrementing the end index of the FIFO
    fifo[e++] = s;
    // marking the source node
    marker[s] = 1;
    while (b != e) { // FIFO not empty
        // popping the first node of the FIFO and incrementing the start index of the FIFO
        u = fifo[b++];
        for (i = g->cd[u]; i < g->cd[u+1]; i++){ // Loop on u's neighbors
            // v is a neighbor of u
            v = g->adj[i];
            if (marker[v] == 0) { // v not marked
                // adding the node v to the FIFO and incrementing the end index of the FIFO
                fifo[e++] = v;
                // marking the node v
                marker[v] = 1;
                // updating the distance from v to s
                distances[v] = distances[u] + 1;
                // updating the max distance from s and the farthest node
                if (*diameter < distances[v]){
                    *diameter = distances[v];
                    w = v;
                }
            }
        }
    }
    free(fifo);
    free(marker);
    free(distances);
    return w;
}

// computing a good lower bound to the diameter of a graph
unsigned long lower_bound_diameter(adjlist *g){
    // initializing the diameter, the source node and an index
    unsigned long diameter, s, i;
    unsigned long nodes[NITER];
    // initializing previous diameter and an index
    // setting the source node to the first random node which has at least one neighbor
    s = rand() % (g->n);
    while (g->cd[s+1] == g->cd[s])
        s = rand() % (g->n);
    for (i=0; i<NITER; i++){
        nodes[i] = s;
        // initializing the maximum distance between s and the other nodes to 0
        diameter = 0;
        // bfs
        printf("    -- distance between nodes %lu ", s);
        s = bfs(g, s, &diameter);
        printf("and %lu = %lu\n", s, diameter);
        if (i > 0){
            if (s == nodes[i-1])
                return diameter;
        }
    }
    return diameter;
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
    // computing a good lower bound to the diameter of a graph
    printf("Diameter algorithm:\n");
    printf("A lower bound to the diameter of a graph is %lu\n", lower_bound_diameter(g));
    free_adjlist(g);
    t2=time(NULL);
    printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    return 0;
}
