/*
Maximilien Danisch
September 2017
http://bit.ly/danisch
maximilien.danisch@gmail.com

Info:
Feel free to use these lines as you wish. This program loads a graph in main memory as a list of edges.

To compile:
"gcc ./LoadGraph/edgelist.c -O3 -o ./LoadGraph/edgelist".

To execute:
"./LoadGraph/edgelist graphs/edgelist.txt".
"edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
The prograph loads the graph in main memory and then it terminates.

Note:
If the graph is directed (and weighted) with selfloops and you want to make it undirected unweighted without selfloops, use the following linux command line.
awk '{if ($1<$2) print $1" "$2;else if ($2<$1) print $2" "$1}' net.txt | sort -n -k1,2 -u > net2.txt

Performance:
Up to 500 million edges on my laptop with 8G of RAM:
Takes more or less 1.6G of RAM and 25 seconds (I have an SSD hardrive) for 100M edges.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>//to estimate the runing time
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>

#define NLINKS 100000000 //maximum number of edges for memory allocation, will increase if needed

typedef struct {
	unsigned long s;
	unsigned long t;
} edge;

//edge list structure:
typedef struct {
	unsigned long n;//number of nodes
	unsigned long e;//number of edges
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

int main(int argc,char** argv){
	edgelist* g;
	time_t t1,t2;
    struct rusage r_usage;
    int ret;

	t1=time(NULL);

	printf("Reading edgelist from file %s\n",argv[1]);
	g=readedgelist(argv[1]);

	printf("Number of nodes: %lu\n",g->n);
	printf("Number of edges: %lu\n",g->e);

	free_edgelist(g);

	t2=time(NULL);

	printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
    ret = getrusage(RUSAGE_SELF,&r_usage);
    if (ret == 0)
        printf("- Memory usage = %ld kilobytes\n", r_usage.ru_maxrss);

	return 0;
}
