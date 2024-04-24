#include "model/outputs.hpp"

namespace logicsim
{
    namespace model
    {
        namespace output
        {
            // Output
            Output::Output(unsigned int n_inputs, unsigned int n_evals) : NInputComponent(n_inputs, 0, n_evals) {}

            unsigned int Output::n_outputs() const
            {
                return 0;
            }

            // BaseOutput
            BaseOutput::BaseOutput() : Output(1, 1) {}

            BaseOutput::BaseOutput(component::Component &input, unsigned int input_out) : Output(1, 1)
            {
                set_input(0, input, input_out);
            }

            bool BaseOutput::_evaluate(unsigned int)
            {
                return _inputs[0]->evaluate(_inputs_out[0]);
            }

            std::string BaseOutput::ctype() const
            {
                return "OUTPUT";
            }

            // _5in_7SegmentDisplay
            _5in_7SegmentDisplay::_5in_7SegmentDisplay() : Output(5, 8) {}

            _5in_7SegmentDisplay::_5in_7SegmentDisplay(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, unsigned int input1_out, unsigned int input2_out, unsigned int input3_out, unsigned int input4_out, unsigned int input5_out) : Output(5, 8)
            {
                set_inputs(input1, input2, input3, input4, input5, input1_out, input2_out, input3_out, input4_out, input5_out);
            }

            void _5in_7SegmentDisplay::set_inputs(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, unsigned int input1_out, unsigned int input2_out, unsigned int input3_out, unsigned int input4_out, unsigned int input5_out)
            {
                set_input(0, input1, input1_out);
                set_input(1, input2, input2_out);
                set_input(2, input3, input3_out);
                set_input(3, input4, input4_out);
                set_input(4, input5, input5_out);
            }

            void _5in_7SegmentDisplay::clear()
            {
                Component::clear();
                _cached = false;
            }

            bool _5in_7SegmentDisplay::_evaluate(unsigned int out)
            {
                if (!_cached)
                {
                    for (size_t i = 0; i < 5; ++i)
                    {
                        _input_cache[i] = _inputs[i]->evaluate(_inputs_out[i]);
                    }
                }

                bool(&x)[5] = _input_cache;
                switch (out)
                {
                case 1:
                    return !(x[0] && x[1] && x[2] || x[0] && x[1] && !x[2] && !x[3] || !x[0] && !x[1] && x[2] && !x[3]);
                case 2:
                    return !(x[1] && x[2] && x[3] || !x[0] && x[1] && !x[2] && !x[3] || !x[0] && !x[1] && !x[2] && x[3] || x[0] && !x[1] && x[2] && !x[3]);
                case 3:
                    return !(!x[0] && x[3] || !x[0] && x[1] && !x[2] && !x[3] || x[0] && !x[1] && !x[2] && x[3]);
                case 4:
                    return !(!x[0] && !x[1] && !x[2] || !x[0] && x[1] && x[2] && x[3] || x[0] && x[1] && !x[2] && !x[3]);
                case 5:
                    return !(!x[0] && !x[1] && x[2] || !x[0] && x[2] && x[3] || !x[0] && !x[1] && !x[2] && x[3] || x[0] && x[1] && !x[2] && x[3]);
                case 6:
                    return !(!x[0] && !x[1] && !x[2] && x[3] || !x[0] && x[1] && !x[2] && !x[3] || x[0] && x[1] && !x[2] && x[3] || x[0] && !x[1] && x[2] && x[3]);
                case 7:
                    return !(x[1] && x[2] && !x[3] || x[0] && x[1] && x[2] || x[0] && x[2] && x[3] || x[0] && x[1] && !x[3] || !x[0] && x[1] && !x[2] && x[3]);
                case 0:
                default:
                    return x[4];
                }
            }

            std::string _5in_7SegmentDisplay::ctype() const
            {
                return "5IN_7SEGMENT";
            }

            // _8in_7SegmentDisplay
            _8in_7SegmentDisplay::_8in_7SegmentDisplay() : Output(8, 8) {}

            _8in_7SegmentDisplay::_8in_7SegmentDisplay(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, component::Component &input6, component::Component &input7, component::Component &input8, unsigned int input1_out, unsigned int input2_out, unsigned int input3_out, unsigned int input4_out, unsigned int input5_out, unsigned int input6_out, unsigned int input7_out, unsigned int input8_out) : Output(8, 8)
            {
                set_inputs(input1, input2, input3, input4, input5, input6, input7, input8, input1_out, input2_out, input3_out, input4_out, input5_out, input6_out, input7_out, input8_out);
            }

            void _8in_7SegmentDisplay::set_inputs(component::Component &input1, component::Component &input2, component::Component &input3, component::Component &input4, component::Component &input5, component::Component &input6, component::Component &input7, component::Component &input8, unsigned int input1_out, unsigned int input2_out, unsigned int input3_out, unsigned int input4_out, unsigned int input5_out, unsigned int input6_out, unsigned int input7_out, unsigned int input8_out)
            {
                set_input(0, input1, input1_out);
                set_input(1, input2, input2_out);
                set_input(2, input3, input3_out);
                set_input(3, input4, input4_out);
                set_input(4, input5, input5_out);
                set_input(5, input6, input6_out);
                set_input(6, input7, input7_out);
                set_input(7, input8, input8_out);
            }

            void _8in_7SegmentDisplay::clear()
            {
                Component::clear();
                _cached = false;
            }

            bool _8in_7SegmentDisplay::_evaluate(unsigned int out)
            {
                if (!_cached)
                {
                    for (size_t i = 0; i < 8; ++i)
                    {
                        _input_cache[i] = _inputs[i]->evaluate(_inputs_out[i]);
                    }
                }

                return out < 8 ? _input_cache[7 - out] : _input_cache[0];
            }

            std::string _8in_7SegmentDisplay::ctype() const
            {
                return "8IN_7SEGMENT";
            }
        }
    }
}
