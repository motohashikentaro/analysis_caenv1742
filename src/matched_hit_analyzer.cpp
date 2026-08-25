#include "./../include/matched_hit_analyzer.h"
#include "./../include/hit_extractor.h"
#include "./../include/channel_map.h"

#include <vector>
#include <array>
#include <algorithm>

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

ReconstructedPixelPosition MatchedHitAnalyzer::ChargeWeightPixelPosition(const std::vector<PixelHit>& hits){
    double total_charge = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    std::vector<PixelHit> sorted_hits = hits;
    std::sort(sorted_hits.begin(), sorted_hits.end(), [](const PixelHit& a, const PixelHit& b) {
        return a.charge > b.charge; // Sort in descending order of charge
    });

    const size_t n_hits = std::min(static_cast<size_t>(3), sorted_hits.size());

    for(size_t i = 0; i<n_hits; ++i){
        const auto& hit = sorted_hits[i];
        const double weight = hit.charge;

        weighted_x += weight * static_cast<double>(hit.position.x);
        weighted_y += weight * static_cast<double>(hit.position.y);
        total_charge += weight;
    }

    if(total_charge == 0.0) return ReconstructedPixelPosition{-1.0, -1.0}; // Avoid division by zero

    return ReconstructedPixelPosition{weighted_x / total_charge, weighted_y / total_charge};
}

double MatchedHitAnalyzer::CorrectStripPosition(double strip_position){
    constexpr double pitch = 0.5; // mm
    constexpr double center = 3.5;

    return (strip_position - center) * pitch;
}

ReconstructedPixelPosition MatchedHitAnalyzer::CorrectPixelPosition(const ReconstructedPixelPosition& pixel_position){
    constexpr double pitch = 0.5; // mm
    constexpr double center = 1.5;

    return {(pixel_position.x - center) * pitch,(pixel_position.y - center) * pitch};
}

std::optional<ReconstructedHitPosition> MatchedHitAnalyzer::Reconstruct(const std::vector<EvtHit>& hits){
    if(!Is6Through(hits)) return std::nullopt;

    std::array<std::vector<StripHit>, 4> layer_hits;
    std::array<std::vector<PixelHit>, 2> pixel_hits;

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

            pixel_hits[lgad].push_back({pos, hit.charge});
        }
    }

    const double strip_position_front_x_idx = ChargeWeightStripPosition(layer_hits[0]);
    const double strip_position_front_y_idx = ChargeWeightStripPosition(layer_hits[1]);
    const double strip_position_back_x_idx = ChargeWeightStripPosition(layer_hits[2]);
    const double strip_position_back_y_idx = ChargeWeightStripPosition(layer_hits[3]);
    const ReconstructedPixelPosition dut_position_front_idx = ChargeWeightPixelPosition(pixel_hits[0]);
    const ReconstructedPixelPosition dut_position_back_idx = ChargeWeightPixelPosition(pixel_hits[1]);

    if(strip_position_front_x_idx < 0 || strip_position_front_y_idx < 0 ||
       strip_position_back_x_idx < 0 || strip_position_back_y_idx < 0 ||
       dut_position_front_idx.x < 0 || dut_position_front_idx.y < 0 ||
       dut_position_back_idx.x < 0 || dut_position_back_idx.y < 0){
        return std::nullopt; // Invalid reconstruction
    }

    const ReconstructedHitPosition rhe = {
        .strip_position_front_x = CorrectStripPosition(strip_position_front_x_idx),
        .strip_position_front_y = CorrectStripPosition(strip_position_front_y_idx),
        .strip_position_back_x = CorrectStripPosition(strip_position_back_x_idx),
        .strip_position_back_y = CorrectStripPosition(strip_position_back_y_idx),
        .dut_position_front = CorrectPixelPosition(dut_position_front_idx),
        .dut_position_back = CorrectPixelPosition(dut_position_back_idx),
        .n_hit_ch_front = static_cast<int>(pixel_hits[0].size()),
        .n_hit_ch_back = static_cast<int>(pixel_hits[1].size())
    };

    return rhe;
}