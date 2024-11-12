import pandas as pd
import pymetis
import argparse

# Define the argument parser
parser = argparse.ArgumentParser(description='Graph Partitioning Script')
parser.add_argument('file_path', type=str, help='Fragments Graph quad file')
parser.add_argument('output_file_path', type=str, help='ALlocation Output file path')
parser.add_argument('k', type=int, help='Number of partitions')

# Parse the arguments
args = parser.parse_args()
# Load the data
data = pd.read_csv(args.file_path, sep=' ', header=None, names=['source', 'predicate', 'target', 'weight'])
# Convert to integers
data['source'] = data['source'].astype(int)
data['target'] = data['target'].astype(int)
data['weight'] = data['weight'].astype(int)

# Create a graph for PyMetis
num_nodes = max(data['source'].max(), data['target'].max()) + 1
adjacency_list = [[] for _ in range(num_nodes)]
adjacency_weights = [[] for _ in range(num_nodes)]
for index, row in data.iterrows():
    adjacency_list[row['source']].append(row['target'])
    adjacency_list[row['target']].append(row['source'])
    adjacency_weights[row['source']].append(row['weight'])
    adjacency_weights[row['target']].append(row['weight'])

# Flatten the adjacency list and weights
xadj = [0]
adjncy = []
eweights = []
for i in range(num_nodes):
    adjncy.extend(adjacency_list[i])
    eweights.extend(adjacency_weights[i])
    xadj.append(len(adjncy))

# Perform the partitioning
n_cuts, membership = pymetis.part_graph(nparts=args.k, xadj=xadj, adjncy=adjncy, eweights=eweights)
membership = [m + 1 for m in membership]

# Create the affectation file
affectation = pd.DataFrame({'fragment': range(1,num_nodes), 'partition': membership[1:]})
affectation.to_csv(args.output_file_path, sep=' ', header=False, index=False)

print("Affectation file created successfully.")

