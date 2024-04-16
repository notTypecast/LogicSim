#include <iostream>
#include "circuit.hpp"
#include "inputs.hpp"
#include "outputs.hpp"

int main()
{
    logicsim::model::input::Constant t(true);
    logicsim::model::input::Keypad keypad;
    logicsim::model::output::_5in_7SegmentDisplay display;

    display.set_inputs(t, keypad, keypad, keypad, keypad, 0, 0, 1, 2, 3);

    logicsim::model::circuit::Circuit circuit;
    circuit.add_component(t);
    circuit.add_component(keypad);
    circuit.add_component(display);
    circuit.check();

    for (int i = 0; i < 16; ++i)
    {
        keypad.set_key(i);
        circuit.tick();
        std::cout << "Keypad: ";
        for (int j = 3; j >= 0; --j)
        {
            std::cout << keypad.evaluate(j);
        }
        std::cout << std::endl;

        std::cout << "Display: ";
        for (int j = 1; j < 8; ++j)
        {
            std::cout << display.evaluate(j) << " ";
        }
        std::cout << std::endl;
    }

    circuit.write("keypad_display");

    return 0;
}
