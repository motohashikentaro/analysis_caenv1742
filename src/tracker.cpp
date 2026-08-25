#include "./../include/tracker.h"
#include "./../include/matched_hit_analyzer.h"

TrackingResult Tracker::Track(const ReconstructedHitPosition& rhp){
    double slope_x = (rhp.strip_position_back_x - rhp.strip_position_front_x) / 
                     (dop_.backstrip_x_z - dop_.frontstrip_x_z);
    double slope_y = (rhp.strip_position_back_y - rhp.strip_position_front_y) / 
                     (dop_.backstrip_y_z - dop_.frontstrip_y_z);

    double extrapolated_front_x = rhp.strip_position_front_x + slope_x * (dop_.dut_front_z - dop_.frontstrip_x_z);
    double extrapolated_front_y = rhp.strip_position_front_y + slope_y * (dop_.dut_front_z - dop_.frontstrip_y_z);
    double extrapolated_back_x = rhp.strip_position_front_x + slope_x * (dop_.dut_back_z - dop_.frontstrip_x_z);
    double extrapolated_back_y = rhp.strip_position_front_y + slope_y * (dop_.dut_back_z - dop_.frontstrip_y_z);

    TrackingResult tr;
    tr.extrapolated_front_x = extrapolated_front_x;
    tr.extrapolated_front_y = extrapolated_front_y;
    tr.extrapolated_back_x = extrapolated_back_x;
    tr.extrapolated_back_y = extrapolated_back_y;
    tr.slope_x = slope_x;
    tr.slope_y = slope_y;

    return tr;
}