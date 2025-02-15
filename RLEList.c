#include "RLEList.h"
#include<stdlib.h>
#include <assert.h>


#define NULL_LIST -1
#define ILLEGAL_DATA 0


typedef struct node{
    char val;
    int valCount;
    struct node* next;
}*Node;

//not available for RLElist interface user
static int countNodes(RLEList list);
static int numOfDigits(RLEList list);
static Node createNode(char val);
static Node getLastNode(RLEList list);
static void mergeNodes(RLEList list);
static void resultSaver(RLEListResult *result, RLEListResult result_identifier);


struct RLEList_t{
    Node listHead;
    int listLen;
};
//implement the functions here


static Node createNode(char val)
{
    Node newNode=malloc(sizeof(*newNode));
    if(!newNode)
    {
        return NULL;
    }
    assert(newNode);
    newNode->val=val;
    newNode->valCount=1;
    newNode->next=NULL;
    return newNode;
}


static int countNodes(RLEList list)
{
    assert(list);
    int nodeCounter=0;
    Node ptr = list->listHead;
    while(ptr)
    {
        nodeCounter++;
        ptr=ptr->next;
    }
    return nodeCounter;
}


static Node getLastNode(RLEList list)
{
    Node currentNode = list->listHead;
    if(currentNode == NULL)
    {
        return NULL;
    }
    while(currentNode->next != NULL)
    {
        currentNode=currentNode->next;
    }
    return currentNode;
}


RLEList RLEListCreate()
{
    RLEList newList = malloc(sizeof(*newList));
    if(!newList)
    {
        return NULL;
    }
    assert(newList);
    newList->listHead=NULL;
    newList->listLen = 0;
    return newList;
}


void RLEListDestroy(RLEList list)
{
    if(!list)
    {
        return;
    }
    assert(list);
    while(list->listHead)
    {
        Node toDelete= list->listHead;
        list->listHead=list->listHead->next;
        free(toDelete);
    }
    free(list);
}


RLEListResult RLEListAppend(RLEList list, char value)
{
    if(list == NULL)
    {
        return RLE_LIST_NULL_ARGUMENT;
    }
    Node lastNode = getLastNode(list);
    if((lastNode == NULL) || lastNode->val != value)
    {
        Node newNode = createNode(value);
        if(newNode == NULL)
        {
            return RLE_LIST_OUT_OF_MEMORY;
        }
        if(lastNode==NULL)
        {
            list->listHead=newNode;
        }
        else
        {
            lastNode->next=newNode;
        }
    }
    else
    {
        lastNode->valCount++;
    }
    list->listLen++;
    return RLE_LIST_SUCCESS;
}

int RLEListSize(RLEList list)
{
    if(!list)
    {
        return NULL_LIST;
    }
    return list->listLen;
}


RLEListResult RLEListRemove(RLEList list, int index)
{
    index+=1;
    if(list == NULL)
    {
        return RLE_LIST_NULL_ARGUMENT;
    }
    if( (index > list->listLen) || (index <= 0) )
    {
        return RLE_LIST_INDEX_OUT_OF_BOUNDS;
    }

    Node currentNode = list->listHead;
    Node previousNode = NULL;
    int currentApperanceSum = currentNode->valCount;
    while (currentApperanceSum<index)
    {
        previousNode = currentNode;
        currentNode = currentNode->next;
        currentApperanceSum += currentNode->valCount;
    }

    if(currentNode->valCount > 1)
    {
        currentNode->valCount-=1;
    }
    else
    {
        if(currentNode==list->listHead)
        {
            list->listHead=currentNode->next;
        }
        else
        {
            previousNode->next=currentNode->next;

        }
        mergeNodes(list);
        free(currentNode);
    }
    list->listLen--;
    return RLE_LIST_SUCCESS;
}


char RLEListGet(RLEList list, int index, RLEListResult *result)
{
    if(!list)
    {
        resultSaver(result,RLE_LIST_NULL_ARGUMENT);
        return ILLEGAL_DATA;
    }
    if(index<0)
    {
        resultSaver(result,RLE_LIST_INDEX_OUT_OF_BOUNDS);
        return ILLEGAL_DATA;
    }
    else
    {
        Node ptr = list->listHead;
        int indexOfFirstAppearance=0;
        int indexOfLastAppearance=0;
        while(ptr)
        {
            indexOfLastAppearance=indexOfFirstAppearance+(ptr->valCount)-1;
            if (index>=indexOfFirstAppearance && index<=indexOfLastAppearance)
            {
                resultSaver(result,RLE_LIST_SUCCESS);
                return ptr->val;
            }
            indexOfFirstAppearance=indexOfLastAppearance+1;
            ptr=ptr->next;

        }
    }
    resultSaver(result,RLE_LIST_INDEX_OUT_OF_BOUNDS);
    return ILLEGAL_DATA;
}


RLEListResult RLEListMap(RLEList list, MapFunction map_function)
{
    if(map_function==NULL || list==NULL)
    {
        return RLE_LIST_NULL_ARGUMENT;
    }
    if(list->listLen == 0)
    {
        return RLE_LIST_SUCCESS;
    }
    Node currentNode = list->listHead;
    while(currentNode != NULL)
    {
        currentNode->val=map_function(currentNode->val);
        currentNode = currentNode->next;
    }
    mergeNodes(list);
    return RLE_LIST_SUCCESS;
}


char* RLEListExportToString(RLEList list, RLEListResult* result)
{
    if (!list)
    {
        resultSaver(result,RLE_LIST_NULL_ARGUMENT);
        return NULL;
    }
    Node nodePtr = list->listHead;
    int nodesNumber=countNodes(list);
    int digitsNumber=numOfDigits(list);
    char* string = malloc( sizeof(*string)*((2*nodesNumber)+digitsNumber+1));
    if (string==NULL)
    {
        resultSaver(result,RLE_LIST_ERROR);
        return NULL;
    }
    assert(string);
    char* stringPtr=string;
    while (nodePtr)
    {
        int neededSpace=sprintf(stringPtr,"%c%d\n",nodePtr->val,nodePtr->valCount);
        stringPtr=stringPtr+neededSpace;
        nodePtr=nodePtr->next;
    }
    resultSaver(result,RLE_LIST_SUCCESS);
    return string;

}


static int numOfDigits(RLEList list)
{
    int counter=0;
    Node ptr = list->listHead;
    while(ptr)
    {
        int number=ptr->valCount;
        assert(number>0);
        while(number>0)
        {
            counter++;
            number/=10;
        }
        ptr=ptr->next;
    }
    return counter;
}


static void mergeNodes(RLEList list)
{
    Node currentNode = list->listHead;
    while (currentNode != NULL)
    {
        while(currentNode->next!= NULL && currentNode->val==currentNode->next->val)
        {
            currentNode->valCount+=currentNode->next->valCount;
            Node tmp = currentNode->next->next;
            free(currentNode->next);
            currentNode->next = tmp;
        }
        currentNode = currentNode->next;
    }
}


static void resultSaver(RLEListResult *result, RLEListResult result_identifier)
{
    if(result != NULL)
    {
        *result = result_identifier;
    }
}

