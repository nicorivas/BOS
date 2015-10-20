#define DEBUG_MODE
#include <algorithm/PriorityQueue.h>
#undef DEBUG_MODE
#include <queue>
#include <vector>
#include <initializer_list>
#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>

#define PERFTEST_COUNT (1l << 20)

template<typename A, typename B>
bool testQueueEquals(A& a, B& b) {
    
    //If sizes aren't equal, shit is messed up!
    if (a.size() != b.size())
        return false;
    
    std::size_t position = 0;
    while (!a.empty() && !b.empty()) {
        std::cout << position++ << '\t' << a.top() << '\t' << b.top() << '\n';
        if (a.top() != b.top()) {
            return false;
        }
        a.pop();
        b.pop();
    }
    
    
    return true;
}

template<typename A, typename B, typename C>
void fillQueues(A& a, B& b, std::initializer_list<C> c) {
    for (C cElem : c) {
        a.push(cElem);
        b.push(cElem);
    }
}

int main(int argc, char** argv) {
    std::priority_queue<int> a;
    PriorityQueue<int> b;
    
    fillQueues(a,b,{0,21,2,13,9,7,5,3,17,0});
    
    if (!b.validate())
        return 2;
    std::cout << std::endl;
    b.dump();
    
    std::cout << std::endl;
    if (!testQueueEquals(a,b))
        return 1;
    
    b = {0,3,4,7,5};
    if (!b.validate())
        return 3;
    
    b.dump();
    
    std::cout << std::endl;
    
    b.erase(3);
    if (!b.validate())
        return 4;
    
    b.dump();
    std::cout << std::endl;
    
    std::cout << "Starting speed test." << std::endl;
    std::vector<int> vector;
    
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<> dis(-PERFTEST_COUNT,PERFTEST_COUNT);
    
    vector.resize(PERFTEST_COUNT);
    for (int& i : vector) {
        i = dis(gen);
    }
    
    a = std::priority_queue<int>();
    b = PriorityQueue<int>();
    
    auto s1 = std::chrono::high_resolution_clock::now();
    for (int i : vector) {
        a.push(i);
    }
    auto s2 = std::chrono::high_resolution_clock::now();
    for (int i : vector) {
        b.push(i);
    }
    auto s3 = std::chrono::high_resolution_clock::now();
    while (!a.empty())
        a.pop();
    auto s4 = std::chrono::high_resolution_clock::now();
    while (!b.empty())
        b.pop();
    auto s5 = std::chrono::high_resolution_clock::now();
    
    std::cout << "Timing report:\n";
    std::chrono::milliseconds insA = std::chrono::duration_cast<std::chrono::milliseconds>(s2 - s1);
    std::chrono::milliseconds insB = std::chrono::duration_cast<std::chrono::milliseconds>(s3 - s2);
    std::chrono::milliseconds popA = std::chrono::duration_cast<std::chrono::milliseconds>(s4 - s3);
    std::chrono::milliseconds popB = std::chrono::duration_cast<std::chrono::milliseconds>(s5 - s4);
    std::cout << "     Insertion:\t    Deletion:\t    Total:\n";
    std::cout << "A: " << std::setw(10) << insA.count() << "ms\t"
                       << std::setw(10) << popA.count() << "ms\t"
                       << std::setw(10) << (insA + popA).count() << "ms\n";
    std::cout << "B: " << std::setw(10) << insB.count() << "ms\t"
                       << std::setw(10) << popB.count() << "ms\t"
                       << std::setw(10) << (insB + popB).count() << "ms\n";
    std::cout << std::endl;
    return 0;
}

