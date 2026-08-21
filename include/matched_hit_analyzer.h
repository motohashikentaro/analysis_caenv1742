#ifndef MATCHED_HIT_ANALYZER
#define MATCHED_HIT_ANALYZER

#include "./../include/channel_map.h"
#include "./../include/hit_extractor.h"
#include "./../include/hit_reader.h"

#include <vector>
#include <array>
#include <optional>

struct StripHit{
    double position;
    double charge;
};

struct ReconstructedHitPosition{
    double strip_position_front_x;
    double strip_position_front_y;
    double strip_position_back_x;
    double strip_position_back_y;

    std::array<std::vector<PixelPosition>, 2> pixel_positions;
};

struct MatchedHitAnalyzer{
    public:
        MatchedHitAnalyzer(HitData& hd): hd_(hd){};

        bool Is4Through(const std::vector<EvtHit>& hits);
        bool Is6Through(const std::vector<EvtHit>& hits);

        double ChargeWeightStripPosition(const std::vector<StripHit>& hits);

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
};

#endif