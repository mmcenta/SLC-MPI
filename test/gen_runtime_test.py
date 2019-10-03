import os
import math
import random

# Convert graphs to edge lists for output
from networkx.convert import to_edgelist

# Erdős-Rényi graph generator for dense graphs
from networkx.generators.random_graphs import gnp_random_graph
# Erdős-Rényi graph generator for sparse graphs
from networkx.generators.random_graphs import fast_gnp_random_graph

# Watts–Strogatz small-world graph generator
from networkx.generators.random_graphs import watts_strogatz_graph


def write_graph_file(filename, g):
  with open("./test/data/"+filename, mode='w') as file:
    edges = to_edgelist(g)

    file.write("{:}\n".format(g.number_of_edges()))
        
    for e in edges:
      # get random weight
      w = random.random() # in [0.1, 1.0]

      # write edge to file
      file.write("{:} {:} {:}\n".format(e[0], e[1], w))


def generate_graphs(prefix, n_range, generator):
  filename_list = []

  for n in n_range:
    filename = prefix+str(n)+".txt"
    filename_list.append(filename)

    g = generator(n)

    write_graph_file(filename, g)
  
  return filename_list


def sparse_er_generator(n):
  p = 2 / n
  return fast_gnp_random_graph(n, p)


def dense_er_generator(n):
  p = 2 * math.log(n) / n
  return gnp_random_graph(n, p)


def small_world_generator(n):
  k = max(2, int(2*math.log(n)))
  return watts_strogatz_graph(n, k, 0.1)


if __name__ == "__main__":
  tests = dict() # maps test names to a list of test files
  n_range = [ int(i*1e3) for i in range(1, 11) ]

  # Create the data directory if it doesn't exist
  if not os.path.exists("./test/data/"):
    os.makedirs("./test/data/")

  # Generate test graphs
  print("Generating graphs...")

  tests["Sparse ER"] = generate_graphs("sparse_er_",
                                       n_range,
                                       sparse_er_generator)
  print("Sparse ER done.")

  tests["Dense ER"] = generate_graphs("dense_er_",
                                      n_range,
                                      dense_er_generator)
  print("Dense ER done.")

  tests["Small World"] = generate_graphs("small_world_",
                                         n_range,
                                         small_world_generator)
  print("Small World done.")

  # Generate index.txt

  with open("./test/data/index.txt", mode="w") as idx:
    # Print number of tests
    idx.write("{:}\n".format(len(tests.keys())))

    for test, files in tests.items():
      # Print the test name
      idx.write(test+"\n")

      # Print the number of files followed by their names
      idx.write("{:} ".format(len(files)))

      for f in files:
        idx.write(f+" ")

      idx.write("\n")
      
  print("Finished!")
