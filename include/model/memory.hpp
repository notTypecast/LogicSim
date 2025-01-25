#ifndef LOGICSIM_MODEL_MEMORY_HPP
#define LOGICSIM_MODEL_MEMORY_HPP

#include "model/component.hpp"

namespace logicsim
{
namespace model
{
namespace memory
{
class MemoryComponent : virtual public component::NInputComponent
{
  public:
    MemoryComponent();

    virtual void tick() override;
    virtual void reset() override;

    unsigned int n_outputs() const override;

  protected:
    // Double pointers point to pointers to input components kept by
    // NInputComponent superclass. Thus, when changing an input component, the
    // double pointer will automatically point to the new component
    component::Component **_pre, **_clr;
    unsigned int          *_pre_out, *_clr_out;

    State _Q         = State::HiZ;
    bool  _evaluated = false;

    State _evaluate(unsigned int out = 0) override final;
    // Updates state
    virtual void _memory_evaluate() = 0;
};

class SRMemoryComponent : public MemoryComponent
{
  public:
    SRMemoryComponent();

  protected:
    Component   **_S, **_R;
    unsigned int *_S_out, *_R_out;

    void _memory_evaluate() override;
};

class JKMemoryComponent : public MemoryComponent
{
  public:
    JKMemoryComponent();

  protected:
    Component   **_J, **_K;
    unsigned int *_J_out, *_K_out;

    void _memory_evaluate() override;
};

class DMemoryComponent : public MemoryComponent
{
  public:
    DMemoryComponent();

  protected:
    Component   **_D;
    unsigned int *_D_out;

    void _memory_evaluate() override;
};

class TMemoryComponent : public MemoryComponent
{
  public:
    TMemoryComponent();

  protected:
    Component   **_T;
    unsigned int *_T_out;

    void _memory_evaluate() override;
};

/* Defines a clocked memory component
 * Inputs:
 *  * The class name
 *  * The memory component to inherit from
 *  * The clock type to use
 */
#define DEFINE_CLOCKED_MEMORY(name, memory_component, clock_type) \
    class name                                                    \
      : public memory_component                                   \
      , public clock_type                                         \
    {                                                             \
      public:                                                     \
        name();                                                   \
        std::string ctype() const override;                       \
                                                                  \
      protected:                                                  \
        void _memory_evaluate() override;                         \
    };

/* Implements a defined clocked memory component
 * Inputs:
 *  * The class name
 *  * The memory component it inherits from
 *  * The clock type it uses
 *  * The total number of inputs
 *  * The component delay
 *  * The total number of evaluations
 *  * The index of the clock input
 *  * The ctype string
 */
#define IMPLEMENT_CLOCKED_MEMORY(name,                \
                                 memory_component,    \
                                 clock_type,          \
                                 n,                   \
                                 delay,               \
                                 n_evals,             \
                                 clk_idx,             \
                                 ctype_str)           \
    name::name()                                      \
      : component::NInputComponent(n, delay, n_evals) \
      , clock_type(clk_idx)                           \
    {                                                 \
    }                                                 \
    std::string name::ctype() const                   \
    {                                                 \
        return ctype_str;                             \
    }                                                 \
    void name::_memory_evaluate()                     \
    {                                                 \
        if (_clk_edge())                              \
        {                                             \
            memory_component::_memory_evaluate();     \
        }                                             \
    }

DEFINE_CLOCKED_MEMORY(SRLatch, SRMemoryComponent,
                      component::LevelTriggeredComponent)
DEFINE_CLOCKED_MEMORY(JKLatch, JKMemoryComponent,
                      component::LevelTriggeredComponent)
DEFINE_CLOCKED_MEMORY(DLatch, DMemoryComponent,
                      component::LevelTriggeredComponent)
DEFINE_CLOCKED_MEMORY(TLatch, TMemoryComponent,
                      component::LevelTriggeredComponent)
DEFINE_CLOCKED_MEMORY(SRFlipFlop, SRMemoryComponent,
                      component::EdgeTriggeredComponent)
DEFINE_CLOCKED_MEMORY(JKFlipFlop, JKMemoryComponent,
                      component::EdgeTriggeredComponent)
DEFINE_CLOCKED_MEMORY(TFlipFlop, TMemoryComponent,
                      component::EdgeTriggeredComponent)
DEFINE_CLOCKED_MEMORY(DFlipFlop, DMemoryComponent,
                      component::EdgeTriggeredComponent)

}
}
}

#endif // LOGICSIM_MODEL_MEMORY_HPP
