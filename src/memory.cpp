#include "component.hpp"
#include "memory.hpp"
#include <iostream>

namespace logicsim
{
    namespace memory
    {
        // MemoryComponent
        MemoryComponent::MemoryComponent(component::Component &clk, unsigned int clk_out) : _clk(&clk), _clk_out(clk_out)
        {
        }

        void MemoryComponent::_check_inputs() const
        {
            if (!_clk)
            {
                throw component::NULLInput();
            }
            _clk->check();
        }

        void MemoryComponent::_clear_inputs()
        {
            _clk->clear();
        }

        // SRMemoryComponent
        SRMemoryComponent::SRMemoryComponent(component::Component &clk, component::Component &S, component::Component &R, unsigned int clk_out, unsigned int S_out, unsigned int R_out) : MemoryComponent(clk, clk_out), _S(&S), _R(&R), _S_out(S_out), _R_out(R_out)
        {
        }

        bool SRMemoryComponent::_evaluate(unsigned int out)
        {
            if (_clk_edge())
            {
                _Q = _S->evaluate(_S_out) || (_Q && !_R->evaluate(_R_out));
            }

            return out ^ _Q;
        }

        void SRMemoryComponent::_check_inputs() const
        {
            MemoryComponent::_check_inputs();
            if (!_S || !_R)
            {
                throw component::NULLInput();
            }
            _S->check();
            _R->check();
        }

        void SRMemoryComponent::_clear_inputs()
        {
            MemoryComponent::_clear_inputs();
            _S->clear();
            _R->clear();
        }

        // JKMemoryComponent
        JKMemoryComponent::JKMemoryComponent(component::Component &clk, component::Component &J, component::Component &K, unsigned int clk_out, unsigned int J_out, unsigned int K_out) : MemoryComponent(clk, clk_out), _J(&J), _K(&K), _J_out(J_out), _K_out(K_out)
        {
        }

        bool JKMemoryComponent::_evaluate(unsigned int out)
        {
            if (_clk_edge())
            {
                _Q = !_K->evaluate(_K_out) && _Q || _J->evaluate(_J_out) && !_Q;
            }

            return out ^ _Q;
        }

        void JKMemoryComponent::_check_inputs() const
        {
            MemoryComponent::_check_inputs();
            if (!_J || !_K)
            {
                throw component::NULLInput();
            }
            _J->check();
            _K->check();
        }

        void JKMemoryComponent::_clear_inputs()
        {
            MemoryComponent::_clear_inputs();
            _J->clear();
            _K->clear();
        }

        // DMemoryComponent
        DMemoryComponent::DMemoryComponent(component::Component &clk, component::Component &D, unsigned int clk_out, unsigned int D_out) : MemoryComponent(clk, clk_out), _D(&D), _D_out(D_out)
        {
        }

        bool DMemoryComponent::_evaluate(unsigned int out)
        {
            if (_clk_edge())
            {
                _Q = _D->evaluate(_D_out);
            }

            return out ^ _Q;
        }

        void DMemoryComponent::_check_inputs() const
        {
            MemoryComponent::_check_inputs();
            if (!_D)
            {
                throw component::NULLInput();
            }
            _D->check();
        }

        void DMemoryComponent::_clear_inputs()
        {
            MemoryComponent::_clear_inputs();
            _D->clear();
        }

        // TMemoryComponent
        TMemoryComponent::TMemoryComponent(component::Component &clk, component::Component &T, unsigned int clk_out, unsigned int T_out) : MemoryComponent(clk, clk_out), _T(&T), _T_out(T_out)
        {
        }

        bool TMemoryComponent::_evaluate(unsigned int out)
        {
            if (_clk_edge())
            {
                _Q = _T->evaluate(_T_out) ^ _Q;
            }

            return out ^ _Q;
        }

        void TMemoryComponent::_check_inputs() const
        {
            MemoryComponent::_check_inputs();
            if (!_T)
            {
                throw component::NULLInput();
            }
            _T->check();
        }

        void TMemoryComponent::_clear_inputs()
        {
            MemoryComponent::_clear_inputs();
            _T->clear();
        }
    }
}