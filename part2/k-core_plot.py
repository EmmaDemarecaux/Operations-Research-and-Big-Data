import matplotlib
import matplotlib.pyplot as plt
import pandas as pd  
import seaborn as sns
import numpy as np

input_graph = 'results/net_k-core.txt'
input_degrees = 'results/net_degrees.txt'
title = 'Google Scholar'
output = 'figures/net_coreness_degree.png'
names_file = 'graphs/ID.txt'


if __name__ == '__main__':
    
    nodes = []
    cores = []
    file = open(input_graph, "r")
    i = 0
    for line in file:
        if i > 0:
            node = int(line.split(None, 1)[0])
            core = int(line.split(None, 1)[1][:-1])
            nodes.append(node)
            cores.append(core)
        i += 1
    file.close()
    degrees = []
    file_deg = open(input_degrees, "r")
    for line in file_deg:
        degree = int(line.split(None, 1)[1][:-1])
        degrees.append(degree)
    file_deg.close()
    core_dict = {'nodes': nodes, 'degrees': degrees, 'cores': cores}
    core_df = pd.DataFrame(core_dict) 
    core_df['tuple'] = list(zip(core_df['cores'], core_df['degrees']))
    density_dict = core_df['tuple'].value_counts().to_dict()
    core_df['density'] = [density_dict[i] for i in zip(core_df['cores'], core_df['degrees'])]
    # degree-coreness plot
    fig, ax = plt.subplots()
    ax.set(xscale='log', yscale='log')
    ax = sns.lineplot(x=np.arange(0.7, max(cores)+7, 0.01), 
                      y=np.arange(0.7, max(cores)+7, 0.01),
                      color='black')
    points = plt.scatter(core_df['degrees'], core_df['cores'], c=core_df['density'], 
                         s=30, marker=',', norm=matplotlib.colors.LogNorm(), cmap="jet") 
    plt.colorbar(points)
    plt.title(title)
    plt.xlabel('Degree')  # Set x-axis label
    plt.ylabel('Coreness')  # Set y-axis label
    plt.savefig(output)
    plt.show()
    # anomalous authors are the vertices with the highest corenes
    c = max(cores)
    anomalous_nodes = [i for (i,j) in enumerate(cores) if j==c]
    # find names
    file = open(names_file, "r")
    names = []
    print("Anomalous Authors:\n")
    for line in file:
        if int(line.split(None, 1)[0]) in anomalous_nodes:
            names.append((int(line.split(None, 1)[0]), line.split(None, 1)[1][:-1]))
            print(int(line.split(None, 1)[0]), ": ", line.split(None, 1)[1][:-1])
    file.close()
    