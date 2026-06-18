#include <array>

#include "./../include/pedestal_calcurate.h"

void PedestalCalc::MedianPedestal(){
    float pedestal = 0;
    std::array<float, 50> samples;
    for(int sample=0; sample<50; sample++){
        samples[sample] = rd_.ev_.amp[target_board][target_ch][sample];
    }
    std::sort(samples.begin(), samples.end());
    pedestal = (samples[24] + samples[25]) / 2.0;
}