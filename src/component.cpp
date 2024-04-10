#include "component.hpp"

namespace logicsim
{
    const char *NULLInput::what() const noexcept
    {
        return "NULL input";
    }

    bool Component::evaluate()
    {
        ++tmp;
        bool tmp = _cached;
        if (!_cached)
        {
            _output = _evaluate();
            _cached = true;
        }
        return _output;
    }

    void Component::clear()
    {
        if (_cached)
        {
            _clear_inputs();
            _cached = false;
        }
    }

    void Component::check() const
    {
        _check_inputs();
    }

    void _2InputComponent::_check_inputs() const
    {
        if (!_input1 || !_input2)
        {
            throw NULLInput();
        }
        _input1->check();
        _input2->check();
    }

    void _2InputComponent::_clear_inputs() const
    {
        _input1->clear();
        _input2->clear();
    }

    void _1InputComponent::_check_inputs() const
    {
        if (!_input)
        {
            throw NULLInput();
        }
        _input->check();
    }

    void _1InputComponent::_clear_inputs() const
    {
        _input->clear();
    }

    bool TimeComponent::evaluate()
    {
        if (!_cached) {
            ++_ticks;
        }
        return Component::evaluate();
    }

}