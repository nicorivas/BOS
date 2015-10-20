#ifndef PROXY_H
#define	PROXY_H

// We need class methods proxy's, as we want to add them to a simulation object
// (pointers, into std::vectors) that gets called during the event loop. As we
// want every FunctionalEvent to be its own class (to contain the possible
// internal data), then we cannot add the member functions as simple pointers
// (as they are member functions). Thus we need the following, for example:
// Class 'A' is a measurement class;
// void (A::*MEMFUN)(Simulation<3>&) is a pointer to a member function that
// returns void and gets a Simulation reference as argument.
// This can be 'casted' to a std::function as we define the calling operator.

// This would be nice because then we would only need one FunctionProxy
// (as we could make the arguments abstract) but it can't be done in C++11.
// template<typename FunctionEventClass, typename... Args, void (FunctionEventClass::*MemberFunction)(Args...) >

template<unsigned int DIM>
class Simulation;

template<typename Class, void (Class::*MemberFunction)(Simulation<3>&) >
class FunctionalEventProxy {
    Class* functionalEvent;
    
public:
    FunctionalEventProxy(Class* functionEvent) : functionalEvent(functionEvent) { }
    
    void operator()(Simulation<3>& arg) {
        (functionalEvent->*MemberFunction) (arg);
    }
};

template<typename Class, void (Class::*MemberFunction)(Particle<3>&, Particle<3>&, Data<3>&) >
class ParticleInteractionProxy {
    Class* particleInteraction;
    
public:
    ParticleInteractionProxy(Class* functionEvent) : particleInteraction(functionEvent) { }
    
    void operator()(Particle<3>& p1, Particle<3>& p2, Data<3>& data) {
        (particleInteraction->*MemberFunction) (p1, p2, data);
    }
};

template<typename Class, void (Class::*MemberFunction)(Particle<3>&, Wall<3>&, Data<3>&) >
class WallInteractionProxy {
    Class* wallInteraction;
    
public:
    WallInteractionProxy(Class* functionEvent) : wallInteraction(functionEvent) { }
    
    void operator()(Particle<3>& p, Wall<3>& w, Data<3>& data) {
        (wallInteraction->*MemberFunction) (p, w, data);
    }
};

#endif