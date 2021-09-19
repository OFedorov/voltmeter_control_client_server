#include "channel.h"

#include <cstdlib>

CHANNEL::Channel::Channel(): value(0), range(0), status(idle_state), m_status(idle_state)
{

}

void CHANNEL::Channel::update()
{
    update_status();
    if (status == measure_state){
        generate_value();
    }
}

void CHANNEL::Channel::generate_value()
{
    float val = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    const float *r;
    if (range == 0){
        r = range0;
    } else if (range == 1) {
        r = range1;
    } else if (range == 2) {
        r = range2;
    } else if (range == 3) {
        r = range3;
    } else {
        status = error_state;
    }
    value = (val/(1./(r[1] - r[0]))) + r[0];
}

void CHANNEL::Channel::update_status()
{
    if (status == error_state){
        return ;
    }

    if (status == busy_state){
        status = m_status;
    }

    if (status == measure_state){
        m_status = status;
        float err_p = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        if (err_p < 0.05){
            status = error_state;
            return ;
        }
        if (err_p < 0.30){
            status = busy_state;
        }
    }

    if (status == idle_state){
        m_status = status;
        float err_p = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        if (err_p < 0.25){
            status = busy_state;
        }
    }
}
