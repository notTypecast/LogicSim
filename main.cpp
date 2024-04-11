#include <iostream>
#include "simulator.hpp"
#include "gates.hpp"
#include "inputs.hpp"
#include "outputs.hpp"
#include "memory.hpp"

int main()
{
    logicsim::input::Switch sw1(true);
    logicsim::input::Oscillator osc(2, 2);

    logicsim::memory::TFlipFlop tlatch(osc, sw1);

    logicsim::output::BaseOutput out(tlatch);

    logicsim::sim::Simulator sim(out);

    sim.check_circuit();

    for (int i = 0; i < 10; ++i)
    {
        sim.tick();
        std::cout << "Tick " << i << ": " << out.evaluate() << ", " << osc.evaluate() << std::endl;
    }

    /*
    logicsim::input::Switch sw1(false), sw2(true), sw3(false);
    logicsim::input::Oscillator osc(2, 2);
    logicsim::gate::OR or1(sw1, sw2);
    logicsim::gate::NOT not1(sw3);
    logicsim::gate::AND and2(not1, osc);
    logicsim::gate::XOR xor1(or1, and2);
    logicsim::output::BaseOutput out(xor1);

    logicsim::sim::Simulator sim(out);

    sim.check_circuit();

    for (int i = 0; i < 10; ++i)
    {
        sim.tick();
        std::cout << "Tick " << i << ": " << out.evaluate() << std::endl;
    }
    */

    return 0;
}