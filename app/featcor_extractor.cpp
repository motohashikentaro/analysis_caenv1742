#include "./../include/feature_extractor.h"
#include "./../include/channel_map.h"

int main(int argc, char* argv[]){

    RootfileReader rfr(argv[1]);
    FeatureExtractor fe(rfr.GetRootData());
    fe.FeatcorExtraction(); 
    
    return 0;
}