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
                Output(unsigned int n = 1);

                unsigned int n_outputs() const override;
            };

            class BaseOutput : public Output
            {
            public:
                BaseOutput();
                BaseOutput(Component &input, unsigned int out = 0);

                unsigned int n_evals() const override;

                std::string ctype() const override;

            protected:
                bool _evaluate(unsigned int = 0) override;
            };

            class _5in_7SegmentDisplay : public Output
            {
            public:
                _5in_7SegmentDisplay();
                _5in_7SegmentDisplay(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, unsigned int input1_out = 0, unsigned int input2_out = 0, unsigned int input3_out = 0, unsigned int input4_out = 0, unsigned int input5_out = 0);

                void set_inputs(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, unsigned int input1_out = 0, unsigned int input2_out = 0, unsigned int input3_out = 0, unsigned int input4_out = 0, unsigned int input5_out = 0);

                void clear() override;

                unsigned int n_evals() const override;

                std::string ctype() const override;

            protected:
                bool _input_cache[5], _cached = false;
                bool _evaluate(unsigned int out = 0) override;
            };

            class _8in_7SegmentDisplay : public Output
            {
            public:
                _8in_7SegmentDisplay();
                _8in_7SegmentDisplay(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, component::Component &input6, component::Component &input7, component::Component &input8, unsigned int input1_out = 0, unsigned int input2_out = 0, unsigned int input3_out = 0, unsigned int input4_out = 0, unsigned int input5_out = 0, unsigned int input6_out = 0, unsigned int input7_out = 0, unsigned int input8_out = 0);

                void set_inputs(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, component::Component &input6, component::Component &input7, component::Component &input8, unsigned int input1_out = 0, unsigned int input2_out = 0, unsigned int input3_out = 0, unsigned int input4_out = 0, unsigned int input5_out = 0, unsigned int input6_out = 0, unsigned int input7_out = 0, unsigned int input8_out = 0);

                void clear() override;

                unsigned int n_evals() const override;

                std::string ctype() const override;

            protected:
                bool _input_cache[8], _cached = false;
                bool _evaluate(unsigned int out = 0) override;
            };
        }
    }
}

#endif // OUTPUTS_HPP
