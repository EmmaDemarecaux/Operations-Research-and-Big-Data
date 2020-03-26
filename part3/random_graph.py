import networkx as nx
import itertools
import random
from networkx.drawing.nx_pylab import draw
import matplotlib.pyplot as plt
import numpy as np


def generate_random_graph(n, c, p, q):
    '''
    Implement an algorithm to generate the following random graph.
    
    Inputs:
        - n: number of nodes
        - c: number of clusters
        - p: probability that each pair of nodes in the same cluster is connected
        - q: probability that each pair of nodes in different cluster is connected
    
    Outpus:
        - G: a graph
        - clusters: cluster membership
    '''
    nodes = range(n)
    #  defining cluster membership
    clusters = []
    for node in nodes:
        clusters.append(node%c)
    # creating edges
    edges = []
    for pair in itertools.combinations(nodes, r=2):
        if clusters[pair[0]] == clusters[pair[1]]:
            if random.random() < p:
                edges.append(pair)
        else:
            if random.random() < q:
                edges.append(pair)    
    # building graph
    G= nx.Graph()
    G.add_nodes_from(nodes)
    G.add_edges_from(edges)
    return G, clusters

if __name__ == '__main__':
    
    n = 100
    c = 4
    P = [0.9, 0.8, 0.7, 0.6, 0.9, 0.8, 0.9, 0.8, 0.6]
    Q = [0.05, 0.05, 0.05, 0.05, 0.1, 0.1, 0.15, 0.15, 0.15]
    for (p, q) in zip(P,Q):
        G, clusters = generate_random_graph(n, c, p, q)
        draw(G, node_color=clusters, node_size=120, cmap=plt.cm.Reds)
        plt.title('p/q = ' + str(round(p, 2)) + '/' + str(round(q, 2)) + 
                  ' = ' + str(round(p/q, 2)))
        plt.savefig('figures/random_graph' + str(round(p/q, 2)) + 
                    '_p' + str(round(p, 2)) + '_q' + str(round(q, 2))+ '.png')
        plt.show()
