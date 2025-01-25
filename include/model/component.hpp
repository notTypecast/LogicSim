#ifndef LOGICSIM_MODEL_COMPONENT_HPP
#define LOGICSIM_MODEL_COMPONENT_HPP

#include <cassert>
#include <exception>
#include <limits>
#include <list>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace logicsim
{
namespace model
{
enum State : short
{
    ZERO = 0,
    ONE  = 1,
    HiZ  = 2
};

inline State operator!(const State &x)
{
    return static_cast<State>((x != 2) * (1 - x) + (x == 2) * 2);
}

inline State operator&&(const State &x, const State &y)
{
    return static_cast<State>((x == 1) * y + (x == 2) * 2 * (y != 0));
}

inline State operator||(const State &x, const State &y)
{
    return static_cast<State>((x == 0) * y + (x == 1) +
                              (x == 2) * (2 - (y == 1)));
}

inline State operator^(const State &x, const State &y)
{
    return static_cast<State>((x == 0) * y + (x == 1) * !y + (x == 2) * 2);
}

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
    // moves output to next tick
    // separate to tick method, since all ticks are performed first, then all
    // updates
    virtual void update();

    // returns value for given output, taking delay into account
    State evaluate(unsigned int out = 0);

    virtual void check() const; // checks input components
    virtual void reset();       // resets component state

    virtual unsigned int n_inputs() const;
    virtual unsigned int n_outputs() const;
    // n_evals: how many evaluations component produces (indices as input to
    // evaluate()) for non-outputs, this is equal to n_outputs
    unsigned int n_evals() const;

    // Methods for saving/loading circuits
    unsigned int        id() const;
    virtual std::string ctype() const = 0;
    // map: input index -> (input component id, output index on input component)
    virtual std::vector<std::pair<unsigned int, unsigned int>> input_ids()
      const;
    virtual std::string param_string() const;
    virtual void        set_params(const std::string &param_string);

  protected:
    static unsigned int _CURR_ID;
    unsigned int        _id;

    size_t                        _history_size;
    unsigned int                  _n_evals;
    std::list<std::vector<State>> _cache_history;

    virtual State _evaluate(unsigned int out = 0) = 0;
};

// Singleton component object to use for undriven inputs
// Will always evaluate to the preset value (default HiZ)
class NullComponent : public Component
{
  public:
    static NullComponent &get_instance();

    std::string ctype() const override;

  private:
    NullComponent();

  public:
    NullComponent(Component const &)  = delete;
    void operator=(Component const &) = delete;

  protected:
    virtual State _evaluate(unsigned int out = 0) override;
};

class NInputComponent : public Component
{
  public:
    NInputComponent(unsigned int n, unsigned int delay, unsigned int n_evals);

    void set_input(size_t index, Component &input, unsigned int out = 0);
    void remove_input(size_t index);

    virtual std::vector<std::pair<unsigned int, unsigned int>> input_ids()
      const override;

    void check() const override;

    unsigned int n_inputs() const override;

  protected:
    unsigned int              _n;
    std::vector<Component *>  _inputs;
    std::vector<unsigned int> _inputs_out;
};

#define DEFINE_1_INPUT_COMPONENT(name, delay)       \
    class name : public component::NInputComponent  \
    {                                               \
      public:                                       \
        name() : NInputComponent(1, delay, 1) {}    \
        std::string ctype() const override;         \
                                                    \
      protected:                                    \
        State _evaluate(unsigned int = 0) override; \
    };

#define DEFINE_2_INPUT_COMPONENT(name, delay)       \
    class name : public component::NInputComponent  \
    {                                               \
      public:                                       \
        name() : NInputComponent(2, delay, 1) {}    \
        std::string ctype() const override;         \
                                                    \
      protected:                                    \
        State _evaluate(unsigned int = 0) override; \
    };

class TimeComponent : public Component
{
  public:
    TimeComponent(unsigned int delay, unsigned int n_evals);

    void tick() override;
    void update() override;
    void reset() override;

  protected:
    unsigned long _ticks  = -1;
    bool          _ticked = false;
};

class ClockedComponent : virtual public NInputComponent
{
  public:
    ClockedComponent(unsigned int clk_idx);

  protected:
    component::Component **_clk;
    unsigned int          *_clk_out;

    // whether the current clock state indicates input should be evaluated
    virtual bool _clk_edge() = 0;
};

class LevelTriggeredComponent : public ClockedComponent
{
  public:
    LevelTriggeredComponent(unsigned int clk_idx);

  protected:
    bool _clk_edge() override final;
};

class EdgeTriggeredComponent : public ClockedComponent
{
  public:
    EdgeTriggeredComponent(unsigned int clk_idx);

  protected:
    bool _prev_clk = false;
    bool _clk_edge() override final;
};
}
}
}

#endif // LOGICSIM_MODEL_COMPONENT_HPP
