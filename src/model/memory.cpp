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
                _clk = &_inputs[_inputs.size() - 1];
                _clk_out = &_inputs_out[_inputs.size() - 1];
            }

            void MemoryComponent::set_clk(component::Component &clk, unsigned int clk_out)
            {
                set_input(_inputs.size() - 1, clk, clk_out);
            }

            void MemoryComponent::clear()
            {
                NInputComponent::clear();
                _evaluated = false;
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
                _S = &_inputs[0];
                _S_out = &_inputs_out[0];
                _R = &_inputs[1];
                _R_out = &_inputs_out[1];
            }

            SRMemoryComponent::SRMemoryComponent(component::Component &clk, component::Component &S, component::Component &R, unsigned int clk_out, unsigned int S_out, unsigned int R_out) : SRMemoryComponent()
            {
                set_clk(clk, clk_out);
                set_input(0, S, S_out);
                set_input(1, R, R_out);
            }

            bool SRMemoryComponent::_evaluate(unsigned int out)
            {
                if (_clk_edge() && !_evaluated)
                {
                    _Q = (*_S)->evaluate(*_S_out) || (_Q && !(*_R)->evaluate(*_R_out));
                    _evaluated = true;
                }

                return out ^ _Q;
            }

            // JKMemoryComponent
            JKMemoryComponent::JKMemoryComponent() : MemoryComponent(2)
            {
                _J = &_inputs[0];
                _J_out = &_inputs_out[0];
                _K = &_inputs[1];
                _K_out = &_inputs_out[1];
            }

            JKMemoryComponent::JKMemoryComponent(component::Component &clk, component::Component &J, component::Component &K, unsigned int clk_out, unsigned int J_out, unsigned int K_out) : JKMemoryComponent()
            {
                set_clk(clk, clk_out);
                set_input(0, J, J_out);
                set_input(1, K, K_out);
            }

            bool JKMemoryComponent::_evaluate(unsigned int out)
            {
                if (_clk_edge() && !_evaluated)
                {
                    _Q = !(*_K)->evaluate(*_K_out) && _Q || (*_J)->evaluate(*_J_out) && !_Q;
                    _evaluated = true;
                }

                return out ^ _Q;
            }

            // DMemoryComponent
            DMemoryComponent::DMemoryComponent() : MemoryComponent(1)
            {
                _D = &_inputs[0];
                _D_out = &_inputs_out[0];
            }

            DMemoryComponent::DMemoryComponent(component::Component &clk, component::Component &D, unsigned int clk_out, unsigned int D_out) : DMemoryComponent()
            {
                set_clk(clk, clk_out);
                set_input(0, D, D_out);
            }

            bool DMemoryComponent::_evaluate(unsigned int out)
            {
                if (_clk_edge() && !_evaluated)
                {
                    _Q = (*_D)->evaluate(*_D_out);
                    _evaluated = true;
                }

                return out ^ _Q;
            }

            // TMemoryComponent
            TMemoryComponent::TMemoryComponent() : MemoryComponent(1)
            {
                _T = &_inputs[0];
                _T_out = &_inputs_out[0];
            }

            TMemoryComponent::TMemoryComponent(component::Component &clk, component::Component &T, unsigned int clk_out, unsigned int T_out) : TMemoryComponent()
            {
                set_clk(clk, clk_out);
                set_input(0, T, T_out);
            }

            bool TMemoryComponent::_evaluate(unsigned int out)
            {
                if (_clk_edge() && !_evaluated)
                {
                    _Q ^= (*_T)->evaluate(*_T_out);
                    _evaluated = true;
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
