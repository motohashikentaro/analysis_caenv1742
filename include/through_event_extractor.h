#ifndef THROUGH_EVENT_EXTRACTOR_H
#define THROUGH_EVENT_EXTRACTOR_H

#include "./hit_reader.h"
#include "./reconstruction_conditions.h"

struct ThroughEvent{
    Long64_t evt;

    double strip_position_front_x;
    double strip_position_front_y;
    double strip_position_back_x;
    double strip_position_back_y;

    double dut_position_front_x;
    double dut_position_front_y;
    double dut_position_back_x;
    double dut_position_back_y;

    double extrapolated_front_x;
    double extrapolated_front_y;
    double extrapolated_back_x;
    double extrapolated_back_y;

    double slope_x;
    double slope_y;

    int n_hit_ch_front;
    int n_hit_ch_back;
};

class ThroughEventExtractor{
    public:
        ThroughEventExtractor(HitData& hd, const ReconstructionConditions::StripReconstructionCondition& strip_condition, const ReconstructionConditions::PixelReconstructionCondition& pixel_condition): 
            hd_(hd), strip_condition_(strip_condition), pixel_condition_(pixel_condition){};

        void ThroughEventExtraction();

    private:
        HitData& hd_;
        ReconstructionConditions::StripReconstructionCondition strip_condition_;
        ReconstructionConditions::PixelReconstructionCondition pixel_condition_;
};

#endif