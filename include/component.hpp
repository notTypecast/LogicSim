#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <cassert>
#include <type_traits>
#include <exception>

namespace logicsim
{
    class NULLInput : public std::exception
    {
    public:
        const char *what() const noexcept override;
    };

    class Component
    {
    public:
        virtual bool evaluate();
        void clear();
        void check() const;

        int tmp = 0;

    protected:
        bool _output, _cached = false;
        virtual bool _evaluate() = 0;

        // must be overriden if component has inputs
        virtual void _check_inputs() const {};
        virtual void _clear_inputs() const {};
    };

    class _2InputComponent : public Component
    {
    public:
        _2InputComponent() {}
        template <typename T1, typename T2>
        _2InputComponent(T1 &input1, T2 &input2)
        {
            set_inputs(input1, input2);
        }

        template <typename T1, typename T2>
        void set_inputs(T1 &input1, T2 &input2)
        {
            assert((std::is_base_of<Component, T1>::value) && (std::is_base_of<Component, T2>::value) && "Inputs must be Circuit objects");
            set_input1(input1);
            set_input2(input2);
        }

        template <typename T>
        void set_input1(T &input)
        {
            assert((std::is_base_of<Component, T>::value) && "Input must be a Circuit object");
            _input1 = &input;
        }

        template <typename T>
        void set_input2(T &input)
        {
            assert((std::is_base_of<Component, T>::value) && "Input must be a Circuit object");
            _input2 = &input;
        }

    protected:
        Component *_input1;
        Component *_input2;

        void _check_inputs() const override;

        void _clear_inputs() const override;
    };

    class _1InputComponent : public Component
    {
    public:
        _1InputComponent() {}
        template <typename T>
        _1InputComponent(T &input)
        {
            set_input(input);
        }

        template <typename T>
        void set_input(T &input)
        {
            assert((std::is_base_of<Component, T>::value) && "Input must be a Circuit object");
            _input = &input;
        }

    protected:
        Component *_input;

        void _check_inputs() const override;

        void _clear_inputs() const override;
    };

#define DEFINE_2_INPUT_COMPONENT(name)                                     \
    class name : public _2InputComponent                                   \
    {                                                                      \
    public:                                                                \
        name() {}                                                          \
        template <typename T1, typename T2>                                \
        name(T1 &input1, T2 &input2) : _2InputComponent(input1, input2) {} \
                                                                           \
    protected:                                                             \
        bool _evaluate() override;                                         \
    };

#define DEFINE_1_INPUT_COMPONENT(name)              \
    class name : public _1InputComponent            \
    {                                               \
    public:                                         \
        name() {}                                   \
        template <typename T>                       \
        name(T &input) : _1InputComponent(input) {} \
                                                    \
    protected:                                      \
        bool _evaluate() override;                  \
    };

    class TimeComponent : public Component
    {
    public:
        TimeComponent() {}

        bool evaluate() override;

    protected:
        unsigned long _ticks = -1;
    };
}

#endif // COMPONENT_HPP
