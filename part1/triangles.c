/*
 The program will load the graph in main memory and return the number of triangles. The program expects an argument `edgelist.txt` that should contain the graph: one edge on each line (two unsigned long (nodes' ID) separated by a space); and an argument `results.txt` for writing the results.
 
To compile:
"gcc triangles.c -O3 -o triangles".

To execute:
"./triangles graphs/tuto_graph.txt results/tuto_triangles.txt".
"edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
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

// comparing 2 elements
static int compare_elements(const void *e1, const void *e2){
    const unsigned long element1 = *(const unsigned long* const)e1;
    const unsigned long element2 = *(const unsigned long* const)e2;
    return element1 - element2;
}

// sorting the list of neighbors of each node
void sort_neighbors_list(adjlist* g){
    // initialisation of the size of a list of neigbors and the node index
    unsigned long neighbors_list_size, i;
    // address of a list of neighbors
    unsigned long *neighbors_list_pointer;
    for (unsigned long i=0; i<(g->n); i++){
        // address of the first element of the list of neighbors of node i
        neighbors_list_pointer = g->adj + (g->cd[i]);
        // size of the list of neighbors of node i
        neighbors_list_size = g->cd[i+1] - g->cd[i];
        // sorting the list of neighbors of node i
        qsort(neighbors_list_pointer, neighbors_list_size, sizeof(unsigned long), compare_elements);
    }
}

void triangles(adjlist* g, char *output){
    unsigned long u, v;
    unsigned long w1, w2; // neighbors of u and v
    unsigned long j, i1, i2; // indexes
    // initialisation of the number of triangles
    unsigned long number_triangles = 0;
    // for each u in G
    for(u=0; u<g->n; u++){
        // for each edge (u,v) in G
        // the list of neighbors of u is truncated and sorted
        for (j = g->cd[u]; j < g->cd[u+1]; j++){
            v = g->adj[j];
            if(u < v){
                // computing the intersection of the lists of neighbors of u and v
                // taking the first neighbor of u
                i1 = g->cd[u];
                w1 = g->adj[i1];
                // taking the first neighbor of v
                i2 = g->cd[v];
                w2 = g->adj[i2];
                while((i1 < g->cd[u+1]) && (i2 < g->cd[v+1]) && (w2 < u) && (w1 < u)){
                    if(w1 > w2){
                        // going to v's next neighbor
                        i2++;
                        w2 = g->adj[i2];
                    }
                    else if(w1 < w2){
                        // going to u's next neighbor
                        i1++;
                        w1 = g->adj[i1];
                    }
                    else{
                        // w1 = w2: we found a common neighbor of u and v
                        // incrementing the number of triangles
                        number_triangles++;
                        // going to u's next neighbor
                        i1++;
                        w1 = g->adj[i1];
                        // going to v's next neighbor
                        i2++;
                        w2 = g->adj[i2];
                    }
                }
            }
        }
    }
    // displaying results
    printf("Number of triangles: %lu\n", number_triangles);
    // writing results in file
    printf("Writing in file %s\n", output);
    FILE *f = fopen(output, "w");
    fprintf(f,"Number of triangles: %lu\n", number_triangles);
    fclose(f);
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
    // sorting the list of neighbors of each node
    sort_neighbors_list(g);
    // computing the number of triangles
    triangles(g, argv[2]);
    free_adjlist(g);
    t2=time(NULL);
    printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    return 0;
}

