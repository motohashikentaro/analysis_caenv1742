#include "./../include/matched_hit_analyzer.h"
#include "./../include/hit_extractor.h"

bool MatchedHitAnalyzer::Is4Through(const std::vector<EvtHit>& hits){
    bool front_x = false;
    bool front_y = false;
    bool back_x = false;
    bool back_y = false;

    for(const auto& hit: hits){
        if(hit.board == 0 && hit.ch >= 0 && hit.ch < 8) front_x = true;
        if(hit.board == 0 && hit.ch >= 8 && hit.ch < 16) front_y = true;
        if(hit.board == 0 && hit.ch >= 16 && hit.ch < 24) back_x = true;
        if(hit.board == 0 && hit.ch >= 24 && hit.ch < 32) back_y = true;
    }

    return front_x && front_y && back_x && back_y;
}

bool MatchedHitAnalyzer::Is6Through(const std::vector<EvtHit>& hits){
    bool front_x = false;
    bool front_y = false;
    bool back_x = false;
    bool back_y = false;
    bool dut_front = false;
    bool dut_back = false;

    for(const auto& hit: hits){
        if(hit.board == 0 && hit.ch >= 0 && hit.ch < 8) front_x = true;
        if(hit.board == 0 && hit.ch >= 8 && hit.ch < 16) front_y = true;
        if(hit.board == 0 && hit.ch >= 16 && hit.ch < 24) back_x = true;
        if(hit.board == 0 && hit.ch >= 24 && hit.ch < 32) back_y = true;
        if(hit.board == 1 && hit.ch >= 0 && hit.ch < 16) dut_front = true;
        if(hit.board == 1 && hit.ch >= 16 && hit.ch < 32) dut_back = true;
    }

    return front_x && front_y && back_x && back_y && dut_front && dut_back;
}

double MatchedHitAnalyzer::ChargeWeightStripPosition(const std::vector<StripHit>& hits){
    double total_charge = 0.0;
    double weighted_position = 0.0;

    for(const auto& hit: hits){
        const double weight = hit.charge;

        weighted_position += weight * static_cast<double>(hit.position);
        total_charge += weight;
    }

    if(total_charge == 0.0) return -1.0; // Avoid division by zero

    return weighted_position / total_charge; // Return the charge-weighted average position
}

std::optional<ReconstructedHitPosition> MatchedHitAnalyzer::Reconstruct(const std::vector<EvtHit>& hits){
    if(!Is6Through(hits)) return std::nullopt;

    std::array<std::vector<StripHit>, 4> layer_hits;
    std::array<std::vector<PixelPosition>, 2> pixel_hits;

    for(const auto& hit: hits){
        if(hit.board == 0){
            const StripPosition pos = Digi2CorrectStripPosition(hit.ch);

            if(pos.layer < 0 || pos.layer >= 4) continue;

            layer_hits[pos.layer].push_back({
                static_cast<double>(pos.strip),
                hit.charge
            });
        }else if(hit.board == 1){
            const int lgad = Digi2Lgad(hit.ch);
            const PixelPosition pos = Digi2PixelPosition(hit.ch);

            if(lgad < 0 || lgad >= 2) continue;
            if(pos.x < 0 || pos.y < 0) continue;
            
            pixel_hits[lgad].push_back(pos);
        }
    }

    const ReconstructedHitPosition rhe = {
        ChargeWeightStripPosition(layer_hits[0]),
        ChargeWeightStripPosition(layer_hits[1]),
        ChargeWeightStripPosition(layer_hits[2]),
        ChargeWeightStripPosition(layer_hits[3]),
        pixel_hits
    };

    if(rhe.strip_position_front_x < 0 || rhe.strip_position_front_y < 0 ||
       rhe.strip_position_back_x < 0 || rhe.strip_position_back_y < 0){
        return std::nullopt; // Invalid reconstruction
    }

    return rhe;
}