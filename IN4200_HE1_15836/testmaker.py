import numpy as np

n = int(5e3)
edges = 50

f = open("test_test.txt", "w")

f.write("""# Undirected graph: facebook_combined.txt
# A connectivity graph representing social circles from Facebook
# Nodes: {} Edges: {}
# FromNodeId    ToNodeId\n""".format(n,n*edges))

for i in range(n):
	r = np.random.choice(range(n),edges)
	for j in r:
		f.write("{} {}\n".format(i,j))

f.close()