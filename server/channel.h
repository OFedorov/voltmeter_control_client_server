#ifndef CHANNEL_H
#define CHANNEL_H

#include <map>
#include <string>
#include <vector>
//#include <functional>

namespace CHANNEL {
const std::string error_state = "error_state";
const std::string idle_state = "idle_state";
const std::string measure_state = "measure_state";
const std::string busy_state = "busy_state";

const float range0[2] = {0.0000001, 0.001};
const float range1[2] = {0.001, 1.};
const float range2[2] = {1., 1000.};
const float range3[2] = {1000., 1000000.};

class Channel{
public:
    Channel();
    void update();


    float value;
    uint range;
    std::string status;
private:
    void generate_value();
    void update_status();
    std::string m_status;
};
}


#endif // CHANNEL_H
