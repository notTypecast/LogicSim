#include <iostream>
#include "circuit.hpp"
#include "component.hpp"

int main()
{
    logicsim::model::circuit::Circuit circuit;
    circuit.read("saves/simple_circuit.lsc");
    circuit.check();

    logicsim::model::component::Component &out = circuit.get_active_component(1);

    for (int i = 0; i < 10; ++i)
    {
        circuit.tick();
        std::cout << "Tick " << i << ": " << out.evaluate() << std::endl;
    }

    return 0;
}
