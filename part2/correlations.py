import matplotlib.pyplot as plt
import pandas as pd  
import seaborn as sns

# PageRank with alpha = 0.15
file_015 = open("results/wiki_correlations_0_15.txt", "r")
nodes = []
pr_015 = []
for line_015 in file_015:
    line_list_015 = line_015.split(None, 1)
    nodes.append(line_list_015[0])
    pr_015.append(line_list_015[1])
file_015.close()
nodes = [int(i) for i in nodes[1:]]
pr_015 = [float(i[:-1]) for i in pr_015[1:]]

# PageRank with alpha = 0.1
file_01 = open("results/wiki_correlations_0_1.txt", "r")
pr_01 = []
for line_01 in file_01:
    pr_01.append(line_01.split(None, 1)[1])
file_01.close()
pr_01 = [float(i[:-1]) for i in pr_01[1:]]

# PageRank with alpha = 0.2
file_02 = open("results/wiki_correlations_0_2.txt", "r")
pr_02 = []
for line_02 in file_02:
    pr_02.append(line_02.split(None, 1)[1])
file_02.close()
pr_02 = [float(i[:-1]) for i in pr_02[1:]]

# PageRank with alpha = 0.5
file_05 = open("results/wiki_correlations_0_5.txt", "r")
pr_05 = []
for line_05 in file_05:
    pr_05.append(line_05.split(None, 1)[1])
file_05.close()
pr_05 = [float(i[:-1]) for i in pr_05[1:]]

# PageRank with alpha = 0.9
file_09 = open("results/wiki_correlations_0_9.txt", "r")
pr_09 = []
for line_09 in file_09:
    pr_09.append(line_09.split(None, 1)[1])
file_09.close()
pr_09 = [float(i[:-1]) for i in pr_09[1:]]

# in-degree
file_din = open("results/wiki_degrees_in.txt", "r")
din = []
for line_din in file_din:
    din.append(line_din.split(None, 1)[1])
file_din.close()
din = [int(i[:-1]) for i in din]

# in-degree
file_dout = open("results/wiki_degrees_out.txt", "r")
dout = []
for line_dout in file_dout:
    dout.append(line_dout.split(None, 1)[1])
file_dout.close()
dout = [int(i[:-1]) for i in dout]

pagerank_dict = {'pr_015': pr_015, 'pr_01': pr_01, 'pr_02': pr_02, 'pr_05': pr_05,
                 'pr_09': pr_09, 'din': din, 'dout': dout}
pagerank_df = pd.DataFrame(pagerank_dict) 
    
to_plot = ['din', 'dout', 'pr_01', 'pr_02', 'pr_05', 'pr_09']
y_label = ['in-degree', 'out-degree', 'PageRank with alpha = 0.1',
           'PageRank with alpha = 0.2', 'PageRank with alpha = 0.5',
           'PageRank with alpha = 0.9']

for i in range(len(to_plot)):
    fig1, ax1 = plt.subplots()
    sns.regplot(x=pagerank_df['pr_015'], y=pagerank_df[to_plot[i]], fit_reg=False, 
                marker="+")
    plt.title('Correlations')
    plt.xlabel('PageRank with alpha = 0.15')  # Set x-axis label
    plt.ylabel(y_label[i])  # Set y-axis label
    plt.savefig('figures/corr_' + to_plot[i] + '.png')
    plt.show()
    
    fig2, ax2 = plt.subplots()
    ax2.set(xscale='log', yscale='log')  # Set the scale of the x-and y-axes
    sns.regplot(x=pagerank_df['pr_015'], y=pagerank_df[to_plot[i]], fit_reg=False, 
                ax=ax2, marker="+")
    plt.title('Correlations')
    plt.xlabel('PageRank with alpha = 0.15')  # Set x-axis label
    plt.ylabel(y_label[i])  # Set y-axis label
    plt.savefig('figures/log_corr_' + to_plot[i] + '.png')
    plt.show()
