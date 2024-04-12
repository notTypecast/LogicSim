#ifndef CTYPE_MAP_HPP
#define CTYPE_MAP_HPP

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <functional>
#include "component.hpp"
#include "gates.hpp"
#include "inputs.hpp"
#include "memory.hpp"
#include "outputs.hpp"

namespace logicsim
{
    // ctypes of active components
    const std::unordered_set<std::string> ACTIVE_COMPONENTS = {
        "OSCILLATOR",
        "SRLATCH",
        "JKLATCH",
        "DLATCH",
        "TLATCH",
        "SRFLIPFLOP",
        "JKFLIPFLOP",
        "DFLIPFLOP",
        "TFLIPFLOP",
        "OUTPUT",
        "5IN_7SEGMENT",
        "8IN_7SEGMENT"};

#define DEFINE_FACTORY_FUNCTION(name, class_t)   \
    inline component::Component *create_##name() \
    {                                            \
        return new class_t();                    \
    }

    DEFINE_FACTORY_FUNCTION(and, gate::AND)
    DEFINE_FACTORY_FUNCTION(or, gate::OR)
    DEFINE_FACTORY_FUNCTION(xor, gate::XOR)
    DEFINE_FACTORY_FUNCTION(nand, gate::NAND)
    DEFINE_FACTORY_FUNCTION(nor, gate::NOR)
    DEFINE_FACTORY_FUNCTION(xnor, gate::XNOR)
    DEFINE_FACTORY_FUNCTION(not, gate::NOT)

    DEFINE_FACTORY_FUNCTION(constant, input::Constant)
    DEFINE_FACTORY_FUNCTION(switch, input::Switch)
    DEFINE_FACTORY_FUNCTION(oscillator, input::Oscillator)
    DEFINE_FACTORY_FUNCTION(keypad, input::Keypad)

    DEFINE_FACTORY_FUNCTION(srlatch, memory::SRLatch)
    DEFINE_FACTORY_FUNCTION(jklatch, memory::JKLatch)
    DEFINE_FACTORY_FUNCTION(dlatch, memory::DLatch)
    DEFINE_FACTORY_FUNCTION(tlatch, memory::TLatch)
    DEFINE_FACTORY_FUNCTION(srflipflop, memory::SRFlipFlop)
    DEFINE_FACTORY_FUNCTION(jkflipflop, memory::JKFlipFlop)
    DEFINE_FACTORY_FUNCTION(dflipflop, memory::DFlipFlop)
    DEFINE_FACTORY_FUNCTION(tflipflop, memory::TFlipFlop)

    DEFINE_FACTORY_FUNCTION(output, output::BaseOutput)
    DEFINE_FACTORY_FUNCTION(5in_7segment, output::_5in_7SegmentDisplay)
    DEFINE_FACTORY_FUNCTION(8in_7segment, output::_8in_7SegmentDisplay)

    const std::unordered_map<std::string, std::function<component::Component *()>> ctype_map = {
        {"AND", &create_and},
        {"OR", &create_or},
        {"XOR", &create_xor},
        {"NAND", &create_nand},
        {"NOR", &create_nor},
        {"XNOR", &create_xnor},
        {"NOT", &create_not},
        {"CONSTANT", &create_constant},
        {"SWITCH", &create_switch},
        {"OSCILLATOR", &create_oscillator},
        {"KEYPAD", &create_keypad},
        {"SRLATCH", &create_srlatch},
        {"JKLATCH", &create_jklatch},
        {"DLATCH", &create_dlatch},
        {"TLATCH", &create_tlatch},
        {"SRFLIPFLOP", &create_srflipflop},
        {"JKFLIPFLOP", &create_jkflipflop},
        {"DFLIPFLOP", &create_dflipflop},
        {"TFLIPFLOP", &create_tflipflop},
        {"OUTPUT", &create_output},
        {"5IN_7SEGMENT", &create_5in_7segment},
        {"8IN_7SEGMENT", &create_8in_7segment}};
}

#endif // CTYPE_MAP_HPP
