#ifndef LOGICSIM_MODEL_CONTROL_HPP
#define LOGICSIM_MODEL_CONTROL_HPP

#include <cmath>

#include "model/component.hpp"

namespace logicsim
{
    namespace model
    {
        namespace control
        {
            class Multiplexer : public component::NInputComponent
            {
            public:
                Multiplexer(unsigned int bits);

                std::string ctype() const override;

            protected:
                unsigned int _bits;
                Component **_E;
                unsigned int *_E_out;
                std::vector<Component **> _S;
                std::vector<unsigned int *> _S_out;
                std::vector<Component **> _D;
                std::vector<unsigned int *> _D_out;

                State _evaluate(unsigned int = 0) override;
            };

            class Decoder : public component::NInputComponent
            {
            public:
                Decoder(unsigned int bits);

                unsigned int n_outputs() const override;

                std::string ctype() const override;

            protected:
                unsigned int _bits, _outputs;
                Component **_E;
                unsigned int *_E_out;
                std::vector<Component **> _B;
                std::vector<unsigned int *> _B_out;

                State _evaluate(unsigned int out = 0) override;
            };

        }
    }
}

#endif // CONTROL_HPP
