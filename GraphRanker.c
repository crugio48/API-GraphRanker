#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<stdbool.h>

#define MAX_EDGE_VALUE UINT_MAX
#define MAX_STARTING_INPUT 34

typedef unsigned int edgeType;

bool *visited;  // true if node already visited

edgeType *distance;  //current distance of al nodes updated during djikstra

edgeType *edgesWeight;  //matrix of all edges values  (2D)

struct graphInfo{
    int id;
    edgeType cost;
};

struct MaxHeap{
    int currentSize;
    struct graphInfo **array;
} *maxHeap;

struct MinHeap{
    int currentSize;
    int *position;     // keeps track of the position of each node in the array (needed in the function: decrease distance)
    int *array;       //the array is filled with the id of each node
} *minHeap;


int parent(int index)
{
    return index / 2;
}

int left(int index)
{
    return index * 2;
}

int right(int index)
{
    return index * 2 + 1;
}

//custom function to return distance of node in position of the array = index
edgeType getDist(int index)
{
    return distance[minHeap->array[index]];
}

//custom function to invert two minHeap nodes with index = position in the array
void invert(int index1, int index2) 
{
    //inverting position tracking
    int tempPos = minHeap->position[minHeap->array[index1]];
    minHeap->position[minHeap->array[index1]] = minHeap->position[minHeap->array[index2]];
    minHeap->position[minHeap->array[index2]] = tempPos;

    //inverting the nodes ids in the minHeap array
    int tempId = minHeap->array[index1];
    minHeap->array[index1] = minHeap->array[index2];
    minHeap->array[index2] = tempId;
}

//index is the position in the array
void minHeapify(int index)
{
    int l, r, i;
    int min = index;

    while (true){
        i = min;
        l = left(i);
        r = right(i);

        if (l <= minHeap->currentSize && getDist(l) < getDist(i)){
            min = l;
        }
        else {
            min = i;
        }

        if (r <= minHeap->currentSize && getDist(r) < getDist(min)){
            min = r;
        }

        if (min != i){
            invert(i, min);
        }
        else {
            break;
        }
    }
}

void buildMinHeap(int heapSize)
{
    minHeap->currentSize = heapSize;
    for (int i = (heapSize/2); i >= 1; i--){
        minHeapify(i);
    }
}

//extracts the id of the top node in the minHeap array
int extractMin()
{
    int min = minHeap->array[1];

    invert(1, minHeap->currentSize);
    minHeap->currentSize--;

    minHeapify(1);

    return min;
}

//id is the id of the node to decrease
void decreaseDistance(int id, edgeType newDistance)
{
    distance[id] = newDistance;

    int i = minHeap->position[id];
    int p = parent(i);

    while (i > 1 && getDist(p) > getDist(i)){
        invert(p,i);

        i = p;
        p = parent(i);
    }
}

//index is the position in the array
void maxHeapify(int index)
{
    int l, r, i;
    int max = index;

    while (true){
        i = max;
        l = left(i);
        r = right(i);

        if (l <= maxHeap->currentSize){
            if (maxHeap->array[l]->cost > maxHeap->array[i]->cost){
                max = l;
            }
            else if (maxHeap->array[l]->cost == maxHeap->array[i]->cost && maxHeap->array[l]->id > maxHeap->array[i]->id){
                max = l;
            }
            else {
                max = i;
            }
        }
        else {
            max = i;
        }

        if (r <= maxHeap->currentSize){
            if (maxHeap->array[r]->cost > maxHeap->array[max]->cost){
                max = r;
            }
            else if (maxHeap->array[r]->cost == maxHeap->array[max]->cost && maxHeap->array[r]->id > maxHeap->array[max]->id){
                max = r;
            }
        }

        if (max != i){
            struct graphInfo *temp = maxHeap->array[i];       //inverting positions of the two graphs
            maxHeap->array[i] = maxHeap->array[max];
            maxHeap->array[max] = temp;
        }
        else {
            break;
        }
    }
}

// id = graphId and cost = graphCost
void replaceMax(int id, edgeType cost)
{
    maxHeap->array[1]->id = id;
    maxHeap->array[1]->cost = cost;
    maxHeapify(1);
}

void buildMaxHeap(int topKQuantity)
{
    maxHeap->currentSize = topKQuantity;
    for (int i = (topKQuantity/2); i >= 1; i--){
        maxHeapify(i);
    }
}

edgeType peekMaxCost()
{
    return maxHeap->array[1]->cost;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

void allocateMemory(int nodesQuantity, int topKQuantity)
{
    edgesWeight = (edgeType*) malloc((nodesQuantity*nodesQuantity) * sizeof(edgeType));
    visited = (bool*) malloc(nodesQuantity * sizeof(bool));
    distance = (edgeType*) malloc(nodesQuantity * sizeof(edgeType));

    minHeap = (struct MinHeap*) malloc(sizeof(struct MinHeap) + sizeof(int) * nodesQuantity + sizeof(int) * nodesQuantity);
    minHeap->position = (int*) malloc(sizeof(int) * nodesQuantity);
    minHeap->array = (int*) malloc(sizeof(int) * nodesQuantity);

    maxHeap = (struct MaxHeap*) malloc(sizeof(struct MaxHeap) + sizeof(struct graphInfo*) * (topKQuantity + 1));
    maxHeap->array = (struct graphInfo**) malloc(sizeof(struct graphInfo*) * (topKQuantity + 1));
    for (int i = 0; i <= topKQuantity; i++){
        maxHeap->array[i] = malloc(sizeof(struct graphInfo));
    }
}

void freeMemory(int kQuantity)
{
    free(edgesWeight);
    free(visited);
    free(distance);

    free(minHeap->position);
    free(minHeap->array);
    free(minHeap);

    for (int i = 0; i <= kQuantity; i++){
        free(maxHeap->array[i]);
    }
    free(maxHeap->array);
    free(maxHeap);
}

void resetStructures(int nodesQuantity)
{
    for (int i = 0; i < nodesQuantity; i++){
        visited[i] = false;

    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

//with this function i read single chars one at the time and convert them into the final edgeType (integer) value that is stored in the
//memory pointed by the pointer
void fastReadInt(edgeType *number)
{
    *number = 0;
    char c;

    while ((c = getchar()) >= '0' && c  <= '9'){  //while c = number
        *number = *number * 10 + (c - '0');
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void computeGraphFast(int nodesQuantity, int kQuantity, int graphNumber)
{
    resetStructures(nodesQuantity);

    edgeType totalMinDistance = 0;

    for (int i = 0, j = 0; j < nodesQuantity; j++){

        for (int k = 0; k < nodesQuantity; k++){
            fastReadInt((edgesWeight + i++));
        }
    }

    distance[0] = 0;
    visited[0] = true;

    for (int i = 1 ; i < nodesQuantity; i++){
        minHeap->position[i] = i;
        minHeap->array[i] = i;

        if (*(edgesWeight + i) != 0){  //node is adiacent to node 0
            distance[i] = *(edgesWeight + i);
        }
        else {                             //node is not adiacent to node 0
            distance[i] = MAX_EDGE_VALUE;
        }
    }

    buildMinHeap(nodesQuantity-1);

    edgeType tempDist, newDist;
    int moltIndex, nodeId;

    while (minHeap->currentSize > 0){      //actual Djikstra

        nodeId = extractMin();

        if (distance[nodeId] == MAX_EDGE_VALUE) break; //because surely all remaining nodes are unreachable

        visited[nodeId] = true;

        totalMinDistance += distance[nodeId];

        moltIndex = nodeId * nodesQuantity;

        for (int j = 1; j < nodesQuantity; j++){

            if (visited[j]){
                continue;
            }

            if ((tempDist = *(edgesWeight + moltIndex + j)) == 0){
                continue;
            }

            newDist = distance[nodeId] + tempDist;

            if (distance[j] > newDist){
                decreaseDistance(j, newDist);
            }
        }
    }

    //printf("\ngraph: %d , cost: %u\n", graphNumber, totalMinDistance);

    int index = graphNumber + 1;

    maxHeap->array[index]->id = graphNumber;
    maxHeap->array[index]->cost = totalMinDistance;
    
    if (index == kQuantity){
        buildMaxHeap(kQuantity);
    }

}


void computeGraphSlow(int nodesQuantity, int kQuantity, int graphNumber)
{
    resetStructures(nodesQuantity);

    edgeType totalMinDistance = 0;

    for (int i = 0, j = 0; j < nodesQuantity; j++){

        for (int k = 0; k < nodesQuantity; k++){
            fastReadInt((edgesWeight + i++));
        }
    }

    distance[0] = 0;
    visited[0] = true;

    for (int i = 1 ; i < nodesQuantity; i++){
        minHeap->position[i] = i;
        minHeap->array[i] = i;

        if (*(edgesWeight + i) != 0){  //node is adiacent to node 0
            distance[i] = *(edgesWeight + i);
        }
        else {                             //node is not adiacent to node 0
            distance[i] = MAX_EDGE_VALUE;
        }
    }

    buildMinHeap(nodesQuantity-1);

    edgeType tempDist, newDist;
    int moltIndex, nodeId;

    while(minHeap->currentSize > 0) {   //actual Djikstra

        nodeId = extractMin();

        if (distance[nodeId] == MAX_EDGE_VALUE) break; //because surely all remaining nodes are unreachable

        visited[nodeId] = true;

        totalMinDistance += distance[nodeId];

        if (peekMaxCost() <= totalMinDistance) return;  //surely the new graph is slower than at least the k previous ones

        moltIndex = nodeId * nodesQuantity;

        for (int j = 1; j < nodesQuantity; j++){

            if (visited[j]){
                continue;
            }

            if ((tempDist = *(edgesWeight + moltIndex + j)) == 0){
                continue;
            }

            newDist = distance[nodeId] + tempDist;

            if (distance[j] > newDist){
                decreaseDistance(j, newDist);
            }
        }
    }

    //printf("\ngraph: %d , cost: %u\n", graphNumber, totalMinDistance);

    if (totalMinDistance != peekMaxCost()){
        replaceMax(graphNumber, totalMinDistance);
    } 

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void printGraphs(int numOfGraphs)
{
    if (numOfGraphs == 0){
        printf("\n");
        return;
    }

    for (int i = 1; i < numOfGraphs; i++){
        printf("%d ", maxHeap->array[i]->id);
    }

    printf("%d\n", maxHeap->array[numOfGraphs]->id);

    /*
    for (int i = 1; i <=numOfGraphs; i++){
        printf("%u ", maxHeap->array[i]->cost);
    }
    */
    return;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void readInput()
{
    int nodesQuantity, kQuantity;
    int graphNumber = 0;

    char lineBuffer[MAX_STARTING_INPUT];
    char *token;

    if (fgets(lineBuffer, MAX_STARTING_INPUT, stdin) == NULL){
        return; //error
    }

    lineBuffer[strcspn(lineBuffer, "\n\r")] = 0;

    token = strtok(lineBuffer, " ,");  //should only be ',' from specs but open input files have ' '

    nodesQuantity = atoi(token);
    token = strtok(NULL, " ,");
    kQuantity = atoi(token);

    allocateMemory(nodesQuantity, kQuantity);

    while(true){

        if (fgets(lineBuffer, MAX_STARTING_INPUT, stdin) == NULL){

            freeMemory(kQuantity);

            return;   //end program
        }

        lineBuffer[strcspn(lineBuffer, "\n\r")] = 0;

        if(strcmp(lineBuffer, "AggiungiGrafo") == 0){

            if (graphNumber < kQuantity){
                computeGraphFast(nodesQuantity, kQuantity, graphNumber);
            }
            else{
                computeGraphSlow(nodesQuantity, kQuantity, graphNumber);
            }
            graphNumber++;
        }
        else if (strcmp(lineBuffer, "TopK") == 0){
            if (graphNumber < kQuantity){
                printGraphs(graphNumber);
            }
            else {
                printGraphs(kQuantity);
            }
        }
        else{
            freeMemory(kQuantity);

            return;  //end program
        }
    }
}

int main(int argc, const char *argv[])
{
    readInput();

    return 0;
}
