import networkx as nx
import matplotlib.pyplot as plt

if __name__ == '__main__':
    # LFR benchmark
    clusters = []
    for line in open("./LFR-Benchmark/community.dat").readlines():
        clusters.append(int(line.replace('\t',' ').replace('\n',' ').split()[1])-1)
    
    edges = []
    w = []
    i = 0
    max_node = 0
    for line in open("./LFR-Benchmark/network.dat").readlines():
        if i > 0:
            edge = line.replace('\t',' ').replace('\n',' ').split()
            w.append(edge[2])
            max_node = max(int(edge[0])-1, int(edge[1])-1, max_node)
            edges.append((int(edge[0])-1, int(edge[1])-1))
        else:
            nb_nodes = str(line[9:12])
        i+=1
    
    file_name = "n" + str(nb_nodes) + "_c" + str(len(set(clusters)))
    graph_file_name = "./graphs/lfr_" + file_name + ".txt"
    res_file_name = "./results/lfr_" + file_name + ".txt"
    figure_file_name = "./figures/lfr_" + file_name + ".png"
    
    # build graph  
    G= nx.Graph()
    G.add_nodes_from(range(max_node+1))
    G.add_edges_from(edges)
    nx.draw(G, node_color=clusters, node_size=120, cmap=plt.cm.Greens)
    plt.title("Nodes = " + str(file_name[1:4]) + ", clusters = " + str(len(set(clusters))))
    plt.savefig(figure_file_name)
    plt.show()
    
    # save graph
    res_file = open(graph_file_name, "w")
    res_file.writelines([str(i) + " " + str(j) + "\n" for (i,j) in G.edges()])
    res_file.close()
    # save clusters
    clusters_file = open(res_file_name, "w")
    clusters_file.writelines([str(i) + " " + str(j) + "\n" for (i,j) in enumerate(clusters)])
    clusters_file.close()
    
