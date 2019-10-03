import numpy as np
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
import matplotlib.cm as cm

def unique(list1): 
  
    # intilize a null list 
    unique_list = [] 
      
    # traverse for all elements 
    for x in list1: 
        # check if exists in unique_list or not 
        if x not in unique_list: 
            unique_list.append(x)
    return unique_list

n=500
k=3


f = open("../data/kruskal_centers.txt","r")
f=f.readline()
kruskal=list(map(int,f.split(' ' )[:-1]))
kruskal=np.array(kruskal)
assert(len(unique(kruskal))==k)


f = open("../data/kmeans_centers.txt","r")
f=f.readline()
kmeans=list(map(int,f.split(' ' )[:-1]))
kmeans=np.array(kmeans)

f=open("../data/iris2.data","r")
f=f.readlines()
points=[]
for x in f:  
    points.append(np.array(list(map(float,x.split(' ')[:-1]))))


f=open("../data/kmeans_points.txt","r")
f=f.readlines()
centers=[]
for x in f:
    centers.append(np.array(list(map(float,x.split(' ')[:-1]))))


centers=np.array(centers).T
points=np.array(points).T
points=points[:,:n]
pca = PCA(n_components=2)
components=pca.fit_transform(points)
centers=(components.T).dot(centers)

def plot_kruskal():
    colors = cm.rainbow(np.linspace(0, 1, k))
    for tgt,color in zip(unique(kruskal),colors):
        idx=kruskal[:n]==tgt
        plt.scatter(points[0,idx],points[1,idx],c=color)#,c=np.array([np.random.rand(),np.random.rand(),np.random.rand()]))

def plot_kmeans():
    colors = cm.rainbow(np.linspace(0, 1, k))
    for tgt, color in zip(range(k),colors):
        idx=kmeans==tgt
        plt.scatter(points[0,idx],points[1,idx],c=color,marker='.')
        #plt.scatter(centers[0,tgt],centers[1,tgt],s=14,c=color,marker='o')



