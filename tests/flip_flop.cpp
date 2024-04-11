#include <iostream>
#include "simulator.hpp"
#include "inputs.hpp"
#include "outputs.hpp"
#include "memory.hpp"


int main()
{
    logicsim::input::Switch sw1(true);
    logicsim::input::Oscillator osc(2, 2);

    logicsim::memory::TFlipFlop tflipflop(osc, sw1);

    logicsim::output::BaseOutput out(tflipflop);

    logicsim::sim::Simulator sim(out);

    sim.check_circuit();

    for (int i = 0; i < 10; ++i)
    {
        sim.tick();
        std::cout << "Tick " << i << ": " << out.evaluate() << ", " << osc.evaluate() << std::endl;
    }

    return 0;
}