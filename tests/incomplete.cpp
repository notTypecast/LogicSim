#include <iostream>
#include "circuit.hpp"
#include "gates.hpp"
#include "inputs.hpp"
#include "outputs.hpp"

int main()
{
    logicsim::model::input::Switch sw1(false);

    logicsim::model::gate::NAND nand;
    nand.set_input(1, sw1);

    logicsim::model::output::_8in_7SegmentDisplay display;
    display.set_input(3, nand, 0);

    logicsim::model::circuit::Circuit circuit;
    circuit.add_component(sw1);
    circuit.add_component(nand);
    circuit.add_component(display);

    circuit.write("incomplete");

    return 0;
}
