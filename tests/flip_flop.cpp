#include <iostream>
#include "circuit.hpp"
#include "inputs.hpp"
#include "outputs.hpp"
#include "memory.hpp"

int main()
{
    logicsim::model::input::Switch sw1(true);
    logicsim::model::input::Oscillator osc(2, 2);

    logicsim::model::memory::TFlipFlop tflipflop(osc, sw1);

    logicsim::model::output::BaseOutput out(tflipflop);

    logicsim::model::circuit::Circuit circuit;
    circuit.add_component(sw1);
    circuit.add_component(osc);
    circuit.add_component(tflipflop);
    circuit.add_component(out);

    circuit.check();

    for (int i = 0; i < 10; ++i)
    {
        circuit.tick();
        std::cout << "Tick " << i << ": " << out.evaluate() << ", " << osc.evaluate() << std::endl;
    }

    circuit.write("flip_flop");

    return 0;
}
