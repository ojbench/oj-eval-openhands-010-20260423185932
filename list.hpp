#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;
        
        node() : data(nullptr), prev(nullptr), next(nullptr) {}
        
        node(const T &value) : data(new T(value)), prev(nullptr), next(nullptr) {}
        
        ~node() {
            if (data) {
                delete data;
                data = nullptr;
            }
        }
    };

protected:
    node *head;
    node *tail;
    size_t currentSize;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        currentSize++;
        return cur;
    }
    
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        node *ret = pos->next;
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        currentSize--;
        return ret;
    }

public:
    class const_iterator;
    class iterator {
    private:
        node *current;
        list *container;
        
        iterator(node *p, list *c) : current(p), container(c) {}

    public:
        friend class list;
        friend class const_iterator;
        
        iterator() : current(nullptr), container(nullptr) {}
        
        /**
         * iter++
         */
        iterator operator++(int) {
            if (!current || !container || current == container->tail) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            current = current->next;
            return tmp;
        }
        
        /**
         * ++iter
         */
        iterator & operator++() {
            if (!current || !container || current == container->tail) {
                throw invalid_iterator();
            }
            current = current->next;
            return *this;
        }
        
        /**
         * iter--
         */
        iterator operator--(int) {
            if (!current || !container || current == container->head->next) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            current = current->prev;
            return tmp;
        }
        
        /**
         * --iter
         */
        iterator & operator--() {
            if (!current || !container || current == container->head->next) {
                throw invalid_iterator();
            }
            current = current->prev;
            return *this;
        }
        
        /**
         * *it
         */
        T & operator *() const {
            if (!current || !container || current == container->tail || !current->data) {
                throw invalid_iterator();
            }
            return *(current->data);
        }
        
        /**
         * it->field
         */
        T * operator ->() const {
            if (!current || !container || current == container->tail || !current->data) {
                throw invalid_iterator();
            }
            return current->data;
        }
        
        bool operator==(const iterator &rhs) const {
            return current == rhs.current && container == rhs.container;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return current == rhs.current && container == rhs.container;
        }
        
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };
    
    class const_iterator {
    private:
        node *current;
        const list *container;
        
        const_iterator(node *p, const list *c) : current(p), container(c) {}

    public:
        friend class list;
        friend class iterator;
        
        const_iterator() : current(nullptr), container(nullptr) {}
        
        const_iterator(const iterator &other) : current(other.current), container(other.container) {}
        
        /**
         * iter++
         */
        const_iterator operator++(int) {
            if (!current || !container || current == container->tail) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            current = current->next;
            return tmp;
        }
        
        /**
         * ++iter
         */
        const_iterator & operator++() {
            if (!current || !container || current == container->tail) {
                throw invalid_iterator();
            }
            current = current->next;
            return *this;
        }
        
        /**
         * iter--
         */
        const_iterator operator--(int) {
            if (!current || !container || current == container->head->next) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            current = current->prev;
            return tmp;
        }
        
        /**
         * --iter
         */
        const_iterator & operator--() {
            if (!current || !container || current == container->head->next) {
                throw invalid_iterator();
            }
            current = current->prev;
            return *this;
        }
        
        /**
         * *it
         */
        const T & operator *() const {
            if (!current || !container || current == container->tail || !current->data) {
                throw invalid_iterator();
            }
            return *(current->data);
        }
        
        /**
         * it->field
         */
        const T * operator ->() const {
            if (!current || !container || current == container->tail || !current->data) {
                throw invalid_iterator();
            }
            return current->data;
        }
        
        bool operator==(const iterator &rhs) const {
            return current == rhs.current && container == rhs.container;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return current == rhs.current && container == rhs.container;
        }
        
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };
    
    /**
     * Constructs
     */
    list() {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        currentSize = 0;
    }
    
    list(const list &other) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        currentSize = 0;
        
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }
    
    /**
     * Destructor
     */
    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }
    
    /**
     * Assignment operator
     */
    list &operator=(const list &other) {
        if (this != &other) {
            clear();
            for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
                push_back(*it);
            }
        }
        return *this;
    }
    
    /**
     * access the first / last element
     */
    const T & front() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(head->next->data);
    }
    
    const T & back() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(tail->prev->data);
    }
    
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return iterator(head->next, this);
    }
    
    const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }
    
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(tail, this);
    }
    
    const_iterator cend() const {
        return const_iterator(tail, this);
    }
    
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const {
        return currentSize == 0;
    }
    
    /**
     * returns the number of elements
     */
    virtual size_t size() const {
        return currentSize;
    }
    
    /**
     * clears the contents
     */
    virtual void clear() {
        while (!empty()) {
            pop_front();
        }
    }
    
    /**
     * insert value before pos
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) {
            throw invalid_iterator();
        }
        node *newNode = new node(value);
        insert(pos.current, newNode);
        return iterator(newNode, this);
    }
    
    /**
     * remove the element at pos
     */
    virtual iterator erase(iterator pos) {
        if (pos.container != this || empty() || pos.current == tail) {
            throw invalid_iterator();
        }
        node *nextNode = erase(pos.current);
        delete pos.current;
        return iterator(nextNode, this);
    }
    
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        insert(end(), value);
    }
    
    /**
     * removes the last element
     */
    void pop_back() {
        if (empty()) {
            throw container_is_empty();
        }
        erase(--end());
    }
    
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        insert(begin(), value);
    }
    
    /**
     * removes the first element.
     */
    void pop_front() {
        if (empty()) {
            throw container_is_empty();
        }
        erase(begin());
    }
    
    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (size() <= 1) return;
        
        T **array = new T*[size()];
        int index = 0;
        for (iterator it = begin(); it != end(); ++it) {
            array[index++] = &(*it);
        }
        
        sjtu::sort<T*>(array, array + size(), [](const T *a, const T *b) {
            return *a < *b;
        });
        
        list<T> tempList;
        for (int i = 0; i < size(); ++i) {
            tempList.push_back(*array[i]);
        }
        
        *this = tempList;
        delete[] array;
    }
    
    /**
     * merge two sorted lists into one
     */
    void merge(list &other) {
        if (this == &other) return;
        
        node *p1 = head->next;
        node *p2 = other.head->next;
        
        while (p1 != tail && p2 != other.tail) {
            if (*(p2->data) < *(p1->data)) {
                node *nextP2 = p2->next;
                other.erase(p2);
                insert(p1, p2);
                p2 = nextP2;
            } else {
                p1 = p1->next;
            }
        }
        
        while (p2 != other.tail) {
            node *nextP2 = p2->next;
            other.erase(p2);
            insert(tail, p2);
            p2 = nextP2;
        }
    }
    
    /**
     * reverse the order of the elements
     */
    void reverse() {
        if (size() <= 1) return;
        
        node *current = head;
        while (current != nullptr) {
            node *temp = current->next;
            current->next = current->prev;
            current->prev = temp;
            current = temp;
        }
        
        node *temp = head;
        head = tail;
        tail = temp;
    }
    
    /**
     * remove all consecutive duplicate elements
     */
    void unique() {
        if (size() <= 1) return;
        
        node *current = head->next;
        while (current != tail && current->next != tail) {
            if (*(current->data) == *(current->next->data)) {
                node *toDelete = current->next;
                erase(toDelete);
                delete toDelete;
            } else {
                current = current->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
