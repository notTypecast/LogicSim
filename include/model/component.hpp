#ifndef LOGICSIM_MODEL_COMPONENT_HPP
#define LOGICSIM_MODEL_COMPONENT_HPP

#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <cassert>
#include <type_traits>
#include <exception>
#include <limits>
#include <utility>

namespace logicsim
{
    namespace model
    {
        namespace component
        {
            class null_input : public std::exception
            {
            public:
                const char *what() const noexcept override;
            };

            class Component
            {
            public:
                Component(unsigned int delay, unsigned int n_evals);
                virtual ~Component();

                // calculates all outputs for current inputs
                virtual void tick();
                // returns value for given output, taking delay into account
                bool evaluate(unsigned int out = 0);

                virtual void check() const; // checks input components
                virtual void clear();       // clears component cache, allowing next tick
                virtual void reset();       // resets component state

                virtual unsigned int n_inputs() const;
                virtual unsigned int n_outputs() const;
                // n_evals: how many evaluations component produces (indices as input to evaluate())
                // for non-outputs, this is equal to n_outputs
                unsigned int n_evals() const;

                // Methods for saving/loading circuits
                unsigned int id() const;
                virtual std::string ctype() const = 0;
                // map: input index -> (input component id, output index on input component)
                virtual std::vector<std::pair<unsigned int, unsigned int>> input_ids() const;
                virtual std::string param_string() const;
                virtual void set_params(const std::string &param_string);

            protected:
                static unsigned int _CURR_ID;
                unsigned int _id;

                size_t _history_size;
                unsigned int _n_evals;
                std::list<std::vector<bool>> _cache_history;
                bool _cached = false;

                virtual bool _evaluate(unsigned int out = 0) = 0;


            };

            class NInputComponent : public Component
            {
            public:
                NInputComponent(unsigned int n, unsigned int delay, unsigned int n_evals);

                void set_input(size_t index, Component &input, unsigned int out = 0);
                void remove_input(size_t index);

                virtual std::vector<std::pair<unsigned int, unsigned int>> input_ids() const override;

                void check() const override;

                unsigned int n_inputs() const override;

            protected:
                unsigned int _n;
                std::vector<Component *> _inputs;
                std::vector<unsigned int> _inputs_out;
            };

    #define DEFINE_1_INPUT_COMPONENT(name)                                          \
        class name : public component::NInputComponent                              \
        {                                                                           \
        public:                                                                     \
            name() : NInputComponent(1, 1, 1) {}                                    \
            name(Component &input, unsigned int out = 0) : NInputComponent(1, 1, 1) \
            {                                                                       \
                set_input(0, input, out);                                           \
            }                                                                       \
            std::string ctype() const override;                                     \
                                                                                    \
        protected:                                                                  \
            bool _evaluate(unsigned int = 0) override;                              \
        };

    #define DEFINE_2_INPUT_COMPONENT(name)                                                                                      \
        class name : public component::NInputComponent                                                                          \
        {                                                                                                                       \
        public:                                                                                                                 \
            name() : NInputComponent(2, 1, 1) {}                                                                                \
            name(Component &input1, Component &input2, unsigned int out1 = 0, unsigned int out2 = 0) : NInputComponent(2, 1, 1) \
            {                                                                                                                   \
                set_input(0, input1, out1);                                                                                     \
                set_input(1, input2, out2);                                                                                     \
            }                                                                                                                   \
            std::string ctype() const override;                                                                                 \
                                                                                                                                \
        protected:                                                                                                              \
            bool _evaluate(unsigned int = 0) override;                                                                          \
        };

            class TimeComponent : public Component
            {
            public:
                TimeComponent(unsigned int delay, unsigned int n_evals);

                void tick() override;
                void reset() override;

            protected:
                unsigned long _ticks = -1;
            };
        }
    }
}

#endif // COMPONENT_HPP
