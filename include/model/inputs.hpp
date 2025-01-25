#ifndef LOGICSIM_MODEL_INPUTS_HPP
#define LOGICSIM_MODEL_INPUTS_HPP

#include <random>

#include "model/component.hpp"

#include "utils.hpp"

namespace logicsim
{
namespace model
{
namespace input
{
class Input : public component::Component
{
  public:
    Input(unsigned int n_evals);
};

class Constant : public Input
{
  public:
    Constant();
    Constant(bool value);

    std::string ctype() const override;
    std::string param_string() const override;
    void        set_params(const std::string &param_string) override;

  protected:
    bool  _value = false;
    State _evaluate(unsigned int = 0) override;
};

class Button : public Input
{
  public:
    Button();

    void press();
    void release();

    std::string ctype() const override;

  protected:
    bool  _state = false;
    State _evaluate(unsigned int = 0) override;
};

class Switch : public Input
{
  public:
    Switch();
    Switch(bool value);

    void toggle();

    std::string ctype() const override;
    std::string param_string() const override;
    void        set_params(const std::string &param_string) override;

  protected:
    bool  _value = false;
    State _evaluate(unsigned int = 0) override;
};

class Oscillator : public component::TimeComponent
{
  public:
    Oscillator();
    Oscillator(unsigned int low_ticks, unsigned int high_ticks,
               unsigned int phase = 0);

    std::string ctype() const override;
    std::string param_string() const override;
    void        set_params(const std::string &param_string) override;

  protected:
    unsigned int _low_ticks = 200;
    unsigned int _period    = 400;
    unsigned int _phase     = 0;
    State        _evaluate(unsigned int = 0) override;
};

class Keypad : public Input
{
  public:
    Keypad();

    void set_key(unsigned int key);

    unsigned int n_outputs() const override;

    std::string ctype() const override;

  protected:
    unsigned int _key;
    State        _evaluate(unsigned int out = 0) override;
};

class Random : public component::EdgeTriggeredComponent
{
  public:
    Random();

    unsigned int n_outputs() const override;

    std::string ctype() const override;

    virtual void tick() override;
    virtual void reset() override;

  protected:
    std::random_device                                       _dev;
    std::mt19937                                             _rng;
    std::uniform_int_distribution<std::mt19937::result_type> _state_dist;

    bool  _evaluated = false;
    State _stored[4] = { State::HiZ, State::HiZ, State::HiZ, State::HiZ };
    State _evaluate(unsigned int out = 0) override;
};
}
}
}

#endif // LOGICSIM_MODEL_INPUTS_HPP
