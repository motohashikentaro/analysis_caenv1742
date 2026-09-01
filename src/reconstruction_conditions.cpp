#include "./../include/reconstruction_conditions.h"
#include "./../include/alignment_data.h"

#include <vector>
#include <optional>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <array>

double ReconstructionConditions::CorrectStripPosition(double strip_position){
    constexpr double pitch = 0.5; // mm
    constexpr double center = 3.5;

    return (strip_position - center) * pitch;
}

ReconstructedPixelPosition ReconstructionConditions::CorrectPixelPosition(
    const ReconstructedPixelPosition& pixel_position
){
    constexpr double pitch = 0.5; // mm
    constexpr double center = 1.5;

    return {
        (pixel_position.x - center) * pitch,
        (pixel_position.y - center) * pitch
    };
}

// strip
std::optional<double> ReconstructionConditions::StandardStripReconstructionCondition(const std::vector<StripHit>& hits){
    if(hits.size() < 2) return std::nullopt;
    // leading ch: max charge
    std::vector<StripHit> sorted_hits = hits;
    std::sort(sorted_hits.begin(), sorted_hits.end(), [](const StripHit& a, const StripHit& b) {
        return a.charge > b.charge; // Sort in descending order of charge
    });

    const StripHit& leading = sorted_hits[0];
    const StripHit& subleading = sorted_hits[1];

    if(std::abs(leading.position - subleading.position) != 1.0) return std::nullopt;

    const double total_charge = leading.charge + subleading.charge;
    if(total_charge == 0.0) return std::nullopt; // Avoid division

    const double reconstructed_position = (leading.position * leading.charge + subleading.position * subleading.charge) / total_charge;

    return CorrectStripPosition(reconstructed_position);
}

// pixel
// ---------------------------------------------------------------------------------------------------
std::optional<ReconstructedPixelPosition> ReconstructionConditions::Top3ChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
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

    if(total_charge == 0.0) return std::nullopt; // Avoid division by zero

    const double reconstructed_x = weighted_x / total_charge;
    const double reconstructed_y = weighted_y / total_charge;

    return CorrectPixelPosition(ReconstructedPixelPosition{reconstructed_x, reconstructed_y});
}

std::optional<ReconstructedPixelPosition> ReconstructionConditions::AdjacentChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;
    // leading ch: max charge
    std::vector<PixelHit> sorted_hits = hits;
    std::sort(sorted_hits.begin(), sorted_hits.end(), [](const PixelHit& a, const PixelHit& b) {
        return a.charge > b.charge; // Sort in descending order of charge
    });

    const PixelHit& leading = sorted_hits[0];

    std::vector<const PixelHit*> subleadings;

    for(size_t i=1; i<sorted_hits.size(); ++i){
        const PixelHit& hit = sorted_hits[i];

        const int dx = std::abs(hit.position.x - leading.position.x);
        const int dy = std::abs(hit.position.y - leading.position.y);

        if(dx + dy != 1) continue; // Only consider adjacent pixels

        subleadings.push_back(&hit);
    }

    double total_charge = leading.charge;
    double weighted_x = static_cast<double>(leading.position.x) * leading.charge;
    double weighted_y = static_cast<double>(leading.position.y) * leading.charge;
    
    for(const auto* hit: subleadings){
        total_charge += hit->charge;
        weighted_x += static_cast<double>(hit->position.x) * hit->charge;
        weighted_y += static_cast<double>(hit->position.y) * hit->charge;
    }

    if(total_charge == 0.0) return std::nullopt; // Avoid division by zero

    const double reconstructed_x = weighted_x / total_charge;
    const double reconstructed_y = weighted_y / total_charge;

    return CorrectPixelPosition(ReconstructedPixelPosition{reconstructed_x, reconstructed_y});
}

std::optional<ReconstructedPixelPosition> ReconstructionConditions::AxisNeighborChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;
    // leading ch: max charge
    std::vector<PixelHit> sorted_hits = hits;
    std::sort(sorted_hits.begin(), sorted_hits.end(), [](const PixelHit& a, const PixelHit& b) {
        return a.charge > b.charge; // Sort in descending order of charge
    });

    const PixelHit& leading = sorted_hits[0];

    const PixelHit* x_neighbor = nullptr;
    const PixelHit* y_neighbor = nullptr;

    for(const auto& hit: sorted_hits){
        if(&hit == &leading) continue;

        const int dx = std::abs(hit.position.x - leading.position.x);
        const int dy = std::abs(hit.position.y - leading.position.y);

        if(dx == 1 && dy == 0){
            if(x_neighbor == nullptr || hit.charge > x_neighbor->charge){
                x_neighbor = &hit;
            }
        }else if(dx == 0 && dy == 1){
            if(y_neighbor == nullptr || hit.charge > y_neighbor->charge){
                y_neighbor = &hit;
            }
        }
    }

    double reconstructed_x = static_cast<double>(leading.position.x);
    double reconstructed_y = static_cast<double>(leading.position.y);

    if(x_neighbor){
        const double total_charge = leading.charge + x_neighbor->charge;
        if(total_charge != 0.0){
            reconstructed_x = (leading.position.x * leading.charge + x_neighbor->position.x * x_neighbor->charge) / total_charge;
        }
    }
    if(y_neighbor){
        const double total_charge = leading.charge + y_neighbor->charge;
        if(total_charge != 0.0){
            reconstructed_y = (leading.position.y * leading.charge + y_neighbor->position.y * y_neighbor->charge) / total_charge;
        }
    }
    return CorrectPixelPosition(ReconstructedPixelPosition{reconstructed_x, reconstructed_y});
}

std::optional<ReconstructedPixelPosition> ReconstructionConditions::ShowerFullHitReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    double total_charge = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    for(const auto& hit: hits){
        const double weight = hit.charge;

        weighted_x += weight * static_cast<double>(hit.position.x);
        weighted_y += weight * static_cast<double>(hit.position.y);
        total_charge += weight;
    }

    if(total_charge == 0.0) return std::nullopt; // Avoid division by zero

    double reconstructed_x = weighted_x / total_charge;
    double reconstructed_y = weighted_y / total_charge;

    ReconstructedPixelPosition position = CorrectPixelPosition({reconstructed_x, reconstructed_y});

    const AlignmentData ad;

    if(layer == PixelLayer::Front){
        position.x -= ad.front_x;
        position.y -= ad.front_y;
    }else if(layer == PixelLayer::Back){
        position.x -= ad.back_x;
        position.y -= ad.back_y;
    }

    return position;
}


const ReconstructionConditions::StripReconstructionCondition ReconstructionConditions::StandardStrip{
    "Standard",
    StandardStripReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::Top3ChargeWeight{
    "Top3ChargeWeight",
    Top3ChargeWeightReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::AdjacentChargeWeight{
    "AdjacentChargeWeight",
    AdjacentChargeWeightReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::AxisNeighborChargeWeight{
    "AxisNeighborChargeWeight",
    AxisNeighborChargeWeightReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::ShowerFullHit{
    "ShowerFullHit",
    ShowerFullHitReconstructionCondition
};