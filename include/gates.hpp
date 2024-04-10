#ifndef GATES_HPP
#define GATES_HPP

#include <memory>
#include <cassert>
#include <type_traits>
#include "component.hpp"

#define DEFINE_2_INPUT_GATE(name)                                                 \
    class name : public _2InputGate                                               \
    {                                                                             \
    public:                                                                       \
        name() {}                                                                 \
        template <typename T1, typename T2>                                       \
        name(const T1 &input1, const T2 &input2) : _2InputGate(input1, input2) {} \
                                                                                  \
        bool evaluate() const override;                                           \
    };

#define DEFINE_1_INPUT_GATE(name)                    \
    class name : public _1InputGate                  \
    {                                                \
    public:                                          \
        name() {}                                    \
        template <typename T>                        \
        name(const T &input) : _1InputGate(input) {} \
                                                     \
        bool evaluate() const override;              \
    };

namespace logicsim
{
    namespace gate
    {
        class _2InputGate : public Component
        {
        public:
            _2InputGate() {}
            template <typename T1, typename T2>
            _2InputGate(const T1 &input1, const T2 &input2)
            {
                assert((std::is_base_of<Component, T1>::value) && (std::is_base_of<Component, T2>::value) && "Inputs must be Circuit objects");
                _input1 = std::make_shared<T1>(input1);
                _input2 = std::make_shared<T2>(input2);
            }

        protected:
            std::shared_ptr<Component> _input1;
            std::shared_ptr<Component> _input2;

            inline void _check_inputs() const
            {
                if (!_input1 || !_input2)
                {
                    throw NULLInput();
                }
            }
        };

        class _1InputGate : public Component
        {
        public:
            _1InputGate() {}
            template <typename T>
            _1InputGate(const T &input)
            {
                assert((std::is_base_of<Component, T>::value) && "Input must be a Circuit object");
                _input = std::make_shared<T>(input);
            }

        protected:
            std::shared_ptr<Component> _input;

            inline void _check_input() const
            {
                if (!_input)
                {
                    throw NULLInput();
                }
            }
        };

        DEFINE_2_INPUT_GATE(AND)
        DEFINE_2_INPUT_GATE(OR)
        DEFINE_2_INPUT_GATE(XOR)
        DEFINE_2_INPUT_GATE(NAND)
        DEFINE_2_INPUT_GATE(NOR)
        DEFINE_2_INPUT_GATE(XNOR)

        DEFINE_1_INPUT_GATE(NOT)
    }
}

#endif // GATES_HPP
