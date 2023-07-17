#ifndef FILELIST_H
#define FILELIST_H

#include <QObject>
#include "node.h"
template <class T>
class Filelist:public QObject
{
//data
private:
    Node<T> *headnode;
public:
    explicit Filelist(QObject *parent);
    void add(T data);
    void del(T data);
    Node<T> *getHead();
};

template <class T>
Filelist<T>::Filelist(QObject *parent):QObject(parent)
{
    headnode=new Node<T>;
}

template<class T>
void Filelist<T>::add(T data)
{
    Node<T> *cur=headnode;
    Node<T> *newdata=new Node<T>(data);
    while(cur->getNext()!=nullptr){
        cur=cur->getNext();
    }
    cur->setNext(newdata);
}

template<class T>
void Filelist<T>::del(T data)
{
    Node<T> *cur=headnode->getNext();
    Node<T> *pre=headnode;
    while(cur!=nullptr){
        if(cur->getdata()==data){
            pre->setNext(cur->getNext());
            delete cur;
            break;
        }
        pre=cur;
        cur=cur->getNext();
    }
}

template<class T>
Node<T> *Filelist<T>::getHead()
{
    return headnode;
}

#endif // FILELIST_H
