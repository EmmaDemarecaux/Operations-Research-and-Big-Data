/*
The program is the implementation of PageRank using the power iteration method.

The program expects the following arguments:
- edgelist.txt that should contain the graph: one edge on each line (two unsigned long (nodes' ID) separated by a space);
- page_names.txt that should contain a mapping between a node ID and a name;
- degrees_out.txt for writing the degree out of each node;
- results.txt for writing the results of the PageRank algorithm.
 
To compile:
"gcc page_rank.c -O3 -o page_rank".

To execute:
"./page_rank graphs/tuto_graph.txt graphs/tuto_names.txt results/tuto_degrees_out.txt results/tuto_pagerank.txt".
*/


#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>//to estimate the runing time

#define ALPHA 0.15
#define NB_ITERATIONS 200
#define EPSILON 0.00000001
#define NB_RESULTS 5
#define NAME_LENGTH 230
#define NLINKS 100000000 //maximum number of edges for memory allocation, will increase if needed

typedef struct {
    unsigned long node;
    double score;
} page_rank;

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

// comparing 2 elements
static int compare_scores(void const *e1, void const *e2){
    page_rank const *element1 = e1;
    page_rank const *element2 = e2;
    if (element1->score - element2->score > 0)
        return -1;
    else if (element1->score - element2->score < 0)
        return 1;
    else
        return 0;
}

int main(int argc, char** argv){
    edgelist* g;
    time_t t1,t2;
    t1=time(NULL);
    printf("Reading edgelist from file %s\n", argv[1]);
    g=readedgelist(argv[1]);
    printf("Number of nodes: %lu\n",g->n);
    printf("Number of edges: %lu\n",g->e);
    // initialisation
    unsigned long  it = NB_ITERATIONS; // number of page rank iterations
    unsigned long i, node; // indexes
    unsigned long highest_node[NB_RESULTS], lowest_node[NB_RESULTS];
    double highest_score[NB_RESULTS], lowest_score[NB_RESULTS];
    char page_name[NAME_LENGTH], lowest_page_name[NB_RESULTS][NAME_LENGTH], highest_page_name[NB_RESULTS][NAME_LENGTH];
    unsigned long *degrees_out = calloc(g->n, sizeof(unsigned long));
    // initializing lower page name (it might be isolated nodes without names)
    for (i=0; i<NB_RESULTS; i++){
        strcpy(lowest_page_name[i], "NO PAGE NAME");
    }
    // computing nodes' degrees out
    degree_out(g, degrees_out, argv[3]);
    printf("Computing the degree out of each node: done.\n");
    // using power_iteration algorithm to compute PageRank
    double *P = power_iteration(g, degrees_out, &it); // page rank
    printf("Computing PageRank: done.\n");
    // sorting results by score
    page_rank *P_rank = malloc(g->n*sizeof(page_rank));
    for (i=0; i<g->n; i++){
        P_rank[i].node = i;
        P_rank[i].score = P[i];
    }
    qsort(P_rank, g->n, sizeof(page_rank), compare_scores);
    printf("Sorting score: done.\n");
    // finding the NB_RESULTS pages with the highest PageRank and the NB_RESULTS pages with the lowest PageRank
    for (i=0; i<NB_RESULTS; i++){
        highest_node[i] = P_rank[i].node;
        highest_score[i] = P_rank[i].score;
        lowest_node[i] = P_rank[g->n-1-i].node;
        lowest_score[i] = P_rank[g->n-1-i].score;
    }
    printf("Finding highest page ranks and lowest page ranks: done.\n");
    // reading the name of the pages corresponding to each node ID
    FILE *file = fopen(argv[2], "r");
    // finding the name of the NB_RESULTS pages with the highest PageRank and the NB_RESULTS pages with the lowest PageRank
    while (fscanf(file, "%lu %[^\n]", &(node), page_name)==2) {
        for (i=0; i<NB_RESULTS; i++){
            if (node == lowest_node[i]){
                strcpy(lowest_page_name[i], page_name);
            }
            if (node == highest_node[i]){
                strcpy(highest_page_name[i], page_name);
            }
        }
    }
    fclose(file);
    printf("Reading page names: done.\n");
    // printing results
    printf("\nPageRank results with alpha = %f:\n", ALPHA);
    printf("\nNumber of iterations necessary to reach convergence: %lu\n", it);
    printf("\nThe %i pages with the highest PageRank:\n", NB_RESULTS);
    for (i=0; i<NB_RESULTS; i++){
        printf("%lu: ", i+1);
        printf("Node %lu ", highest_node[i]);
        printf("with score %0.10f ", highest_score[i]);
        printf("and name %s\n", highest_page_name[i]);
    }
    printf("\nThe %i pages with the lowest PageRank:\n", NB_RESULTS);
    for (i=0; i<NB_RESULTS; i++){
        printf("%lu: ", i+1);
        printf("Node %lu ", lowest_node[i]);
        printf("with score %0.10f ", lowest_score[i]);
        printf("and name %s\n", lowest_page_name[i]);
    }
    // writing results in file
    FILE *f = fopen(argv[4], "w");
    fprintf(f, "PageRank results with alpha = %f:\n", ALPHA);
    fprintf(f, "\nNumber of iterations necessary to reach convergence: %lu\n", it);
    fprintf(f, "\nThe %i pages with the highest PageRank:\n", NB_RESULTS);
    for (i=0; i<NB_RESULTS; i++){
        fprintf(f, "%lu: ", i+1);
        fprintf(f, "Node %lu ", highest_node[i]);
        fprintf(f, "with score %0.10f ", highest_score[i]);
        fprintf(f, "and name %s\n", highest_page_name[i]);
    }
    fprintf(f, "\nThe %i pages with the lowest PageRank:\n", NB_RESULTS);
    for (i=0; i<NB_RESULTS; i++){
        fprintf(f, "%lu: ", i+1);
        fprintf(f, "Node %lu ", lowest_node[i]);
        fprintf(f, "with score %0.10f ", lowest_score[i]);
        fprintf(f, "and name %s\n", lowest_page_name[i]);
    }
    fprintf(f, "\nFull PageRank results:\n");
    for (i=0; i<g->n; i++){
        fprintf(f, "%lu %0.15f\n", i, P[i]);
    }
    fclose(f);
    free(degrees_out);
    free(P);
    free(P_rank);
    free_edgelist(g);
    t2=time(NULL);
    printf("\n- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    return 0;
}
