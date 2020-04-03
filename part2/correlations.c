/*
The program is the implementation of PageRank using the power iteration method.

The program expects the following arguments:
- edgelist.txt that should contain the graph: one edge on each line (two unsigned long (nodes' ID) separated by a space;
- degrees_out.txt for writing the degree out of each node;
- degrees_in.txt for writing the degree in of each node;
- results.txt for writing the results of the PageRank algorithm.
 
To compile:
"gcc correlations.c -O3 -o correlations".

To execute:
"./correlations graphs/tuto_graph.txt results/tuto_degrees_out.txt results/tuto_degrees_in.txt results/tuto_correlations_0_15.txt".
*/


#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>//to estimate the runing time

#define ALPHA 0.15
#define NB_ITERATIONS 200
#define EPSILON 0.00000001
#define NLINKS 100000000 //maximum number of edges for memory allocation, will increase if needed

typedef struct {
    unsigned long s;
    unsigned long t;
} edge;

//edge list structure:
typedef struct {
    unsigned long n;//number of nodes
    unsigned long e;//number of edges
    unsigned long *nodes;// list of nodes
    edge *edges;//list of edges
} edgelist;

//compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
    a=(a>b) ? a : b;
    return (a>c) ? a : c;
}

//reading the edgelist from file
edgelist* readedgelist(char* input){
    unsigned long e1=NLINKS;
    FILE *file=fopen(input,"r");
    edgelist *g=malloc(sizeof(edgelist));
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

void free_edgelist(edgelist *g){
    free(g->edges);
    free(g);
}

void degree_in(edgelist* g, unsigned long *degrees_in, char *output){
    unsigned long l, i, v;
    for (l=0; l<g->e; l++){
        v = g->edges[l].t;
        degrees_in[v]++;
    }
    // writing results
    FILE *f = fopen(output, "w");
    for (i=0; i<g->n; i++){
        fprintf(f,"%lu %lu\n", i, degrees_in[i]);
    }
    fclose(f);
}

void degree_out(edgelist* g, unsigned long *degrees_out, char *output){
    unsigned long l, i, u;
    for (l=0; l<g->e; l++){
        u = g->edges[l].s;
        degrees_out[u]++;
    }
    // writing results
    FILE *f = fopen(output, "w");
    for (i=0; i<g->n; i++){
        fprintf(f,"%lu %lu\n", i, degrees_out[i]);
    }
    fclose(f);
}

double* mat_vect_prod(edgelist* g, unsigned long *degrees_out, double *P){
    unsigned long l, degree, u, v;
    double *mat_vect_prod = calloc(g->n, sizeof(double));
    for (l=0; l<g->e; l++){
        u = g->edges[l].s;
        v = g->edges[l].t;
        mat_vect_prod[v] += ((double)P[u])/((double)degrees_out[u]);
    }
    double dead_ends_additions = 0;
    for (u=0; u<g->n; u++){
        if (degrees_out[u] == 0){
            dead_ends_additions += ((double)P[u])/((double)g->n);
        }
    }
    for (v=0; v<g->n; v++){
        mat_vect_prod[v] += dead_ends_additions;
    }
    return mat_vect_prod;
}

double absolute(double element){
    double abs_element;
    if (element < 0){
        abs_element = -element;
    }
    else{
        abs_element = element;
    }
    return abs_element;
}

double* power_iteration(edgelist* g, unsigned long *degrees_out, unsigned long *t){
    unsigned long i, it;
    double norm_1, cvg;
    double *I = malloc(g->n*sizeof(double));
    double *P = malloc(g->n*sizeof(double));
    double *P_prev = malloc(g->n*sizeof(double));
    for (i=0; i<g->n; i++){
        I[i] = 1./g->n;
        P[i] = 1./g->n;
    }
    // initializing previous P
    P_prev = P;
    // t iterations
    for (it=0; it<*t; it++){
        // initializing norm 1
        norm_1 = 0.;
        // to measure convergence
        cvg = 0.0;
        // updating P
        P = mat_vect_prod(g, degrees_out, P);
        for (i=0; i<g->n; i++){
            P[i] = (1-ALPHA) * P[i] + ALPHA * I[i];
            // updating norm 1
            norm_1 += P[i];
        }
        // normalisation
        for (i=0; i<g->n; i++){
            P[i] += (1.-norm_1) / g->n;
            cvg += absolute(P[i] - P_prev[i]);
        }
        // convergence test
        if (cvg < EPSILON){
            *t = it;
            return P;
        }
        P_prev = P;
    }
    free(I);
    free(P_prev);
    return P;
}

int main(int argc, char** argv){
    edgelist* g;
    time_t t1,t2;
    t1=time(NULL);
    printf("Reading edgelist from file %s\n", argv[1]);
    g=readedgelist(argv[1]);
    printf("Number of nodes: %lu\n",g->n);
    printf("Number of edges: %lu\n",g->e);
    // initilaisation
    unsigned long  it = NB_ITERATIONS; // number of page rank iterations
    unsigned long i; // index
    // nodes' degrees out
    unsigned long *degrees_out = calloc(g->n, sizeof(unsigned long));
    degree_out(g, degrees_out, argv[2]);
    printf("Computing the degree out of each node: done.\n");
    // nodes' degrees in
    unsigned long *degrees_in = calloc(g->n, sizeof(unsigned long));
    degree_in(g, degrees_in, argv[3]);
    printf("Computing the degree in of each node: done.\n");
    // PageRank
    double *P = power_iteration(g, degrees_out, &it); // page rank
    printf("Computing PageRank: done.\n");
    printf("Number of iterations necessary to reach convergence: %lu\n", it);
    // writing results in file
    FILE *f = fopen(argv[4], "w");
    fprintf(f, "PageRank results with alpha = %f:\n", ALPHA);
    for (i=0; i<g->n; i++){
        fprintf(f, "%lu %0.15f\n", i, P[i]);
    }
    fclose(f);
    free(degrees_out);
    free(degrees_in);
    free(P);
    free_edgelist(g);
    t2=time(NULL);
    printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    return 0;
}
