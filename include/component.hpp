#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <unordered_map>
#include <cassert>
#include <type_traits>
#include <exception>

namespace logicsim
{
    namespace component
    {
        class NULLInput : public std::exception
        {
        public:
            const char *what() const noexcept override;
        };

        class Component
        {
        public:
            virtual bool evaluate(unsigned int out = 0);
            void clear(); // recursively checks if inputs are set
            void check() const; // recursively clears cached state of input components

        protected:
            std::unordered_map<unsigned int, bool> _cache;
            virtual bool _evaluate(unsigned int out = 0) = 0;

            // must be overriden if component has inputs
            virtual void _check_inputs() const {}; // implementation must throw NULLInput if input is not set, else call check() on input components
            virtual void _clear_inputs(){}; // implementation must call clear() on input components
        };

        class NInputComponent : public Component
        {
        public:
            NInputComponent(unsigned int n);
            NInputComponent(std::vector<Component> &inputs, std::vector<unsigned int> &inputs_out);

            void set_inputs(std::vector<Component> &inputs, std::vector<unsigned int> &inputs_out);
            void set_input(size_t index, Component &input, unsigned int out = 0);

        protected:
            unsigned int _n;
            std::vector<Component *> _inputs;
            std::vector<unsigned int> _inputs_out;

            void _check_inputs() const override;
            void _clear_inputs() override;
        };

#define DEFINE_1_INPUT_COMPONENT(name)                                    \
    class name : public component::NInputComponent                        \
    {                                                                     \
    public:                                                               \
        name() : NInputComponent(1) {}                                    \
        name(Component &input, unsigned int out = 0) : NInputComponent(1) \
        {                                                                 \
            set_input(0, input, out);                                     \
        }                                                                 \
                                                                          \
    protected:                                                            \
        bool _evaluate(unsigned int = 0) override;                        \
    };

#define DEFINE_2_INPUT_COMPONENT(name)                                                                                \
    class name : public component::NInputComponent                                                                    \
    {                                                                                                                 \
    public:                                                                                                           \
        name() : NInputComponent(2) {}                                                                                \
        name(Component &input1, Component &input2, unsigned int out1 = 0, unsigned int out2 = 0) : NInputComponent(2) \
        {                                                                                                             \
            set_input(0, input1, out1);                                                                               \
            set_input(1, input2, out2);                                                                               \
        }                                                                                                             \
                                                                                                                      \
    protected:                                                                                                        \
        bool _evaluate(unsigned int = 0) override;                                                                    \
    };

        class TimeComponent : public Component
        {
        public:
            bool evaluate(unsigned int out = 0) override;

        protected:
            unsigned long _ticks = -1;
        };
    }
}

#endif // COMPONENT_HPP
