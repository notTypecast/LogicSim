#include "component.hpp"

namespace logicsim
{
    namespace component
    {
        const char *NULLInput::what() const noexcept
        {
            return "NULL input";
        }

        // Component
        bool Component::evaluate(unsigned int out)
        {
            if (_cache.find(out) == _cache.end())
            {
                _cache[out] = _evaluate(out);
            }
            return _cache[out];
        }

        void Component::clear()
        {
            if (!_cache.empty())
            {
                _clear_inputs();
                _cache.clear();
            }
        }

        void Component::check() const
        {
            _check_inputs();
        }

        // NInputComponent
        NInputComponent::NInputComponent(unsigned int n) : _n(n), _inputs(n), _inputs_out(n) {}

        NInputComponent::NInputComponent(std::vector<Component> &inputs, std::vector<unsigned int> &inputs_out) : _n(inputs.size())
        {
            set_inputs(inputs, inputs_out);
        }

        void NInputComponent::set_inputs(std::vector<Component> &inputs, std::vector<unsigned int> &inputs_out)
        {
            assert(inputs.size() == inputs_out.size());
            for (size_t i = 0; i < _n; ++i)
            {
                set_input(i, inputs[i], inputs_out[i]);
            }
        }

        void NInputComponent::set_input(size_t index, Component &input, unsigned int out)
        {
            assert(index < _n);
            _inputs[index] = &input;
            _inputs_out[index] = out;
        }

        void NInputComponent::_check_inputs() const
        {
            for (auto &input : _inputs)
            {
                if (!input)
                {
                    throw NULLInput();
                }
                input->check();
            }
        }

        void NInputComponent::_clear_inputs()
        {
            for (auto &input : _inputs)
            {
                input->clear();
            }
        }

        // TimeComponent
        bool TimeComponent::evaluate(unsigned int out)
        {
            if (_cache.find(out) == _cache.end())
            {
                ++_ticks;
            }
            return Component::evaluate(out);
        }
    }
}
