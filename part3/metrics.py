import networkx as nx
from sklearn.metrics.cluster import adjusted_mutual_info_score, adjusted_rand_score, normalized_mutual_info_score

graph_files = ["random_n400_c4_pq9_p0.9_q0.1.txt", "random_n400_c4_pq7_p0.7_q0.1.txt", 
          "random_n400_c4_pq6_p0.6_q0.1.txt", "random_n400_c4_pq5_p0.5_q0.1.txt", 
          "random_n400_c4_pq4_p0.6_q0.15.txt", "random_n400_c4_pq3_p0.6_q0.2.txt", 
          "random_n400_c4_pq2_p0.6_q0.3.txt", "random_n400_c4_pq1_p0.3_q0.3.txt"]
cluster_files = ["random_n400_c4_pq9_p0.9_q0.1_clusters.txt", "random_n400_c4_pq7_p0.7_q0.1_clusters.txt", 
            "random_n400_c4_pq6_p0.6_q0.1_clusters.txt", "random_n400_c4_pq5_p0.5_q0.1_clusters.txt", 
            "random_n400_c4_pq4_p0.6_q0.15_clusters.txt", "random_n400_c4_pq3_p0.6_q0.2_clusters.txt", 
            "random_n400_c4_pq2_p0.6_q0.3_clusters.txt", "random_n400_c4_pq1_p0.3_q0.3_clusters.txt"]
lp_files = ["random_n400_c4_pq9_communities.txt", "random_n400_c4_pq7_communities.txt", 
      "random_n400_c4_pq6_communities.txt", "random_n400_c4_pq5_communities.txt", 
      "random_n400_c4_pq4_communities.txt", "random_n400_c4_pq3_communities.txt", 
      "random_n400_c4_pq2_communities.txt", "random_n400_c4_pq1_communities.txt"]
louvain_files = ["random_n400_c4_pq9_p0.9_q0.1_louvain.txt", "random_n400_c4_pq7_p0.7_q0.1_louvain.txt", 
           "random_n400_c4_pq6_p0.6_q0.1_louvain.txt", "random_n400_c4_pq5_p0.5_q0.1_louvain.txt", 
           "random_n400_c4_pq4_p0.6_q0.15_louvain.txt", "random_n400_c4_pq3_p0.6_q0.2_louvain.txt", 
           "random_n400_c4_pq2_p0.6_q0.3_louvain.txt", "random_n400_c4_pq1_p0.3_q0.3_louvain.txt"]

lfr_graph_files = ["lfr_n128_c4.txt", "lfr_n384_c12.txt"]
lfr_cluster_files = ["lfr_n128_c4.txt", "lfr_n384_c12.txt"]
lfr_lp_files = ["lfr_n128_c4_communities.txt", "lfr_n384_c12_communities.txt"]
lfr_louvain_files = ["lfr_n128_c4_louvain.txt", "lfr_n384_c12_louvain.txt"]


if __name__ == '__main__':
    # benchmark made in question 1
    for k in range(len(graph_files)):
        file_graph = graph_files[k]
        file_cluster = cluster_files[k]
        file_lp = lp_files[k]
        file_louvain = louvain_files[k]
        print("\nFile: n=400, c=4 and p/q=" + file_lp[-17])
        
        # load graph
        edges = []
        file_graph = open("graphs/" + file_graph, "r")
        for line in file_graph:
            i = int(line.split(None, 1)[0])
            j = int(line.split(None, 1)[1][:-1])
            edges.append((i, j))
        file_graph.close()
        
        # load labels
        # -- true clusters
        nodes = []
        clusters = []
        file_cluster = open("results/" + file_cluster, "r")
        for line in file_cluster:
            node = int(line.split(None, 1)[0])
            cluster = int(line.split(None, 1)[1][:-1])
            nodes.append(node)
            clusters.append(cluster)
        file_cluster.close()
     
        # -- lp communities
        lps = []
        file_lp = open("results/" + file_lp, "r")
        for line in file_lp:
            lp = int(line.split(None, 1)[1][:-1])
            lps.append(lp)
        file_lp.close()
        dict_lps = dict([(j, i) for (i,j) in enumerate(set(lps))])
        lps = [dict_lps[i] for i in lps]
            
        # -- louvain communities
        louvains = []
        file_louvain = open("results/" + file_louvain, "r")
        for line in file_louvain:
            louvain = int(line.split(None, 1)[1][:-1])
            louvains.append(louvain)
        file_louvain.close()
        louvains = louvains[:-len(set(louvains))]
            
        # build graph  
        G= nx.Graph()
        G.add_nodes_from(nodes)
        G.add_edges_from(edges)
    
        # Modularity
        communities = []
        for l in set(lps):
            communities.append(set([i for (i, j) in enumerate(lps) if j == l]))
        print(" -- LP mod:     ", nx.algorithms.community.modularity(G, communities))
        communities = []
        for l in set(louvains):
            communities.append(set([i for (i, j) in enumerate(louvains) if j == l]))
        print(" -- Louvain mod:", nx.algorithms.community.modularity(G, communities))
        
        # adjusted_mutual_info_score
        print(" -- LP AMI:     ", adjusted_mutual_info_score(clusters, lps, average_method='arithmetic'))
        print(" -- Louvain AMI:", adjusted_mutual_info_score(clusters, louvains, average_method='arithmetic'))
        
        # adjusted_rand_score
        print(" -- LP RI:      ", adjusted_rand_score(clusters, lps))
        print(" -- Louvain RI: ", adjusted_rand_score(clusters, louvains))
        
        # normalized_mutual_info_score
        print(" -- LP NMI:     ", normalized_mutual_info_score(clusters, lps, average_method='arithmetic'))
        print(" -- Louvain NMI:", normalized_mutual_info_score(clusters, louvains, average_method='arithmetic'))
    
    # LFR benchmark
    for k in range(len(lfr_graph_files)):
        lfr_file_graph = lfr_graph_files[k]
        lfr_file_cluster = lfr_cluster_files[k]
        lfr_file_lp = lfr_lp_files[k]
        lfr_file_louvain = lfr_louvain_files[k]
        print("\nFile LFR: n=" + str(lfr_file_graph[5:8]) + ", c=" + str(lfr_file_graph[10:-4]))
        
        # load graph
        edges = []
        lfr_file_graph = open("graphs/" + lfr_file_graph, "r")
        for line in lfr_file_graph:
            i = int(line.split(None, 1)[0])
            j = int(line.split(None, 1)[1][:-1])
            edges.append((i, j))
        lfr_file_graph.close()
        
        # load labels
        # -- true clusters
        nodes = []
        clusters = []
        lfr_file_cluster = open("results/" + lfr_file_cluster, "r")
        for line in lfr_file_cluster:
            node = int(line.split(None, 1)[0])
            cluster = int(line.split(None, 1)[1][:-1])
            nodes.append(node)
            clusters.append(cluster)
        lfr_file_cluster.close()
     
        # -- lp communities
        lps = []
        lfr_file_lp = open("results/" + lfr_file_lp, "r")
        for line in lfr_file_lp:
            lp = int(line.split(None, 1)[1][:-1])
            lps.append(lp)
        lfr_file_lp.close()
        dict_lps = dict([(j, i) for (i,j) in enumerate(set(lps))])
        lps = [dict_lps[i] for i in lps]
            
        # -- louvain communities
        louvains = []
        lfr_file_louvain = open("results/" + lfr_file_louvain, "r")
        for line in lfr_file_louvain:
            louvain = int(line.split(None, 1)[1][:-1])
            louvains.append(louvain)
        lfr_file_louvain.close()
        louvains = louvains[:-len(set(louvains))]
            
        # build graph  
        G= nx.Graph()
        G.add_nodes_from(nodes)
        G.add_edges_from(edges)
    
        # Modularity
        communities = []
        for l in set(lps):
            communities.append(set([i for (i, j) in enumerate(lps) if j == l]))
        print(" -- LP mod:     ", nx.algorithms.community.modularity(G, communities))
        communities = []
        for l in set(louvains):
            communities.append(set([i for (i, j) in enumerate(louvains) if j == l]))
        print(" -- Louvain mod:", nx.algorithms.community.modularity(G, communities))
        
        # adjusted_mutual_info_score
        print(" -- LP AMI:     ", adjusted_mutual_info_score(clusters, lps, average_method='arithmetic'))
        print(" -- Louvain AMI:", adjusted_mutual_info_score(clusters, louvains, average_method='arithmetic'))
        
        # adjusted_rand_score
        print(" -- LP RI:      ", adjusted_rand_score(clusters, lps))
        print(" -- Louvain RI: ", adjusted_rand_score(clusters, louvains))
        
        # normalized_mutual_info_score
        print(" -- LP NMI:     ", normalized_mutual_info_score(clusters, lps, average_method='arithmetic'))
        print(" -- Louvain NMI:", normalized_mutual_info_score(clusters, louvains, average_method='arithmetic'))
