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

enum class StripLayer{
    FrontX,
    FrontY,
    BackX,
    BackY
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
    double peak_adc;
};

class ReconstructionConditions{
    public:
        using StripReconstructionConditionFunction = std::optional<double> (*)(const std::vector<StripHit>& hits, StripLayer layer);
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
        static std::optional<double> StandardStripReconstructionCondition(const std::vector<StripHit>& hits, StripLayer layer);
        static std::optional<double> StripAlignmentedReconstructionCondition(const std::vector<StripHit>& hits, StripLayer layer);
        static std::optional<double> NewAlignmentStripReconstructionCondition(const std::vector<StripHit>& hits, StripLayer layer);

        // pixel
        static std::optional<ReconstructedPixelPosition> Top3ChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> AdjacentChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> AxisNeighborChargeWeightReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> SingleHitReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);

        static std::optional<ReconstructedPixelPosition> ShowerFullHitReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> Shower2HitEvtReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> ShowerSeedReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> ShowerSeedIncludeDeadchReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> ShowerLineReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> Shower2HitIncludeStripAlignmentReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> Shower2HitNewAlignmentReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);
        static std::optional<ReconstructedPixelPosition> Shower2HitAdcWeightNewAlignmentReconstructionCondition(const std::vector<PixelHit>& hits, PixelLayer layer);

        // named conditions
        static const StripReconstructionCondition StandardStrip;
        static const StripReconstructionCondition StripAlignmented;
        static const StripReconstructionCondition NewAlignmentStrip;
        
        static const PixelReconstructionCondition Top3ChargeWeight;
        static const PixelReconstructionCondition AdjacentChargeWeight;
        static const PixelReconstructionCondition AxisNeighborChargeWeight;
        static const PixelReconstructionCondition SingleHit;

        static const PixelReconstructionCondition ShowerFullHit;
        static const PixelReconstructionCondition Shower2HitEvt;
        static const PixelReconstructionCondition ShowerSeed;
        static const PixelReconstructionCondition ShowerSeedIncludeDeadch;
        static const PixelReconstructionCondition ShowerLine;
        static const PixelReconstructionCondition Shower2HitIncludeStripAlignment;
        static const PixelReconstructionCondition Shower2HitNewAlignment;
        static const PixelReconstructionCondition Shower2HitAdcWeightNewAlignment;
        
    private:
        static double CorrectStripPosition(double strip_position);
        static ReconstructedPixelPosition CorrectPixelPosition(const ReconstructedPixelPosition& pixel_position);        
};

#endif