import networkx as nx
from networkx.drawing.nx_pylab import draw
import matplotlib.pyplot as plt
import sys


if __name__ == '__main__':
    
    edges = []
    nodes = []
    communities = []
    file_graph = open(sys.argv[1], "r")
#    file_graph = open(tt = , "r")
    for line in file_graph:
        i = int(line.split(None, 1)[0])
        j = int(line.split(None, 1)[1][:-1])
        edges.append((i, j))
    file_graph.close()
    file_communities = open(sys.argv[2], "r")
#    file_communities = open("results/random9_communities.txt", "r")
    for line in file_communities:
        node = int(line.split(None, 1)[0])
        community = int(line.split(None, 1)[1][:-1])
        nodes.append(node)
        communities.append(community)
    file_communities.close()
    dict_comm = dict([(j, i) for (i,j) in enumerate(set(communities))])
    if len(dict_comm.values()) <= 1:
        communities = 'blue'
    else:
        communities = [dict_comm[i] for i in communities]
    G= nx.Graph()
    G.add_nodes_from(nodes)
    G.add_edges_from(edges)
    # draw graph
    draw(G, node_color=communities, node_size=120, cmap=plt.cm.Blues)
    plt.title(sys.argv[1][7:-4] + " labels")
    plt.savefig("figures/" + sys.argv[1][7:-4] + "_labels.png")
    plt.show()
