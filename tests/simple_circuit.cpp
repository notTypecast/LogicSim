#include <iostream>
#include "circuit.hpp"
#include "gates.hpp"
#include "inputs.hpp"
#include "outputs.hpp"

int main()
{
    logicsim::input::Switch sw1(false), sw2(true), sw3(false);
    logicsim::input::Oscillator osc(2, 2);
    logicsim::gate::OR or1(sw1, sw2);
    logicsim::gate::NOT not1(sw3);
    logicsim::gate::AND and2(not1, osc);
    logicsim::gate::XOR xor1(or1, and2);
    logicsim::output::BaseOutput out(xor1);

    logicsim::circuit::Circuit circuit;
    circuit.add_component(sw1);
    circuit.add_component(sw2);
    circuit.add_component(sw3);
    circuit.add_component(osc);
    circuit.add_component(or1);
    circuit.add_component(not1);
    circuit.add_component(and2);
    circuit.add_component(xor1);
    circuit.add_component(out);
    circuit.check();

    for (int i = 0; i < 10; ++i)
    {
        circuit.tick();
        std::cout << "Tick " << i << ": " << out.evaluate() << std::endl;
    }

    circuit.write("simple_circuit");

    return 0;
}
