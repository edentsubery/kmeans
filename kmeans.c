#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Cluster Cluster;
typedef struct Point Point;
static int d;
static int k;
static int numberOfPoints;
static int MAX_ITER = 200;


struct Point {
    double *coordinates;
};

struct Cluster {
    Point sum_by_coordinates;
    Point centroid;
    int size;
};


static Cluster *createClusterArray(PyObject* pyCentroids);

static PyObject* createReturnedArray(const Cluster *clusterArray);

static void calculateCentroids(Point *points, Cluster *clusterArray);


static Point *getPointPointer(void);

static void freePointPointer(Point *arr);

static void readAllPoints( Point *points,PyObject* pyPoints);

static int calculateNewCentroid(Cluster *cluster);

static double distanceFromCentroid(Cluster cluster, Point point);

static void emptyClusters(Cluster *clusters);

static void addPointToCluster(Cluster *cluster, Point point);

static void freeClusterArray(Cluster *cluster);

static void addPointToArr(Point *pointsArr, Point point, int i);

static int closestCentroid(Cluster *clusters, Point point);

static void readAllPoints( Point *points,PyObject* pyPoints) {
    Point *curPoint;
    int index;
    int i;
    PyObject* pyList;
    index = 0;
    while (index < numberOfPoints) {
        curPoint = malloc(sizeof(Point));
        assert(curPoint!=NULL);
        curPoint->coordinates = malloc(d * sizeof(double));
        pyList=PyList_GetItem(pyPoints,index);
        assert(curPoint->coordinates!=NULL);
        for (i = 0; i < d; i++) {
            curPoint->coordinates[i] = PyFloat_AsDouble(PyList_GetItem(pyList,i));
        }
        addPointToArr(points, *curPoint, index++);
    }
}

static Point* getPointPointer(void) {
    Point *points = malloc(numberOfPoints * sizeof(Point));
    assert(points!=NULL);
    return points;
}

static void calculateCentroids(Point *points, Cluster *clusterArray) {
    int j, i, index, changed;
    changed = 1;
    for (j = 0; changed && j < MAX_ITER; j++) {
        emptyClusters(clusterArray);
        for (i = 0; i < numberOfPoints; i++) {
            index = closestCentroid(clusterArray, points[i]);
            addPointToCluster(&clusterArray[index], points[i]);
        }
        changed = calculateNewCentroid(clusterArray);
    }
}

static PyObject* createReturnedArray(const Cluster *clusterArray) {
    PyObject* returnedList=PyList_New(k);
    PyObject* returnedPoint;
    int i;
    int j;
    for (i = 0; i < k; i++) {
        returnedPoint=PyList_New(d);
        for (j = 0; j < d; j++) {
            PyList_SetItem(returnedPoint,j,PyFloat_FromDouble(clusterArray[i].centroid.coordinates[j]));
        }
        PyList_SetItem(returnedList,i,returnedPoint);
    };
    return returnedList;
}

static void freePointPointer(Point *arr) {
    free(arr);
}

static int calculateNewCentroid(Cluster *cluster) {
    int changed, m, i;
    changed = 0;
    for (m = 0; m < k; m++) {
        for (i = 0; i < d; i++) {
            double coordinateAverage;
            if (cluster[m].size != 0) {
                coordinateAverage = cluster[m].sum_by_coordinates.coordinates[i] / (cluster[m].size);
            } else {
                coordinateAverage = 0;
            }
            if (cluster[m].centroid.coordinates[i] != coordinateAverage) {
                cluster[m].centroid.coordinates[i] = coordinateAverage;
                changed = 1;
            }
        }
    }
    return changed;
}

static double distanceFromCentroid(Cluster cluster, Point point) {
    double distance;
    int i;
    distance = 0;
    for (i = 0; i < d; i++) {
        distance += (point.coordinates[i] - cluster.centroid.coordinates[i])*(point.coordinates[i] - cluster.centroid.coordinates[i]);
    }
    return distance;
}

static Cluster *createClusterArray(PyObject* pyCentroids) {
    int i,j;
    Cluster *arr;
    void *pointerForCentroid;
    void *pointerForSums;
    PyObject* pyList;
    arr = malloc(k * sizeof(Cluster));
    assert(arr!=NULL);
    for (i = 0; i < k; i++) {
        pointerForCentroid = malloc(d * sizeof(double));
        assert(pointerForCentroid!=NULL);
        pointerForSums = malloc(d * sizeof(double));
        assert(pointerForSums!=NULL);
        arr[i].centroid.coordinates = pointerForCentroid;
        pyList=PyList_GetItem(pyCentroids,i);
        for(j=0;j<d;j++){
            arr[i].centroid.coordinates[j]=PyFloat_AsDouble(PyList_GET_ITEM(pyList,j)) ;
        }
        arr[i].sum_by_coordinates.coordinates = pointerForSums;
        arr[i].size = 0;
    }
    return arr;
}

static void freeClusterArray(Cluster *cluster) {
    int i;
    for (i = 0; i < k; i++) {
        free(cluster[i].centroid.coordinates);
        free(cluster[i].sum_by_coordinates.coordinates);
    }
    free(cluster);
}

static int closestCentroid(Cluster *clusters, Point point) {
    int index, i;
    double minDistance, dis;
    index = 0;
    minDistance = distanceFromCentroid(clusters[index], point);
    for (i = 1; i < k; i++) {
        dis = distanceFromCentroid(clusters[i], point);
        if (dis < minDistance) {
            minDistance = dis;
            index = i;
        }
    }
    return index;
}

static void addPointToArr(Point *pointsArr, Point point, int i) {
    pointsArr[i] = point;
}

static void addPointToCluster(Cluster *cluster, Point point) {
    int j;
    for (j = 0; j < d; j++) {
        cluster->sum_by_coordinates.coordinates[j] += point.coordinates[j];
    }
    cluster->size++;
}

static void emptyClusters(Cluster *clusters) {
    int i, j;
    for (i = 0; i < k; i++) {
        for (j = 0; j < d; j++) {
            clusters[i].sum_by_coordinates.coordinates[j] = 0;
        }
        clusters[i].size = 0;
    }
}


static PyObject* kmeans_capi(PyObject *self, PyObject *args){
    Point *points;
    Cluster *clusterArray;
    PyObject* result;
    PyObject* pyCentroids;
    PyObject* pyPoints;
    if(!PyArg_ParseTuple(args,"OOiiii",&pyCentroids, &pyPoints,&k,&MAX_ITER,&d,&numberOfPoints)){
        printf("parsing failed\n");
    }
    points = getPointPointer();
    readAllPoints(points,pyPoints);
    clusterArray = createClusterArray(pyCentroids);
    calculateCentroids(points, clusterArray);
    result=createReturnedArray(clusterArray);
    freeClusterArray(clusterArray);
    freePointPointer(points);
    return result;
}

static PyMethodDef capiMethods[]={
        {"fit",
         (PyCFunction) kmeans_capi ,
         METH_VARARGS,
         PyDoc_STR("calculates k means with the initial centroids passed to it")},
        {NULL,NULL,0,NULL}
};

static struct PyModuleDef moduledef={
        PyModuleDef_HEAD_INIT,
        "mykmeanssp",
        NULL,
        -1,
        capiMethods
};

PyMODINIT_FUNC
PyInit_mykmeanssp(void){
    PyObject *m;
    m=PyModule_Create(&moduledef);
    if(!m){
        return NULL;
    }
    return m;
}

