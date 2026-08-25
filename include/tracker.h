#ifndef TRACKER_H
#define TRACKER_H

#include "./../include/channel_map.h"
#include "./../include/hit_extractor.h"
#include "./../include/hit_reader.h"
#include "./../include/matched_hit_analyzer.h"

#include <optional>

struct DetectorsOpticalPosition{
    double frontstrip_x_z = -22.0;
    double frontstrip_y_z = 2.0;
    double backstrip_x_z = 353.0;
    double backstrip_y_z = 377.0;
    double dut_front_z = 468.5;
    double dut_back_z = 488.0;
};

struct TrackingResult{
    double extrapolated_front_x;
    double extrapolated_front_y;
    double extrapolated_back_x;
    double extrapolated_back_y;

    double slope_x;
    double slope_y;
};

class Tracker{
    public:

        TrackingResult Track(const ReconstructedHitPosition& rhp);

    private:
        DetectorsOpticalPosition dop_;
};

#endif