#ifndef __MCE_HEAP_H__
#define __MCE_HEAP_H__

// ----------------------------------------------------------------------------

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------------

template<typename DataT>
class MinHeap
{
    typedef DataT* DataPtr;
public:
    MinHeap(const int n);
    ~MinHeap()
    {
        free(_heapArray);
        _heapArray = 0;
    }

    inline bool isEmpty();

    inline bool isLeaf(int pos) const;

    inline int leftChild(int pos) const;

    inline int rightChild(int pos) const;

    inline int parent(int pos) const;

    inline bool remove(int pos, DataPtr * node);

    inline bool insert(DataT * newNode);

    inline DataT* removeMin();

    inline DataT* min();

    inline void siftUp(int pos);

    inline void siftDown(int left);

    inline int size()
    {
        return _size;
    }

    inline int capacity()
    {
        return _capacity;
    }

private:
    DataPtr * _heapArray;
    int _size;
    int _capacity;
    int _increment;
    void _swap(int posX, int posY);
    void _buildHeap();
};

// ----------------------------------------------------------------------------

template<typename DataT>
bool MinHeap<DataT>::isEmpty ()
{
    return 0 == _size;
}

// ----------------------------------------------------------------------------

template<typename DataT>
void MinHeap<DataT>::_swap(int posX, int posY)
{
    DataT *temp = 0;
    temp = _heapArray[posX];
    _heapArray[posX] = _heapArray[posY];
    _heapArray[posY] = temp;
    _heapArray[posX]->pos(posX);
    _heapArray[posY]->pos(posY);
}

// ----------------------------------------------------------------------------

template<typename DataT>
MinHeap<DataT>::MinHeap(const int n)
{
    if (n <= 0) {
        return;
    }

    _size = 0;
    _increment = n;
    _capacity = _increment;
    //heapArray = new DATA_P[m_capacity];
    _heapArray = (DataPtr *) malloc(_capacity * sizeof(DataPtr));
    if (_heapArray == 0) {
        throw;
    }
    _buildHeap();
}

// ----------------------------------------------------------------------------

template<typename DataT>
bool MinHeap<DataT>::isLeaf(int pos) const
{
    return (pos >= _size / 2) && (pos < _size);
}

// ----------------------------------------------------------------------------

template<typename DataT>
void MinHeap<DataT>::_buildHeap()
{
    for (int i = _size / 2 - 1; i >= 0; i--) {
        siftDown(i);
    }
}

// ----------------------------------------------------------------------------

template<typename DataT>
int MinHeap<DataT>::leftChild(int pos) const
{
    return 2 * pos + 1;
}

// ----------------------------------------------------------------------------

template<typename DataT>
int MinHeap<DataT>::rightChild(int pos) const
{
    return 2 * pos + 2;
}

// ----------------------------------------------------------------------------

template<typename DataT>
int MinHeap<DataT>::parent(int pos) const
{
    return (pos - 1) / 2;
}

// ----------------------------------------------------------------------------

template<typename DataT>
bool MinHeap<DataT>::insert(DataT* newNode)
{
    if (_size == _capacity) {
        int isize = _capacity + _increment;
        _heapArray = (DataPtr *) realloc(_heapArray, isize * sizeof(DataPtr));
        if (_heapArray == 0) {
            return false;
        }

        _capacity = isize;
    }

    _heapArray[_size] = newNode;
    _heapArray[_size]->pos(_size);

    siftUp(_size);
    _size++;
    return true;
}

// ----------------------------------------------------------------------------

template<typename DataT>
DataT* MinHeap<DataT>::min()
{
    if (_size == 0) {
//        fprintf(stderr, "can't RemoveMin!\n");
        return 0;
    }
    else {
        return _heapArray[0];
    }

    return 0;
}

// ----------------------------------------------------------------------------

template<typename DataT>
DataT* MinHeap<DataT>::removeMin()
{
    if (_size == 0) {
//        fprintf(stderr, "can't RemoveMin!\n");
        return 0;
    }
    else {
        _swap(0, --_size);
        if (_size > 1) {
            siftDown(0);
        }

        return _heapArray[_size];
    }

    return 0;
}

// ----------------------------------------------------------------------------

template<typename DataT>
bool MinHeap<DataT>::remove(int pos, DataPtr * node)
{
    if ((pos < 0) || (pos >= _size)) {
        return false;
    }
    *node = _heapArray[pos];

    _heapArray[pos] = _heapArray[--_size];
    _heapArray[pos]->pos(pos);

    if (*_heapArray[parent(pos)] > *_heapArray[pos]) {
        siftUp(pos);
    }
    else {
        siftDown(pos);
    }

    return true;
}

// ----------------------------------------------------------------------------

template<typename DataT>
void MinHeap<DataT>::siftUp(int pos)
{
    int temppos = pos;
    DataT *temp = _heapArray[temppos];
    while ((temppos > 0) && ((*(_heapArray[parent(temppos)])) > (*temp))) {

        _heapArray[temppos] = _heapArray[parent(temppos)];
        _heapArray[temppos]->pos(temppos);

        temppos = parent(temppos);
    }

    _heapArray[temppos] = temp;
    _heapArray[temppos]->pos(temppos);
}

// ----------------------------------------------------------------------------

template<typename DataT>
void MinHeap<DataT>::siftDown(int left)
{
    int i = left;
    int j = leftChild(i);
    DataT *temp = _heapArray[i];
    while (j < _size) {
        if ((j < _size - 1) && (*(_heapArray[j]) > *(_heapArray[j + 1]))) {
            j++;
        }

        if (*temp > *_heapArray[j]) {
            _heapArray[i] = _heapArray[j];
            _heapArray[i]->pos(i);
            i = j;
            j = leftChild(j);
        }
        else
            break;
    }
    _heapArray[i] = temp;
    _heapArray[i]->pos(i);
}

// ----------------------------------------------------------------------------

#endif /* __MCE_HEAP_H__ */

