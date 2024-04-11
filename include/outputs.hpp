#ifndef OUTPUTS_HPP
#define OUTPUTS_HPP

#include "component.hpp"

namespace logicsim
{
    namespace output
    {
        DEFINE_1_INPUT_COMPONENT(BaseOutput)

        class _5in_7SegmentDisplay : public component::NInputComponent
        {
        public:
            _5in_7SegmentDisplay();
            _5in_7SegmentDisplay(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, unsigned int input1_out = 0, unsigned int input2_out = 0, unsigned int input3_out = 0, unsigned int input4_out = 0, unsigned int input5_out = 0);

            void set_inputs(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, unsigned int input1_out = 0, unsigned int input2_out = 0, unsigned int input3_out = 0, unsigned int input4_out = 0, unsigned int input5_out = 0);

        protected:
            bool _input_cache[5], _cached = false;
            bool _evaluate(unsigned int out = 0) override;

            void _clear_inputs() override;
        };

        class _8in_7SegmentDisplay : public component::NInputComponent
        {
        public:
            _8in_7SegmentDisplay();
            _8in_7SegmentDisplay(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, component::Component &input6, component::Component &input7, component::Component &input8, unsigned int input1_out = 0, unsigned int input2_out = 0, unsigned int input3_out = 0, unsigned int input4_out = 0, unsigned int input5_out = 0, unsigned int input6_out = 0, unsigned int input7_out = 0, unsigned int input8_out = 0);

            void set_inputs(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, component::Component &input6, component::Component &input7, component::Component &input8, unsigned int input1_out = 0, unsigned int input2_out = 0, unsigned int input3_out = 0, unsigned int input4_out = 0, unsigned int input5_out = 0, unsigned int input6_out = 0, unsigned int input7_out = 0, unsigned int input8_out = 0);

        protected:
            bool _input_cache[8], _cached = false;
            bool _evaluate(unsigned int out = 0) override;

            void _clear_inputs() override;
        };
    }
}

#endif // OUTPUTS_HPP
