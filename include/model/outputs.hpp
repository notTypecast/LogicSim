#ifndef LOGICSIM_MODEL_OUTPUTS_HPP
#define LOGICSIM_MODEL_OUTPUTS_HPP

#include "model/component.hpp"

namespace logicsim
{
namespace model
{
namespace output
{
class Output : public component::NInputComponent
{
  public:
    Output(unsigned int n_inputs, unsigned int n_evals);

    unsigned int n_outputs() const override;
};

class BaseOutput : public Output
{
  public:
    BaseOutput();

    std::string ctype() const override;

  protected:
    State _evaluate(unsigned int = 0) override;
};

class _5in_7SegmentDisplay : public Output
{
  public:
    _5in_7SegmentDisplay();

    void tick() override;

    std::string ctype() const override;

  protected:
    State _input_cache[5];
    bool  _ticked = false;
    State _evaluate(unsigned int out = 0) override;
};

class _8in_7SegmentDisplay : public Output
{
  public:
    _8in_7SegmentDisplay();

    void tick() override;

    std::string ctype() const override;

  protected:
    State _input_cache[8];
    bool  _ticked = false;
    State _evaluate(unsigned int out = 0) override;
};
}
}
}

#endif // LOGICSIM_MODEL_OUTPUTS_HPP
