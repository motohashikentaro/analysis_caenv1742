#include "./../include/hit_reader.h"
#include "./../include/through_event_extractor.h"
#include "./../include/reconstruction_conditions.h"

#include <iostream>
#include <exception>

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <hit_root_file>" << std::endl;
        return 1;
    }

    try{
        HitReader hr(argv[1]);

        const auto& strip_condition = ReconstructionConditions::StandardStrip;
        const auto& pixel_condition = ReconstructionConditions::ShowerFullHit;

        // through event extraction
        ThroughEventExtractor tee(hr.GetHitData(), strip_condition, pixel_condition);
        tee.ThroughEventExtraction();
    }catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}