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
        unsigned int Component::_CURR_ID = 0;

        Component::Component()
        {
            _id = _CURR_ID++;
        }

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
                _cache.clear();
            }
        }

        void Component::check() const
        {
        }

        void Component::reset()
        {
        }

        unsigned int Component::id() const
        {
            return _id;
        }

        std::vector<std::pair<unsigned int, unsigned int>> Component::input_ids() const
        {
            return {};
        }

        std::string Component::param_string() const
        {
            return "";
        }

        // NInputComponent
        NInputComponent::NInputComponent(unsigned int n) : _n(n), _inputs(n, nullptr), _inputs_out(n) {}

        void NInputComponent::set_input(size_t index, Component &input, unsigned int out)
        {
            assert(index < _n);
            _inputs[index] = &input;
            _inputs_out[index] = out;
        }

        void NInputComponent::check() const
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

        void NInputComponent::clear()
        {
            Component::clear();
            for (auto &input : _inputs)
            {
                input->clear();
            }
        }

        void NInputComponent::reset()
        {
            for (auto &input : _inputs)
            {
                input->reset();
            }
        }

        std::vector<std::pair<unsigned int, unsigned int>> NInputComponent::input_ids() const
        {
            std::vector<std::pair<unsigned int, unsigned int>> ids(_n, {std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max()});
            for (size_t i = 0; i < _n; ++i)
            {
                if (_inputs[i])
                {
                    ids[i] = {_inputs[i]->id(), _inputs_out[i]};
                }
            }

            return ids;
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
