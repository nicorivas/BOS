/* 
 * File:   PriorityQueue.h
 * Author: dducks
 *
 * Created on April 21, 2015, 7:45 PM
 */

#ifndef PRIORITYQUEUE_H
#define	PRIORITYQUEUE_H

#include <vector>
#include <functional>

#define DEBUG_MODE
#ifdef DEBUG_MODE
#include <iostream>
#endif

template<typename T, typename C = std::vector<T>, typename L = std::less<T>>
class PriorityQueue {
private:
    C container;
    L compare;
    using value_type = T;
    
public:
    PriorityQueue() {
        
    }
    
    PriorityQueue(std::initializer_list<T> t) {
        for (T elem : t)
            push(elem);
    }
        
    /**
     * Returns the top of the priority queue
     * @return top of the queue
     */
    T& top() {
        return container.front();
    }
    
    
    /**
     * Returns the top of the priority queue
     * @return top of the queue
     */
    const T& top() const {
        return container.front();
    }
    
    /**
     * Removes the top element from the priority queue
     */
    void pop() {
        std::swap(container.back(), container.front());
        //now, the top is at the end
        //pop the last, and sort the first
        
        container.pop_back();
        sortDown(0);
    }
    
    /**
     * Adds a new element and sorts the queue.
     * If the capacity of the container is exceeded, any references to the first
     * element will be invalidated
     * @param t the value to insert into the queue
     */
    void push(const T& t) {
        container.push_back(std::move(t));
        sortUp(container.size() - 1);
    }
    
    /**
     * Adds a new element and sorts the queue.
     * If the capacity of the container is exceeded, any references to the first
     * element will be invalidated
     * @param t the value to insert into the queue
     */
    void push(T&& t) {
        container.push_back(std::move(t));
        sortUp(container.size() - 1);
    }
    
    template<typename... ARGS>
    void emplace(ARGS&&... args) {
        container.emplace_back(args...);
        sortUp(container.size() - 1);
    }
    
    /**
     * Erases the value 'value' from the container, resorting
     * This can of course only be done if the equals method is implemented
     * for the given type.
     * @param value The value to remove
     * @return the amount of values remove
     */
    bool erase(const value_type& value) {
        std::size_t valueCount = 0;
        for (std::size_t i = 0; i < (container.size() - 1); i++) {
            if (container[i] == value) {
                std::swap(container[i], container.back());
                
                container.pop_back();
                valueCount++;
                
                sortUp(i);
                sortDown(i);
                
                //SORT LOCAL
            }
        }
        if (value == container.back()) {
            container.pop_back();
            valueCount++;
        }
        return valueCount;
    }
    
    /**
     * @brief reserves space
     * Reserves space at the underlying container
     * In case count is bigger than it's current size,
     * this will enlarge the container, potentially invalidating any references
     * to the top
     * 
     * In case it's smaller than the current size, nothing will happen
     * @param count
     */
    void reserve(std::size_t count) {
        container.reserve(count);
    }
    
    std::size_t capacity() const {
        return container.capacity();
    }
    
    /**
     * Returns true if the container is empty
     * @return 
     */
    bool empty() const {
        return container.empty();
    }
    
    /**
     * Clears the container
     */
    void clear() {
        container.clear();
    }
    
    std::size_t size() const {
        return container.size();
    }

#ifdef DEBUG_MODE    
    template<typename Func>
    void eval(Func func) {
        for (T& t : container) {
            func(t);
        }
    }
    
    void dump() const {
        for (const T& t : container) {
            std::cout << t << '\n';
        }
        std::cout << std::endl;
    }
    
    std::vector<T> getlist() const {
        std::vector<T> vec; 
        for (const T& t : container) {
            vec.push_back(t);
        }
        return vec;
    }
    
    bool validate() const {
        for (std::size_t i = 1; i < container.size(); i++) {
            if (!compare(container[i], container[(i-1)/2]))
                return false;
        }
        return true;
    }
#endif
private:
    void sortUp(std::size_t idx) {
        //The root can't be sorted up.
        if (idx == 0)
            return;
        
        std::size_t parent = (idx-1) / 2;
        //While our parent is bigger than our child
        while (compare(container[parent], container[idx])) {
            std::swap(container[parent],container[idx]);
            idx = parent;
            
            if (parent == 0)
                return;
            parent = (idx-1) / 2;
        }
    }
    
    void sortDown(std::size_t idx) {
        std::size_t cLeft, cRight;
        while(true) {
            cRight = (idx + 1)*2;
            cLeft = cRight - 1;
            if (cLeft >= container.size())
                return; //We reached the end of the tree
            if (cRight == container.size()) {
                if (!compare(container[cLeft],container[idx])) {
                    std::swap(container[idx],container[cLeft]);
                }
                return;
            }
            
            bool biggest = compare(container[cRight],container[cLeft]);
            std::size_t cBiggest = biggest ? cLeft : cRight;
            if (!compare(container[cBiggest], container[idx])) {
                std::swap(container[cBiggest], container[idx]);
                idx = cBiggest;
            } else {
                return;
            }
        }
    }
};


#endif	/* PRIORITYQUEUE_H */

