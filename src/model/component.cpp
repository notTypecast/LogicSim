#include "model/component.hpp"

namespace logicsim
{
    namespace model
    {
        namespace component
        {
            const char *null_input::what() const noexcept
            {
                return "NULL input";
            }

            // Component
            unsigned int Component::_CURR_ID = 0;

            Component::Component(unsigned int delay, unsigned int n_evals) : _history_size(delay + 1), _n_evals(n_evals), _cache_history(_history_size, std::vector<bool>(n_evals, false))
            {
                _id = _CURR_ID++;
            }

            Component::~Component()
            {
            }

            void Component::tick()
            {
                for (size_t i = 0; i < _n_evals; ++i)
                {
                    _cache_history.front()[i] = _evaluate(i);
                }
            }

            void Component::update()
            {
                if (_history_size > 1)
                {
                    std::vector<bool> back = std::move(_cache_history.back());
                    _cache_history.pop_back();
                    _cache_history.push_front(std::move(back));
                }
                else
                {
                    tick();
                }
            }

            bool Component::evaluate(unsigned int out)
            {
                return _cache_history.back()[out];
            }

            void Component::check() const
            {
            }

            void Component::reset()
            {
                std::list<std::vector<bool>>::iterator it;
                for (it = _cache_history.begin(); it != _cache_history.end(); ++it)
                {
                    for (size_t i = 0; i < _n_evals; ++i)
                    {
                        (*it)[i] = false;
                    }
                }
            }

            // default value
            unsigned int Component::n_inputs() const
            {
                return 0;
            }

            unsigned int Component::n_outputs() const
            {
                return 1;
            }

            unsigned int Component::n_evals() const
            {
                return _n_evals;
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

            void Component::set_params(const std::string &param_string)
            {
            }

            // NInputComponent
            NInputComponent::NInputComponent(unsigned int n, unsigned int delay, unsigned int n_evals) : Component(delay, n_evals), _n(n), _inputs(n, nullptr), _inputs_out(n) {}

            void NInputComponent::set_input(size_t index, Component &input, unsigned int out)
            {
                assert(index < _n);
                _inputs[index] = &input;
                _inputs_out[index] = out;
            }

            void NInputComponent::remove_input(size_t index)
            {
                assert(index < _n);
                _inputs[index] = nullptr;
            }

            void NInputComponent::check() const
            {
                for (auto &input : _inputs)
                {
                    if (!input)
                    {
                        throw null_input();
                    }
                }
            }

            unsigned int NInputComponent::n_inputs() const
            {
                return _n;
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
            TimeComponent::TimeComponent(unsigned int delay, unsigned int n_evals) : Component(delay, n_evals)
            {
            }

            void TimeComponent::tick()
            {
                if (!_ticked)
                {
                    ++_ticks;
                    _ticked = true;
                }
                Component::tick();
            }

            void TimeComponent::update()
            {
                _ticked = false;
                Component::update();
            }

            void TimeComponent::reset()
            {
                _ticks = -1;
                Component::reset();
            }
        }
    }
}
