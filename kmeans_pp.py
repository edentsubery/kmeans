import time as time
import pandas as pd
import numpy as np
import sys
import mykmeanssp as sp


def merge (file1 ,file2):
    table1 = pd.read_table(file1, sep =",", header = None , index_col = 0)
    table2 = pd.read_table(file2, sep =",", header = None , index_col = 0)
    return pd.merge(table1 , table2, how = "inner" , left_index = True ,right_index = True)


def convertToList(array):
    n=len(array)
    m=len(array[0])
    arrayList=[]
    for i in range (n):
        arrayRow=[]
        for j in range (m):
            arrayRow.append(array[i][j])
        arrayList.append(arrayRow)
    return arrayList



def utility(points, centroids , points_min , distribution,i):
    for index , point in enumerate(points):
        distribution[index] = distance(point , centroids[i] , points_min[index])
        points_min[index] = distribution[index]
    sum = np.sum(distribution)
    distribution = distribution/sum
    return distribution


def distance(point ,centroid , min):
    temp = np.sum(np.power(centroid - point,2))
    if(temp < min):
        min = temp
    return min


def kmeans_pp():
    k, maxIter, points = analize_arguments()
    centroids, indexes, m, n, points, points_min = create_points_list(k, points)
    calculate_initial_centroids(centroids, indexes, k, n, points, points_min)
    print_indexes(indexes, k)
    centroids = sp.fit(convertToList(centroids),convertToList(points), k, maxIter,m,n)
    print_centroids(centroids)


def create_points_list(k, points):
    points.sort_values(by=0, inplace=True)
    n, m = points.shape  # n- number of rows , m - number of columns  // check
    validate_arguments(k, m, n)
    indexes = np.empty(k)
    points_min = np.full(n, np.inf)
    centroids = np.empty((k, m))
    points = points.to_numpy()
    np.random.seed(0)
    return centroids, indexes, m, n, points, points_min


def analize_arguments():
    k = int(sys.argv[1])
    if (len(sys.argv) == 5):
        points = merge(sys.argv[3], sys.argv[4])
        maxIter = int(sys.argv[2])
    else:
        points = merge(sys.argv[2], sys.argv[3])
        maxIter = 200
    return k, maxIter, points


def validate_arguments(k, m, n):
    if (k >= n):
        print("Invalid k value")
        exit(0)
    if (m <= 0):
        print("Invalid d value")
        exit(0)


def calculate_initial_centroids(centroids, indexes, k, n, points, points_min):
    index = np.random.choice(n)
    indexes[0] = index
    centroids[0] = points[index]
    distribution = np.empty(n)
    for i in range(1, k):
        distribution = utility(points, centroids, points_min, distribution, i - 1)
        index = np.random.choice(n, 1, p=distribution)
        indexes[i] = index[0]
        centroids[i] = points[index[0]]


def print_indexes(indexes, k):
    for i in range(k - 1):
        print(str(int(indexes[i])) + ",", end="")
    print(str(int(indexes[k - 1])))


def print_centroids(centroids):  # prints centroids
    n = len(centroids)
    m = len(centroids[0])
    for i in range(n-1):
        for j in range(m-1):
            print(np.round(centroids[i][j],decimals = 4), end="")
            print(",",end="")
        print(np.round(centroids[i][-1],decimals = 4))
    for i in range(m-1):
        print(np.round(centroids[-1][i],decimals = 4), end="")
        print(",",end="")
    print(np.round(centroids[-1][-1],decimals = 4))


kmeans_pp()
