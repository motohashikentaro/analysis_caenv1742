#ifndef RECONSTRUCTION_CONDITIONS_H
#define RECONSTRUCTION_CONDITIONS_H

#include "./hit_extractor.h"
#include "./channel_map.h"

#include <optional>
#include <vector>

enum class PixelLayer{
    Front,
    Back
};

struct StripHit{
    double position;
    double charge;
};

struct ReconstructedPixelPosition{
    double x;
    double y;
};

struct PixelHit{
    PixelPosition position;
    double charge;
};

class ReconstructionConditions{
    public:
        using StripReconstructionConditionFunction = std::optional<double> (*)(const std::vector<StripHit>& hits);
        using PixelReconstructionConditionFunction = std::optional<ReconstructedPixelPosition> (*)(const std::vector<PixelHit>& hits, PixelLayer layer);

        struct StripReconstructionCondition{
            const char* name;
            StripReconstructionConditionFunction condition;
        };
        struct PixelReconstructionCondition{
            const char* name;
            PixelReconstructionConditionFunction condition;
        };

        // strip
        static std::optional<double> StandardStripReconstructionCondition(const std::vector<StripHit>& hits);

        // pixel
        static std::optional<ReconstructedPixelPosition> Top3ChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> AdjacentChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> AxisNeighborChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);

        static std::optional<ReconstructedPixelPosition> ShowerFullHitReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);

        // named conditions
        static const StripReconstructionCondition StandardStrip;
        
        static const PixelReconstructionCondition Top3ChargeWeight;
        static const PixelReconstructionCondition AdjacentChargeWeight;
        static const PixelReconstructionCondition AxisNeighborChargeWeight;
        static const PixelReconstructionCondition ShowerFullHit;
    private:
        static double CorrectStripPosition(double strip_position);
        static ReconstructedPixelPosition CorrectPixelPosition(const ReconstructedPixelPosition& pixel_position);        
};

#endif