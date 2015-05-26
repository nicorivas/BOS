#include <physics/Simulation.h>
#include <iostream>

double getTimeSum(const std::vector<Particle<3>>& parts) {
    double sumTime = 0;
    for (const Particle<3>& part : parts) {
        sumTime += part.getTime();
    }
    return sumTime;
}

double getTimeAverage(const std::vector<Particle<3>>& parts) {
    double avg = getTimeSum(parts);
    avg /= parts.size();
    return avg;
}

template<typename... T>
bool assertion(bool& assert, bool assertion, std::string error) {
    if (assertion)
        return true;
    
    assert = false;
    std::cerr << error << std::endl;
    return false;
}

bool testRescale() {
    
    Simulation<3> sim(0.1, 1);
    sim.addParticle({0, {0, 0, 0}, {0, 1, 0}, 1, 1});
    sim.addParticle({0, {1, 0, 0}, {0, 1, 0}, 1, 1});
    double tBefore = sim.getGlobalTime();
    double tBefore_avg = getTimeAverage(sim.getParticles());
    
    std::cerr << std::endl;
    
    sim.rescale();
    
    double tAfter = sim.getGlobalTime();
    double tAfter_avg = getTimeAverage(sim.getParticles());
    
    bool as = true;
    
    assertion(as, tBefore == 0, "[IC] Time doesn't start at 0");
    assertion(as, tBefore_avg == 1, "[IC] Time isn't average 1");
    assertion(as, tBefore + tBefore_avg == tAfter + tAfter_avg, "[EX] Sum doesn't remain constant");
    assertion(as, tAfter == 1, "[EX] Time doesn't end at 1");
    assertion(as, tAfter_avg == 0, "[EX] Time avg not 0!");
    
    if (!as) {
        std::cerr << "Dump:"
                << "\ntBefore     = " << tBefore
                << "\ntBefore_avg = " << tBefore_avg
                << "\ntAfter      = " << tAfter
                << "\ntAfter_avg  = " << tAfter_avg << std::endl;
        for (Particle<3> part : sim.getParticles()) {
            std::cerr << part << std::endl;
        }
        std::cerr << std::endl;
    }
    
    sim.addParticle({0, {1, 0, 0}, {0, 1, 0}, 1, 3});
    tAfter_avg = getTimeAverage(sim.getParticles());
    
    sim.rescale();
    double tSecondAfter = sim.getGlobalTime();
    double tSecondAfter_avg = getTimeAverage(sim.getParticles());
    
    assertion(as, tAfter + tAfter_avg == tSecondAfter + tSecondAfter_avg, "[SC] Sum doesn't remain constant");
    assertion(as, tSecondAfter == 2, "[SC] Time doesn't end at 2");
    assertion(as, tSecondAfter_avg == 0, "[SC] Time avg not 0!");
    
     if (!as) {
        std::cerr << "Dump:"
                << "\ntBefore          = " << tBefore
                << "\ntBefore_avg      = " << tBefore_avg
                << "\ntAfter           = " << tAfter
                << "\ntAfter_avg       = " << tAfter_avg
                << "\ntSecondAfter     = " << tSecondAfter
                << "\ntSecondAfter_avg = " << tSecondAfter_avg << std::endl;

        for (Particle<3> part : sim.getParticles()) {
            std::cerr << part << std::endl;
        }
        std::cerr << std::endl;
    }
    
    return as;
    
}

bool testSynchronize() {
    bool as = true;
    Simulation<3> sim(2, 1);
    sim.addParticle({0, {0, 0, 0}, {0, 1, 0}, 1});
    sim.addParticle({0, {5, 0, 0}, {0, 1, 0}, 1});
    
    size_t funcIdx = sim.addFunction([&as](Simulation<3>& sim) {
        sim.synchronise();
        for (Particle<3> part : sim.getParticles()) {
            assertion(as, sim.getGlobalTime() * 1 == part.getPosition()[1], "PARTICLE NOT MOVED RIGHT");

        }
        if (!as) {
            for (Particle<3> part : sim.getParticles()) {
                std::cerr << part << std::endl;
            }
            std::cerr << std::endl;
        }
        
    });
    
    sim.queueFunction(funcIdx, 0.5);
    sim.queueFunction(funcIdx, 1);
    sim.run();
    
    return as;
}

int main(int argc, char** argv) {
    
    if (!testRescale()) {
        std::cerr << "Rescale failed!" << std::endl;
        return 1;
    }
    
    if (!testSynchronize()) {
        
    }
    
    return 0;
}
