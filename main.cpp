#include <iostream>
#include "simulator.hpp"
#include "gates.hpp"
#include "inputs.hpp"

int main()
{
    logicsim::input::Switch sw1(false), sw2(true), sw3(false), sw4(true);
    logicsim::gate::OR or1(sw1, sw2);
    logicsim::gate::NOT not1(sw3);
    logicsim::gate::AND and2(not1, sw4), and3(or1, and2);

    logicsim::sim::Simulator sim(and3);

    for (int i = 0; i < 10; ++i)
    {
        sim.tick();
    }

    return 0;
}