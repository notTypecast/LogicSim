#include "model/memory.hpp"
#include "model/component.hpp"

namespace logicsim
{
namespace model
{
namespace memory
{
// MemoryComponent
MemoryComponent::MemoryComponent()
{
    int s    = _inputs.size();
    _pre     = &_inputs[0];
    _pre_out = &_inputs_out[0];
    _clr     = &_inputs[s - 1];
    _clr_out = &_inputs_out[s - 1];
}

void MemoryComponent::tick()
{
    _evaluated = false;
    Component::tick();
}

void MemoryComponent::reset()
{
    Component::reset();
    _Q = State::HiZ;
}

unsigned int MemoryComponent::n_outputs() const
{
    return 2;
}

State MemoryComponent::_evaluate(unsigned int out)
{
    State pre = (*_pre)->evaluate(*_pre_out);
    State clr = (*_clr)->evaluate(*_clr_out);

    // override HiZ to 0, so unconnected lines do not affect function
    pre = pre == State::HiZ ? State::ZERO : pre;
    clr = clr == State::HiZ ? State::ZERO : clr;

    if (pre ^ clr)
    {
        _Q = pre ? State::ONE : State::ZERO;
        return static_cast<State>(out) ^ _Q;
    }

    if (pre)
    {
        _Q = State::HiZ;
        return static_cast<State>(out) ^ _Q;
    }

    if (!_evaluated)
    {
        _memory_evaluate();
        _evaluated = true;
    }

    return static_cast<State>(out) ^ _Q;
}

// SRMemoryComponent
SRMemoryComponent::SRMemoryComponent()
{
    _S     = &_inputs[1];
    _S_out = &_inputs_out[1];
    _R     = &_inputs[2];
    _R_out = &_inputs_out[2];
}

void SRMemoryComponent::_memory_evaluate()
{
    _Q = (*_S)->evaluate(*_S_out) || (_Q && !(*_R)->evaluate(*_R_out));
}

// JKMemoryComponent
JKMemoryComponent::JKMemoryComponent()
{
    _J     = &_inputs[1];
    _J_out = &_inputs_out[1];
    _K     = &_inputs[2];
    _K_out = &_inputs_out[2];
}

void JKMemoryComponent::_memory_evaluate()
{
    _Q = (!(*_K)->evaluate(*_K_out) && _Q) || ((*_J)->evaluate(*_J_out) && !_Q);
}

// DMemoryComponent
DMemoryComponent::DMemoryComponent()
{
    _D     = &_inputs[1];
    _D_out = &_inputs_out[1];
}

void DMemoryComponent::_memory_evaluate()
{
    _Q = (*_D)->evaluate(*_D_out);
}

// TMemoryComponent
TMemoryComponent::TMemoryComponent()
{
    _T     = &_inputs[1];
    _T_out = &_inputs_out[1];
}

void TMemoryComponent::_memory_evaluate()
{
    _Q = _Q ^ (*_T)->evaluate(*_T_out);
}

// Component Types for implementations

// SRLatch
IMPLEMENT_CLOCKED_MEMORY(SRLatch, SRMemoryComponent,
                         component::LevelTriggeredComponent, 5, 5, 2, 3,
                         "SRLATCH")

// JKLatch
IMPLEMENT_CLOCKED_MEMORY(JKLatch, JKMemoryComponent,
                         component::LevelTriggeredComponent, 5, 5, 2, 3,
                         "JKLATCH")

// DLatch
IMPLEMENT_CLOCKED_MEMORY(DLatch, DMemoryComponent,
                         component::LevelTriggeredComponent, 4, 5, 2, 2,
                         "DLATCH")

// TLatch
IMPLEMENT_CLOCKED_MEMORY(TLatch, TMemoryComponent,
                         component::LevelTriggeredComponent, 4, 5, 2, 2,
                         "TLATCH")

// SRFlipFlop
IMPLEMENT_CLOCKED_MEMORY(SRFlipFlop, SRMemoryComponent,
                         component::EdgeTriggeredComponent, 5, 5, 2, 3,
                         "SRFLIPFLOP")

// JKFlipFlop
IMPLEMENT_CLOCKED_MEMORY(JKFlipFlop, JKMemoryComponent,
                         component::EdgeTriggeredComponent, 5, 5, 2, 3,
                         "JKFLIPFLOP")

// DFlipFlop
IMPLEMENT_CLOCKED_MEMORY(DFlipFlop, DMemoryComponent,
                         component::EdgeTriggeredComponent, 4, 5, 2, 2,
                         "DFLIPFLOP")

// TFlipFlop
IMPLEMENT_CLOCKED_MEMORY(TFlipFlop, TMemoryComponent,
                         component::EdgeTriggeredComponent, 4, 5, 2, 2,
                         "TFLIPFLOP")

}
}
}
