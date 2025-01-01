#ifndef LOGICSIM_MODEL_CTYPE_MAP_HPP
#define LOGICSIM_MODEL_CTYPE_MAP_HPP

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <functional>

#include "model/component.hpp"
#include "model/gates.hpp"
#include "model/inputs.hpp"
#include "model/memory.hpp"
#include "model/outputs.hpp"
#include "model/control.hpp"

namespace logicsim
{
    namespace model
    {
    #define DEFINE_FACTORY_FUNCTION(name, class_t)   \
        inline component::Component *create_##name() \
        {                                            \
            return new class_t();                    \
        }
    #define DEFINE_FACTORY_FUNCTION_ARG(name, class_t, arg) \
        inline component::Component *create_##name()        \
        {                                                   \
            return new class_t(arg);                        \
        }

        DEFINE_FACTORY_FUNCTION(and, gate::AND)
        DEFINE_FACTORY_FUNCTION(or, gate::OR)
        DEFINE_FACTORY_FUNCTION(xor, gate::XOR)
        DEFINE_FACTORY_FUNCTION(nand, gate::NAND)
        DEFINE_FACTORY_FUNCTION(nor, gate::NOR)
        DEFINE_FACTORY_FUNCTION(xnor, gate::XNOR)
        DEFINE_FACTORY_FUNCTION(not, gate::NOT)
        DEFINE_FACTORY_FUNCTION(buffer, gate::BUFFER)
        DEFINE_FACTORY_FUNCTION(connector, gate::CONNECTOR)

        DEFINE_FACTORY_FUNCTION(constant, input::Constant)
        DEFINE_FACTORY_FUNCTION(button, input::Button)
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

        DEFINE_FACTORY_FUNCTION_ARG(mux1, control::Multiplexer, 1)
        DEFINE_FACTORY_FUNCTION_ARG(mux2, control::Multiplexer, 2)
        DEFINE_FACTORY_FUNCTION_ARG(mux3, control::Multiplexer, 3)

        DEFINE_FACTORY_FUNCTION_ARG(dec1, control::Decoder, 1)
        DEFINE_FACTORY_FUNCTION_ARG(dec2, control::Decoder, 2)
        DEFINE_FACTORY_FUNCTION_ARG(dec3, control::Decoder, 3)

        const std::unordered_map<std::string, std::function<component::Component *()>> ctype_map = {
            {"AND", &create_and},
            {"OR", &create_or},
            {"XOR", &create_xor},
            {"NAND", &create_nand},
            {"NOR", &create_nor},
            {"XNOR", &create_xnor},
            {"NOT", &create_not},
            {"CONNECTOR", &create_connector},
            {"BUFFER", &create_buffer},
            {"CONSTANT", &create_constant},
            {"BUTTON", &create_button},
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
            {"8IN_7SEGMENT", &create_8in_7segment},
            {"MUX-1", &create_mux1},
            {"MUX-2", &create_mux2},
            {"MUX-3", &create_mux3},
            {"DEC-1", &create_dec1},
            {"DEC-2", &create_dec2},
            {"DEC-3", &create_dec3}
        };
    }
}

#endif // CTYPE_MAP_HPP
