#ifndef MATCHED_HIT_ANALYZER
#define MATCHED_HIT_ANALYZER

#include "./../include/channel_map.h"
#include "./../include/hit_extractor.h"
#include "./../include/hit_reader.h"
#include "./../include/reconstruction_conditions.h"

#include <vector>
#include <array>
#include <optional>

struct ReconstructedHitPosition{
    double strip_position_front_x;
    double strip_position_front_y;
    double strip_position_back_x;
    double strip_position_back_y;

    ReconstructedPixelPosition dut_position_front;
    ReconstructedPixelPosition dut_position_back;

    int n_hit_ch_front;
    int n_hit_ch_back;
};

struct MatchedHitAnalyzer{
    public:
        MatchedHitAnalyzer(HitData& hd, const ReconstructionConditions::StripReconstructionCondition& strip_condition, const ReconstructionConditions::PixelReconstructionCondition& pixel_condition): 
            hd_(hd), strip_condition_(strip_condition), pixel_condition_(pixel_condition){};

        bool Is4Through(const std::vector<EvtHit>& hits);
        bool Is6Through(const std::vector<EvtHit>& hits);

        double CorrectStripPosition(double strip_position);
        ReconstructedPixelPosition CorrectPixelPosition(const ReconstructedPixelPosition& pixel_position);

        std::optional<ReconstructedHitPosition> Reconstruct(const std::vector<EvtHit>& hits);

        template<typename Func>
        void EventLoop(Func process){

            std::vector<EvtHit> hits;
            Long64_t previous_evt = -1;

            for(Long64_t entry=0; entry<hd_.nentries_; entry++){
                hd_.tree_->GetEntry(entry);

                const Long64_t current_evt = hd_.eh_.evt;

                if(previous_evt == -1) previous_evt = current_evt;

                if(current_evt != previous_evt){

                    const auto rhe = Reconstruct(hits);

                    if(rhe) process(*rhe);

                    hits.clear();
                }

                hits.push_back(hd_.eh_);
                previous_evt = current_evt;
            }

            if(!hits.empty()){
                const auto rhe = Reconstruct(hits);

                if(rhe) process(*rhe);
            }
        }
    private:
        HitData& hd_;
        ReconstructionConditions::StripReconstructionCondition strip_condition_;
        ReconstructionConditions::PixelReconstructionCondition pixel_condition_;
};

#endif