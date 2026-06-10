#include <iostream>
#include <filesystem>

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TApplication.h>

#include "./../include/rootfile_analyzer.h"

int main(int argc, char* argv[]){

    TFile* file = TFile::Open(argv[1]);
    if(file->IsZombie()){
        std::cerr << "Error: cannot open file: " << argv[1] << std::endl;
        return 1;
    }

    TTree* tree = (TTree*)file->Get("tree");
    if(!tree){
        std::cerr << "Error: cannot find tree" << std::endl;
        return 1;
    }

    event ev;
    loopn lp;
    tree->SetBranchAddress("ev_id", &ev.ev_id);
    for(int board=0; board<lp.nboard; board++){
        for(int ch=0; ch<lp.nch; ch++){
            tree->SetBranchAddress(Form("amp_b%d_ch%02d", board, ch), ev.amp[board][ch]);
        }
    }

    Long64_t nentries = tree->GetEntries();

    TApplication* app = new TApplication("app", &argc, argv);

    TCanvas* c = new TCanvas("c", "Signal Waveforms", 2800, 2600);
    c->Divide(8, 8);

    for(Long64_t evt=0; evt<nentries; evt++){
        tree->GetEntry(evt);

        TGraph* gr[2][32];
        for(int board=0; board<lp.nboard; board++){
            for(int ch=0; ch<lp.nch; ch++){
                gr[board][ch] = new TGraph();
            }
        }

        float pedestal[2][32] = {0};
        for(int sample=0; sample<lp.nsample; sample++){
            for(int board=0; board<lp.nboard; board++){
                for(int ch=0; ch<lp.nch; ch++){
                    float tmp_pedestal = 0.0;
                    float signal = 0.0;

                    if(sample<10){
                        pedestal[board][ch] += ev.amp[board][ch][sample];
                        tmp_pedestal = pedestal[board][ch] / (sample + 1);
                    }else if(sample==10){
                        pedestal[board][ch] /= 10.0;
                    }

                    if(sample<10){
                        signal = ev.amp[board][ch][sample] - tmp_pedestal;
                    }else{
                        signal = ev.amp[board][ch][sample] - pedestal[board][ch];
                    }

                    // insert signal processing, e.g., thresholding, ToT calculation, etc.

                    // 
                    gr[board][ch]->SetPoint(gr[board][ch]->GetN(), sample, signal);

                    // std::cout << "Event " << evt << ": Board " << board << ", Channel " << ch << ", Pedestal = " << pedestal[board][ch] << std::endl;
                }
            }
        }
        std::cout << evt << " " << gr[0][0]->GetN() << std::endl;
	    
        for(int board=0; board<lp.nboard; board++){
            for(int ch=0; ch<lp.nch; ch++){
                c->cd(board*lp.nch + ch + 1);
                gr[board][ch]->SetTitle(Form("Board %d Ch %02d", board, ch));
                gr[board][ch]->GetXaxis()->SetTitle("Sample");
                gr[board][ch]->GetYaxis()->SetTitle("Signal (ADC - Pedestal)");
                gr[board][ch]->SetMinimum(-500);
                gr[board][ch]->SetMaximum(200);
                if(evt == 0) gr[board][ch]->Draw("AP");
                if(evt > 0) gr[board][ch]->Draw("same");
            }
        }

        if(evt == 50) break;
    }

    std::cout << "Finished processing " << nentries << " events." << std::endl;

    c->Update();
    std::filesystem::create_directories("./../result");
    c->SaveAs("./../result/signal_waveforms.png");

    app->Run();

    return 0;
}
