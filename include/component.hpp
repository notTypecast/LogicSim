#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <type_traits>
#include <exception>
#include <limits>

namespace logicsim
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
            Component();

            virtual bool evaluate(unsigned int out = 0);
            virtual void check() const; // recursively checks input components
            virtual void clear();       // recursively clears cached state of input components
            virtual void reset();       // recursively resets component state

            virtual unsigned int n_inputs() const;
            virtual unsigned int n_outputs() const;

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
            std::unordered_map<unsigned int, bool> _cache;
            virtual bool _evaluate(unsigned int out = 0) = 0;
        };

        class NInputComponent : public Component
        {
        public:
            NInputComponent(unsigned int n);

            void set_input(size_t index, Component &input, unsigned int out = 0);

            virtual std::vector<std::pair<unsigned int, unsigned int>> input_ids() const override;

            void check() const override;
            void clear() override;
            void reset() override;

            unsigned int n_inputs() const override;

        protected:
            unsigned int _n;
            std::vector<Component *> _inputs;
            std::vector<unsigned int> _inputs_out;
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
        std::string ctype() const override;                               \
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
        std::string ctype() const override;                                                                           \
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
