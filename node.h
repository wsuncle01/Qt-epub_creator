#ifndef NODE_H
#define NODE_H

#include <QObject>

template <class T>
class Node
{
    //data
private:
    T data;
    Node<T> *next;
public:
    explicit Node(T header);
    void setNext(Node<T> *next);
    Node<T>* getNext();
};

template <class T>
Node<T>::Node(T header)
{
    data=header;
    next=nullptr;
}

template<class T>
void Node<T>::setNext(Node<T> *nextptr)
{
    next=nextptr;
}

template<class T>
Node<T>* Node<T>::getNext()
{
    return next;
}

#endif // NODE_H
