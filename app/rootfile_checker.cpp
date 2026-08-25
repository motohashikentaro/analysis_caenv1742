#include "./../include/correlation_analyzer.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/hit_analyzer.h"
#include "./../include/rootfile_reader.h"
#include "./../include/rootfile_analyzer.h"

#include <array>
#include <limits>
#include <algorithm>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TColor.h>

int main(int argc, char* argv[]){

    RootfileReader rfr(argv[1]);

    int draw_count = 0;
    constexpr float threshold = -100.0f;

    TFile* output_file = new TFile("./../result/rootfile_checker_output.root", "RECREATE");
    
    for(Long64_t entry=0; entry<rfr.GetRootData().nentries_; entry++){
        if(draw_count >= 100) break;
        rfr.GetRootData().tree_->GetEntry(entry);

        int leading_count = -1;
        float leading_adc = std::numeric_limits<float>::max();
        float adc_front[16] = {};

        for(int ch=0; ch<16; ++ch){

            float peak_adc = std::numeric_limits<float>::max();

            for(int sample=110; sample<250; ++sample){
                
                peak_adc = std::min(peak_adc, rfr.GetRootData().ev_.amp[1][ch][sample]);
            }
            adc_front[ch] = peak_adc;

            if(peak_adc < leading_adc){
                leading_adc = peak_adc;
                leading_count = ch;
            }
        }

        if(leading_adc < threshold){
            TH2D* hist = new TH2D(Form("front_evt_%lld", entry), Form("Front DUT Event %lld;X;Y", rfr.GetRootData().ev_.ev_id), 4, 0, 4, 4, 0, 4);

            for(int ch=0; ch<16; ++ch){
                PixelPosition pos = Digi2PixelPosition(ch);
                hist->Fill(pos.x, pos.y, -adc_front[ch]);
            }
            hist->Write();
            delete hist;

            draw_count++;
        }
    }

    output_file->Write();
    output_file->Close();

    return 0;
}