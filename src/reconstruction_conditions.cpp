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
std::optional<double> ReconstructionConditions::StandardStripReconstructionCondition(const std::vector<StripHit>& hits, StripLayer layer){
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

std::optional<double> ReconstructionConditions::StripAlignmentedReconstructionCondition(const std::vector<StripHit>& hits, StripLayer layer){
    if(hits.size() < 2) return std::nullopt;

    std::vector<StripHit> sorted_hits = hits;

    std::sort(
        sorted_hits.begin(),
        sorted_hits.end(),
        [](const StripHit& a, const StripHit& b){
            return a.charge > b.charge;
        }
    );

    const StripHit& leading = sorted_hits[0];
    const StripHit& subleading = sorted_hits[1];

    if(std::abs(leading.position - subleading.position) != 1.0)
        return std::nullopt;

    const double total_charge =
        leading.charge + subleading.charge;

    if(total_charge == 0.0)
        return std::nullopt;

    const double reconstructed_position =
        (
            leading.position * leading.charge
            + subleading.position * subleading.charge
        ) / total_charge;

    double position =
        CorrectStripPosition(reconstructed_position);

    const AlignmentStripData ad;

    switch(layer){
        case StripLayer::FrontX:
        case StripLayer::FrontY:
            break;

        case StripLayer::BackX:
            position -= ad.x;
            break;

        case StripLayer::BackY:
            position -= ad.y;
            break;
    }

    return position;
}    

std::optional<double> ReconstructionConditions::NewAlignmentStripReconstructionCondition(const std::vector<StripHit>& hits, StripLayer layer){
    if(hits.size() < 2) return std::nullopt;

    std::vector<StripHit> sorted_hits = hits;

    std::sort(
        sorted_hits.begin(),
        sorted_hits.end(),
        [](const StripHit& a, const StripHit& b){
            return a.charge > b.charge;
        }
    );

    const StripHit& leading = sorted_hits[0];
    const StripHit& subleading = sorted_hits[1];

    if(std::abs(leading.position - subleading.position) != 1.0)
        return std::nullopt;

    const double total_charge =
        leading.charge + subleading.charge;

    if(total_charge == 0.0)
        return std::nullopt;

    const double reconstructed_position =
        (
            leading.position * leading.charge
            + subleading.position * subleading.charge
        ) / total_charge;

    double position =
        CorrectStripPosition(reconstructed_position);

    const NewAlignmentStripData ad;

    switch(layer){
        case StripLayer::FrontX:
        case StripLayer::FrontY:
            break;

        case StripLayer::BackX:
            position -= ad.x;
            break;

        case StripLayer::BackY:
            position -= ad.y;
            break;
    }

    return position;
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

std::optional<ReconstructedPixelPosition> ReconstructionConditions::SingleHitReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    if(hits.size() > 1) return std::nullopt; // Only consider single hit events

    const PixelHit& leading = hits[0];
    return CorrectPixelPosition(ReconstructedPixelPosition{static_cast<double>(leading.position.x), static_cast<double>(leading.position.y)});
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

std::optional<ReconstructedPixelPosition> ReconstructionConditions::Shower2HitEvtReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    double total_charge = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    if(hits.size() < 2) return std::nullopt; // Ensure there are at least 2 hits

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

std::optional<ReconstructedPixelPosition> ReconstructionConditions::ShowerSeedReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    std::vector<PixelHit> sorted_hits = hits;

    std::sort(sorted_hits.begin(), sorted_hits.end(), [](const PixelHit& a, const PixelHit& b) {
        return a.charge > b.charge; // Sort in descending order of charge
    });

    const PixelHit& seed = sorted_hits[0];

    double total_charge = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    int lgad;
    if(layer == PixelLayer::Front){
        lgad = 0;
    }else if(layer == PixelLayer::Back){
        lgad = 1;
    }else{
        return std::nullopt; // Invalid layer
    }

    for(int dx=-1; dx<=1; ++dx){
        for(int dy=-1; dy<=1; ++dy){

            PixelPosition neighbor_pos{seed.position.x + dx, seed.position.y + dy};

            if(neighbor_pos.x < 0 || neighbor_pos.x > 3 || neighbor_pos.y < 0 || neighbor_pos.y > 3) continue;

            const int diti_ch = PixelPosition2Digi(lgad, neighbor_pos);

            if(std::find(dead_channels.begin(), dead_channels.end(), diti_ch) != dead_channels.end()){
                return std::nullopt; // Found a dead channel in the 3x3 region
            }
        }
    }

    for(const auto& hit: sorted_hits){

        const int dx = std::abs(hit.position.x - seed.position.x);
        const int dy = std::abs(hit.position.y - seed.position.y);

        if(dx > 1 || dy > 1) continue; // Only consider hits in the 3x3 region around the seed
        
        total_charge += hit.charge;
        weighted_x += static_cast<double>(hit.position.x) * hit.charge;
        weighted_y += static_cast<double>(hit.position.y) * hit.charge;
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

std::optional<ReconstructedPixelPosition> ReconstructionConditions::ShowerSeedIncludeDeadchReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    std::vector<PixelHit> sorted_hits = hits;

    std::sort(sorted_hits.begin(), sorted_hits.end(), [](const PixelHit& a, const PixelHit& b) {
        return a.charge > b.charge; // Sort in descending order of charge
    });

    const PixelHit& seed = sorted_hits[0];

    double total_charge = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    int n_hit_region = 0;

    for(const auto& hit: sorted_hits){

        const int dx = std::abs(hit.position.x - seed.position.x);
        const int dy = std::abs(hit.position.y - seed.position.y);

        if(dx > 1 || dy > 1) continue; // Only consider hits in the 3x3 region around the seed

        ++n_hit_region;
        
        total_charge += hit.charge;
        weighted_x += static_cast<double>(hit.position.x) * hit.charge;
        weighted_y += static_cast<double>(hit.position.y) * hit.charge;
    }

    if(n_hit_region < 2) return std::nullopt; // Require at least 2 hits in the 3x3 region
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

std::optional<ReconstructedPixelPosition> ReconstructionConditions::ShowerLineReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    std::vector<PixelHit> sorted_hits = hits;

    std::sort(sorted_hits.begin(), sorted_hits.end(), [](const PixelHit& a, const PixelHit& b) {
        return a.charge > b.charge; // Sort in descending order of charge
    });

    const PixelHit& seed = sorted_hits[0];

    double total_charge_x = 0.0;
    double total_charge_y = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    int n_hit_x = 0;
    int n_hit_y = 0;

    for(const auto& hit: sorted_hits){

        if(hit.position.x == seed.position.x){
            weighted_y += static_cast<double>(hit.position.y) * hit.charge;
            total_charge_y += hit.charge;
            ++n_hit_y;
        }
        if(hit.position.y == seed.position.y){
            weighted_x += static_cast<double>(hit.position.x) * hit.charge;
            total_charge_x += hit.charge;
            ++n_hit_x;
        }
    }

    if(n_hit_x < 2 || n_hit_y < 2) return std::nullopt; // Require at least 2 hits in the same row or column
    if(total_charge_x == 0.0 || total_charge_y == 0.0) return std::nullopt; // Avoid division by zero

    double reconstructed_x = weighted_x / total_charge_x;
    double reconstructed_y = weighted_y / total_charge_y;

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

std::optional<ReconstructedPixelPosition> ReconstructionConditions::Shower2HitIncludeStripAlignmentReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    double total_charge = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    if(hits.size() < 2) return std::nullopt; // Ensure there are at least 2 hits

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

    const AlignmentIncludeStripData ad;

    if(layer == PixelLayer::Front){
        position.x -= ad.front_x;
        position.y -= ad.front_y;
    }else if(layer == PixelLayer::Back){
        position.x -= ad.back_x;
        position.y -= ad.back_y;
    }

    return position;
}

std::optional<ReconstructedPixelPosition> ReconstructionConditions::Shower2HitNewAlignmentReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    double total_charge = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    if(hits.size() < 2) return std::nullopt; // Ensure there are at least 2 hits

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

    const NewAlignmentDutData ad;

    if(layer == PixelLayer::Front){
        position.x -= ad.front_x;
        position.y -= ad.front_y;
    }else if(layer == PixelLayer::Back){
        position.x -= ad.back_x;
        position.y -= ad.back_y;
    }

    return position;
}

std::optional<ReconstructedPixelPosition> ReconstructionConditions::Shower2HitAdcWeightNewAlignmentReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer){
    if(hits.empty()) return std::nullopt;

    double total_adc = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    if(hits.size() < 2) return std::nullopt; // Ensure there are at least 2 hits

    for(const auto& hit: hits){
        const double weight = -hit.peak_adc;

        weighted_x += weight * static_cast<double>(hit.position.x);
        weighted_y += weight * static_cast<double>(hit.position.y);
        total_adc += weight;
    }

    if(total_adc == 0.0) return std::nullopt; // Avoid division by zero

    double reconstructed_x = weighted_x / total_adc;
    double reconstructed_y = weighted_y / total_adc;

    ReconstructedPixelPosition position = CorrectPixelPosition({reconstructed_x, reconstructed_y});

    const NewAlignmentDutData ad;

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

const ReconstructionConditions::StripReconstructionCondition ReconstructionConditions::StripAlignmented{
    "StripAlignmented",
    StripAlignmentedReconstructionCondition
};

const ReconstructionConditions::StripReconstructionCondition ReconstructionConditions::NewAlignmentStrip{
    "NewAlignmentStrip",
    NewAlignmentStripReconstructionCondition
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

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::SingleHit{
    "SingleHit",
    SingleHitReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::ShowerFullHit{
    "ShowerFullHit",
    ShowerFullHitReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::Shower2HitEvt{
    "Shower2HitEvt",
    Shower2HitEvtReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::ShowerSeed{
    "ShowerSeed",
    ShowerSeedReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::ShowerSeedIncludeDeadch{
    "ShowerSeedIncludeDeadch",
    ShowerSeedIncludeDeadchReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::ShowerLine{
    "ShowerLine",
    ShowerLineReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::Shower2HitIncludeStripAlignment{
    "Shower2HitIncludeStripAlignment",
    Shower2HitIncludeStripAlignmentReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::Shower2HitNewAlignment{
    "Shower2HitNewAlignment",
    Shower2HitNewAlignmentReconstructionCondition
};

const ReconstructionConditions::PixelReconstructionCondition ReconstructionConditions::Shower2HitAdcWeightNewAlignment{
    "Shower2HitAdcWeightNewAlignment",
    Shower2HitAdcWeightNewAlignmentReconstructionCondition
};