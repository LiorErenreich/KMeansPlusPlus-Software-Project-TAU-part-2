import os
os.environ['OPENBLAS_NUM_THREADS']='1'
import sys
from ctypes import c_void_p, c_double, c_int, CDLL
import numpy as np
import pandas as pd
from numpy.ctypeslib import ndpointer

def merge(file1,file2):
    table1=pd.read_table(file1, sep=",", header=None,index_col=0)
    table2=pd.read_table(file2, sep=",", header=None,index_col=0)
    tablemerge=pd.merge(table1, table2,how='inner',left_index=True, right_index=True)
    return tablemerge


def main(K , max_iter, fileName_1, fileName_2):
    try:
        K=int(K)
    except:
        print("K must be int")
        return
    if(K<0):
        print("K must be positive")
    assert(K>=0)
    try:
        max_iter=int(max_iter)
    except:
        print("max_iter must be int")
        return
    if(max_iter<0):
        print("max_iter must be positive")
    assert(max_iter>=0)

    (vectors,centroids,tablemerge,N,d)= kmeanspp(K , max_iter, fileName_1, fileName_2)
    stringres = ""
    for cord in vectors:
    	stringres += str(int(cord)) + ","
    stringres = stringres[0:-1]
    print(stringres)
    N_array=tablemerge.values.tolist()
    oneDarray = toOneD(N_array,N,d)
    OneDcentroids = toOneD(centroids,K,d)
    
    c_lib = CDLL('./kmeans.cpython-37m-x86_64-linux-gnu.so')

    c_lib.cmain.argtype = (c_int,c_int,c_int,c_int,ndpointer(dtype=c_double,shape=(d*N,)),ndpointer(dtype=c_double,shape=(d*K,)))
    c_lib.cmain.restype = (ndpointer(dtype=c_double,shape=(d*K,)))

    darray = c_lib.cmain(K,max_iter,N,d,c_void_p(oneDarray.ctypes.data),c_void_p(OneDcentroids.ctypes.data))
    doublelist = [x for x in darray]

    doublelist = np.round(doublelist,4)
    stringres=""
    for i in range(K):
        for j in range(d):
            stringres = stringres + str(doublelist[i*d+j]) + ","
        stringres= stringres[0:-1]
        print(stringres)
        stringres=""



def toOneD(N_array,N,d):
    result=np.zeros(N*d)
    for i in range(N):
        for j in range(d):
            result[i*d+j] += N_array[i][j]
    return result

def kmeanspp(K , max_iter, fileName_1, fileName_2):
    tablemerge= merge(fileName_1, fileName_2)    
    (N,d)= tablemerge.shape
    if(K>=N):
        print("K>=N is invalid")
    assert(N>K)

    sortedtable=tablemerge.sort_values(by=0)
    
    P=np.random.seed(0)
    inx=np.random.choice(sortedtable.index,p=P)
    point=sortedtable.loc[inx]
    vectors = [inx]
    centroids = [point]
    Mio = [[i for i in point]]
    Z=1
    P=[0 for i in range(N)]
    while (Z<K):
        D=[0 for i in range( N)]
        for i in range(N):
            D[i]=np.linalg.norm(sortedtable.iloc[i]-Mio[0])
            D[i] = D[i]*D[i]
            min_j = 0
            tmp=0
            for j in range(Z):
                tmp = np.linalg.norm(sortedtable.iloc[i]-Mio[j])
                tmp = tmp*tmp
                if (tmp < D[i]):
                    D[i] = tmp
                    min_j = j
        Z+=1
        for i in range (N):
            P[i] = D[i]/sum(D)
        inx=np.random.choice(sortedtable.index,p=P)
        point=sortedtable.loc[inx]
        vectors.append(inx)
        centroids.append(point)
        Mio.append([i for i in point])
    return (vectors,centroids,sortedtable,N,d)


if __name__ == "__main__":
    if (len(sys.argv)==4):
        main(sys.argv[1], 300, sys.argv[2],sys.argv[3])
    elif (len(sys.argv)==5):
        main(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])
    elif ((len(sys.argv) != 5) and (len(sys.argv)!=4)):
        print("There is problem with the number of arguments")


