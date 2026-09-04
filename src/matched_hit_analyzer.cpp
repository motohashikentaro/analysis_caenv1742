#include "./../include/matched_hit_analyzer.h"

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

    const auto strip_position_front_x = strip_condition_.condition(layer_hits[0], StripLayer::FrontX);
    const auto strip_position_front_y = strip_condition_.condition(layer_hits[1], StripLayer::FrontY);
    const auto strip_position_back_x = strip_condition_.condition(layer_hits[2], StripLayer::BackX);
    const auto strip_position_back_y = strip_condition_.condition(layer_hits[3], StripLayer::BackY);
    const auto dut_position_front = pixel_condition_.condition(pixel_hits[0], PixelLayer::Front);
    const auto dut_position_back = pixel_condition_.condition(pixel_hits[1], PixelLayer::Back);

    if(!strip_position_front_x || 
       !strip_position_front_y ||
       !strip_position_back_x ||
       !strip_position_back_y ||
       !dut_position_front || 
       !dut_position_back){
        return std::nullopt; // Invalid reconstruction
    }

    const ReconstructedHitPosition rhe = {
        .strip_position_front_x = *strip_position_front_x,
        .strip_position_front_y = *strip_position_front_y,
        .strip_position_back_x = *strip_position_back_x,
        .strip_position_back_y = *strip_position_back_y,
        .dut_position_front = *dut_position_front,
        .dut_position_back = *dut_position_back,
        .n_hit_ch_front = static_cast<int>(pixel_hits[0].size()),
        .n_hit_ch_back = static_cast<int>(pixel_hits[1].size())
    };

    return rhe;
}

std::optional<ReconstructedHitPosition> MatchedHitAnalyzer::StripReconstruct(const std::vector<EvtHit>& hits){
    if(!Is4Through(hits)) return std::nullopt;

    std::array<std::vector<StripHit>, 4> layer_hits;

    for(const auto& hit: hits){
        if(hit.board == 0){
            const StripPosition pos = Digi2CorrectStripPosition(hit.ch);

            if(pos.layer < 0 || pos.layer >= 4) continue;

            layer_hits[pos.layer].push_back({
                static_cast<double>(pos.strip),
                hit.charge
            });
        }
    }

    const auto strip_position_front_x = strip_condition_.condition(layer_hits[0], StripLayer::FrontX);
    const auto strip_position_front_y = strip_condition_.condition(layer_hits[1], StripLayer::FrontY);
    const auto strip_position_back_x = strip_condition_.condition(layer_hits[2], StripLayer::BackX);
    const auto strip_position_back_y = strip_condition_.condition(layer_hits[3], StripLayer::BackY);

    if(!strip_position_front_x || 
       !strip_position_front_y ||
       !strip_position_back_x ||
       !strip_position_back_y){
        return std::nullopt; // Invalid reconstruction
    }

    const ReconstructedHitPosition rhe = {
        .strip_position_front_x = *strip_position_front_x,
        .strip_position_front_y = *strip_position_front_y,
        .strip_position_back_x = *strip_position_back_x,
        .strip_position_back_y = *strip_position_back_y,
        .dut_position_front = {-999.0, -999.0},
        .dut_position_back = {-999.0, -999.0},
        .n_hit_ch_front = 0,
        .n_hit_ch_back = 0
    };

    return rhe;
}