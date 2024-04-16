#include "model/component.hpp"
#include "model/memory.hpp"

namespace logicsim
{
    namespace model
    {
        namespace memory
        {
            // MemoryComponent
            MemoryComponent::MemoryComponent(unsigned int n) : NInputComponent(n + 1)
            {
                _clk = &_inputs[0];
                _clk_out = &_inputs_out[0];
            }

            void MemoryComponent::set_clk(component::Component &clk, unsigned int clk_out)
            {
                set_input(0, clk, clk_out);
            }

            void MemoryComponent::reset()
            {
                _Q = false;
            }

            unsigned int MemoryComponent::n_outputs() const
            {
                return 2;
            }

            // SRMemoryComponent
            SRMemoryComponent::SRMemoryComponent() : MemoryComponent(2)
            {
                _S = &_inputs[1];
                _S_out = &_inputs_out[1];
                _R = &_inputs[2];
                _R_out = &_inputs_out[2];
            }

            SRMemoryComponent::SRMemoryComponent(component::Component &clk, component::Component &S, component::Component &R, unsigned int clk_out, unsigned int S_out, unsigned int R_out) : SRMemoryComponent()
            {
                set_clk(clk, clk_out);
                set_input(1, S, S_out);
                set_input(2, R, R_out);
            }

            bool SRMemoryComponent::_evaluate(unsigned int out)
            {
                if (_clk_edge())
                {
                    _Q = (*_S)->evaluate(*_S_out) || (_Q && !(*_R)->evaluate(*_R_out));
                }

                return out ^ _Q;
            }

            // JKMemoryComponent
            JKMemoryComponent::JKMemoryComponent() : MemoryComponent(2)
            {
                _J = &_inputs[1];
                _J_out = &_inputs_out[1];
                _K = &_inputs[2];
                _K_out = &_inputs_out[2];
            }

            JKMemoryComponent::JKMemoryComponent(component::Component &clk, component::Component &J, component::Component &K, unsigned int clk_out, unsigned int J_out, unsigned int K_out) : JKMemoryComponent()
            {
                set_clk(clk, clk_out);
                set_input(1, J, J_out);
                set_input(2, K, K_out);
            }

            bool JKMemoryComponent::_evaluate(unsigned int out)
            {
                if (_clk_edge())
                {
                    _Q = !(*_K)->evaluate(*_K_out) && _Q || (*_J)->evaluate(*_J_out) && !_Q;
                }

                return out ^ _Q;
            }

            // DMemoryComponent
            DMemoryComponent::DMemoryComponent() : MemoryComponent(1)
            {
                _D = &_inputs[1];
                _D_out = &_inputs_out[1];
            }

            DMemoryComponent::DMemoryComponent(component::Component &clk, component::Component &D, unsigned int clk_out, unsigned int D_out) : DMemoryComponent()
            {
                set_clk(clk, clk_out);
                set_input(1, D, D_out);
            }

            bool DMemoryComponent::_evaluate(unsigned int out)
            {
                if (_clk_edge())
                {
                    _Q = (*_D)->evaluate(*_D_out);
                }

                return out ^ _Q;
            }

            // TMemoryComponent
            TMemoryComponent::TMemoryComponent() : MemoryComponent(1)
            {
                _T = &_inputs[1];
                _T_out = &_inputs_out[1];
            }

            TMemoryComponent::TMemoryComponent(component::Component &clk, component::Component &T, unsigned int clk_out, unsigned int T_out) : TMemoryComponent()
            {
                set_clk(clk, clk_out);
                set_input(1, T, T_out);
            }

            bool TMemoryComponent::_evaluate(unsigned int out)
            {
                if (_clk_edge())
                {
                    _Q = (*_T)->evaluate(*_T_out) ^ _Q;
                }

                return out ^ _Q;
            }

            // Component Types for implementations

            std::string SRLatch::ctype() const
            {
                return "SRLATCH";
            }

            std::string JKLatch::ctype() const
            {
                return "JKLATCH";
            }

            std::string DLatch::ctype() const
            {
                return "DLATCH";
            }

            std::string TLatch::ctype() const
            {
                return "TLATCH";
            }

            std::string SRFlipFlop::ctype() const
            {
                return "SRFLIPFLOP";
            }

            std::string JKFlipFlop::ctype() const
            {
                return "JKFLIPFLOP";
            }

            std::string DFlipFlop::ctype() const
            {
                return "DFLIPFLOP";
            }

            std::string TFlipFlop::ctype() const
            {
                return "TFLIPFLOP";
            }
        }
    }
}
