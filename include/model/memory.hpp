#ifndef LOGICSIM_MODEL_MEMORY_HPP
#define LOGICSIM_MODEL_MEMORY_HPP

#include "model/component.hpp"

namespace logicsim
{
    namespace model
    {
        namespace memory
        {
            class MemoryComponent : public component::NInputComponent
            {
            public:
                MemoryComponent(unsigned int n);

                void set_clk(component::Component &clk, unsigned int clk_out = 0);

                virtual void tick() override;
                virtual void reset() override;

                unsigned int n_outputs() const override;

            protected:
                // Double pointers point to pointers to input components kept by NInputComponent superclass
                // Thus, when changing an input component, the double pointer will automatically point to the new component
                component::Component **_clk;
                unsigned int *_clk_out;
                component::Component **_pre, **_clr;
                unsigned int *_pre_out, *_clr_out;

                State _Q = State::HiZ;
                bool _evaluated = false;

                // whether the current clock state indicates input should be evaluated
                virtual bool _clk_edge() = 0;

                virtual State _memory_evaluate(unsigned int out = 0) = 0;
                State _evaluate(unsigned int out = 0) override final;
            };

            class SRMemoryComponent : public MemoryComponent
            {
            public:
                SRMemoryComponent();
                SRMemoryComponent(component::Component &clk, component::Component &S, component::Component &R, unsigned int clk_out = 0, unsigned int S_out = 0, unsigned int R_out = 0);

            protected:
                Component **_S, **_R;
                unsigned int *_S_out, *_R_out;

                State _memory_evaluate(unsigned int out = 0) override;
            };

            class JKMemoryComponent : public MemoryComponent
            {
            public:
                JKMemoryComponent();
                JKMemoryComponent(component::Component &clk, component::Component &J, component::Component &K, unsigned int clk_out = 0, unsigned int J_out = 0, unsigned int K_out = 0);

            protected:
                Component **_J, **_K;
                unsigned int *_J_out, *_K_out;

                State _memory_evaluate(unsigned int out = 0) override;
            };

            class DMemoryComponent : public MemoryComponent
            {
            public:
                DMemoryComponent();
                DMemoryComponent(component::Component &clk, component::Component &D, unsigned int clk_out = 0, unsigned int D_out = 0);

            protected:
                Component **_D;
                unsigned int *_D_out;

                State _memory_evaluate(unsigned int out = 0) override;
            };

            class TMemoryComponent : public MemoryComponent
            {
            public:
                TMemoryComponent();
                TMemoryComponent(component::Component &clk, component::Component &T, unsigned int clk_out = 0, unsigned int T_out = 0);

            protected:
                Component **_T;
                unsigned int *_T_out;

                State _memory_evaluate(unsigned int out = 0) override;
            };

    #define DEFINE_1_INPUT_LATCH(name, base)                                                                                                                             \
        class name : public base                                                                                                                                         \
        {                                                                                                                                                                \
        public:                                                                                                                                                          \
            name() {}                                                                                                                                                    \
            name(component::Component &clk, component::Component &input, unsigned int clk_out = 0, unsigned int input_out = 0) : base(clk, input, clk_out, input_out) {} \
            std::string ctype() const override;                                                                                                                          \
                                                                                                                                                                         \
        protected:                                                                                                                                                       \
            bool _clk_edge() override                                                                                                                                    \
            {                                                                                                                                                            \
                return (*_clk)->evaluate(*_clk_out);                                                                                                                     \
            }                                                                                                                                                            \
        };

    #define DEFINE_2_INPUT_LATCH(name, base)                                                                                                                                                                                                                \
        class name : public base                                                                                                                                                                                                                            \
        {                                                                                                                                                                                                                                                   \
        public:                                                                                                                                                                                                                                             \
            name() {}                                                                                                                                                                                                                                       \
            name(component::Component &clk, component::Component &input1, component::Component &input2, unsigned int clk_out = 0, unsigned int input1_out = 0, unsigned int input2_out = 0) : base(clk, input1, input2, clk_out, input1_out, input2_out) {} \
            std::string ctype() const override;                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                            \
        protected:                                                                                                                                                                                                                                          \
            bool _clk_edge() override                                                                                                                                                                                                                       \
            {                                                                                                                                                                                                                                               \
                return (*_clk)->evaluate(*_clk_out);                                                                                                                                                                                                        \
            }                                                                                                                                                                                                                                               \
        };

    #define DEFINE_1_INPUT_FLIPFLOP(name, base)                                                                                                                          \
        class name : public base                                                                                                                                         \
        {                                                                                                                                                                \
        public:                                                                                                                                                          \
            name() {}                                                                                                                                                    \
            name(component::Component &clk, component::Component &input, unsigned int clk_out = 0, unsigned int input_out = 0) : base(clk, input, clk_out, input_out) {} \
            std::string ctype() const override;                                                                                                                          \
                                                                                                                                                                         \
        protected:                                                                                                                                                       \
            bool _prev_clk = false;                                                                                                                                      \
            bool _clk_edge() override                                                                                                                                    \
            {                                                                                                                                                            \
                bool prev_clk = _prev_clk;                                                                                                                               \
                _prev_clk = (*_clk)->evaluate(*_clk_out);                                                                                                                \
                return !prev_clk && _prev_clk;                                                                                                                           \
            }                                                                                                                                                            \
        };

    #define DEFINE_2_INPUT_FLIPFLOP(name, base)                                                                                                                                                                                                             \
        class name : public base                                                                                                                                                                                                                            \
        {                                                                                                                                                                                                                                                   \
        public:                                                                                                                                                                                                                                             \
            name() {}                                                                                                                                                                                                                                       \
            name(component::Component &clk, component::Component &input1, component::Component &input2, unsigned int clk_out = 0, unsigned int input1_out = 0, unsigned int input2_out = 0) : base(clk, input1, input2, clk_out, input1_out, input2_out) {} \
            std::string ctype() const override;                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                            \
        protected:                                                                                                                                                                                                                                          \
            bool _prev_clk = false;                                                                                                                                                                                                                         \
            bool _clk_edge() override                                                                                                                                                                                                                       \
            {                                                                                                                                                                                                                                               \
                bool prev_clk = _prev_clk;                                                                                                                                                                                                                  \
                _prev_clk = (*_clk)->evaluate(*_clk_out);                                                                                                                                                                                                   \
                return !prev_clk && _prev_clk;                                                                                                                                                                                                              \
            }                                                                                                                                                                                                                                               \
        };

            DEFINE_2_INPUT_LATCH(SRLatch, SRMemoryComponent)
            DEFINE_2_INPUT_LATCH(JKLatch, JKMemoryComponent)
            DEFINE_1_INPUT_LATCH(DLatch, DMemoryComponent)
            DEFINE_1_INPUT_LATCH(TLatch, TMemoryComponent)

            DEFINE_2_INPUT_FLIPFLOP(SRFlipFlop, SRMemoryComponent)
            DEFINE_2_INPUT_FLIPFLOP(JKFlipFlop, JKMemoryComponent)
            DEFINE_1_INPUT_FLIPFLOP(DFlipFlop, DMemoryComponent)
            DEFINE_1_INPUT_FLIPFLOP(TFlipFlop, TMemoryComponent)
        }
    }
}

#endif // MEMORY_HPP
