/* 
 * File:   FunctionProxy.h
 * Author: nrivas
 *
 * Created on June 15, 2015, 1:04 PM
 */

#ifndef FUNCTIONPROXY_H
#define	FUNCTIONPROXY_H

// We need a function proxy, as we want to add to a simulation object function
// (pointers, into std::vectors) that get called during the event loop. As we
// want every function event to be its own class (to contain the possible
// internal data), then we cannot add the member functions of the class (as they
// are member functions). Thus we need the following:
// Class 'A' is a measurement class;
// void (A::*MEMFUN)(Simulation<3>&) is a pointer to a member function that
// returns void and gets a Simulation reference as argument.
// This can be 'casted' to a std::function as we define the calling operator.
template<typename FunctionEventClass, void (FunctionEventClass::*MemberFunction)(Simulation<3>&) >
class FunctionProxy {
    FunctionEventClass* functionEvent;
    
public:
    FunctionProxy(FunctionEventClass* functionEvent) : functionEvent(functionEvent) { }
    
    void operator()(Simulation<3>& sim) {
        (functionEvent->*MemberFunction) (sim);
    }
};

template<typename FunctionEventClass, typename Return, typename... Args, Return (FunctionEventClass::*MemberFunction)(Args...) >
class FunctionProxy2 {
    FunctionEventClass* functionEvent;
    
public:
    FunctionProxy(FunctionEventClass* functionEvent) : functionEvent(functionEvent) { }
    
    Return operator()(Args... args) {
        (functionEvent->*MemberFunction) (args...);
    }
};
#endif	/* FUNCTIONPROXY_H */

