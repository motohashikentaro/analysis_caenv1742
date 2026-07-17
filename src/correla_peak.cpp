#include "./../include/correla_peak.h"

#include <iostream>

#include <TCanvas.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TStyle.h>

CorrelaPeak::CorrelaPeak(
    const std::string& filename
)
    :
    file_(nullptr),
    tree_(nullptr),
    energy_(0),
    nw_(0)
{

    file_ = TFile::Open(
        filename.c_str(),
        "READ"
    );


    if(!file_ || file_->IsZombie()){

        std::cerr
            << "Cannot open "
            << filename
            << std::endl;

        return;
    }


    tree_ =
        (TTree*)file_->Get("tree");


    if(!tree_){

        std::cerr
            << "Cannot find tree"
            << std::endl;

        return;
    }


    SetBranchAddresses();

    CreateHistograms();

}



CorrelaPeak::~CorrelaPeak()
{

    if(file_){

        file_->Close();

        delete file_;

    }

}

void CorrelaPeak::SetBranchAddresses()
{

    tree_->SetBranchAddress(
        "event",
        &eh_.event
    );


    tree_->SetBranchAddress(
        "front_max_adc",
        &eh_.front_max_adc
    );


    tree_->SetBranchAddress(
        "back_max_adc",
        &eh_.back_max_adc
    );


    tree_->SetBranchAddress(
        "nfront",
        &eh_.nfront
    );


    tree_->SetBranchAddress(
        "nback",
        &eh_.nback
    );


    tree_->SetBranchAddress(
        "energy",
        &energy_
    );


    tree_->SetBranchAddress(
        "nw",
        &nw_
    );

}

void CorrelaPeak::CreateHistograms()
{

    for(int i=0;i<nenergy;i++){

        int energy=i+1;


        h_front_energy_all_[i] =
            new TH1D(
                Form("h_front_peak_energy_all_%d",energy),
                Form("Front Peak ADC E=%dGeV W=2;Peak ADC;Events",energy),
                100,
                -200,
                0
            );


        h_back_energy_all_[i] =
            new TH1D(
                Form("h_back_peak_energy_all_%d",energy),
                Form("Back Peak ADC E=%dGeV W=2;Peak ADC;Events",energy),
                100,
                -200,
                0
            );


        h_front_energy_sh_[i] =
            new TH1D(
                Form("h_front_peak_energy_sh_%d",energy),
                Form("Front Peak ADC Shower E=%dGeV W=2;Peak ADC;Events",energy),
                100,
                -200,
                0
            );


        h_back_energy_sh_[i] =
            new TH1D(
                Form("h_back_peak_energy_sh_%d",energy),
                Form("Back Peak ADC Shower E=%dGeV W=2;Peak ADC;Events",energy),
                100,
                -200,
                0
            );

    }



    for(int i=0;i<nnw;i++){

        int w=w_values[i];


        h_front_nw_all_[i] =
            new TH1D(
                Form("h_front_peak_w_all_%d",w),
                Form("Front Peak ADC E=2GeV W=%d;Peak ADC;Events",w),
                100,
                -200,
                0
            );


        h_back_nw_all_[i] =
            new TH1D(
                Form("h_back_peak_w_all_%d",w),
                Form("Back Peak ADC E=2GeV W=%d;Peak ADC;Events",w),
                100,
                -200,
                0
            );


        h_front_nw_sh_[i] =
            new TH1D(
                Form("h_front_peak_w_sh_%d",w),
                Form("Front Peak ADC Shower E=2GeV W=%d;Peak ADC;Events",w),
                100,
                -200,
                0
            );


        h_back_nw_sh_[i] =
            new TH1D(
                Form("h_back_peak_w_sh_%d",w),
                Form("Back Peak ADC Shower E=2GeV W=%d;Peak ADC;Events",w),
                100,
                -200,
                0
            );

    }

}

void CorrelaPeak::Analyze()
{

    Long64_t nentries =
        tree_->GetEntries();


    for(Long64_t i=0;i<nentries;i++){

        tree_->GetEntry(i);


        bool shower =
            (eh_.nfront>0 &&
             eh_.nback>0);



        // Energy dependence W=2

        if(nw_==2 &&
           energy_>=1 &&
           energy_<=nenergy){


            int idx=energy_-1;


            h_front_energy_all_[idx]
                ->Fill(eh_.front_max_adc);


            h_back_energy_all_[idx]
                ->Fill(eh_.back_max_adc);



            if(shower){

                h_front_energy_sh_[idx]
                    ->Fill(eh_.front_max_adc);


                h_back_energy_sh_[idx]
                    ->Fill(eh_.back_max_adc);

            }

        }



        // W dependence E=2

        if(energy_==2){

            int idx=-1;


            for(int j=0;j<nnw;j++){

                if(nw_==w_values[j]){

                    idx=j;

                    break;
                }

            }


            if(idx<0)
                continue;



            h_front_nw_all_[idx]
                ->Fill(eh_.front_max_adc);


            h_back_nw_all_[idx]
                ->Fill(eh_.back_max_adc);



            if(shower){

                h_front_nw_sh_[idx]
                    ->Fill(eh_.front_max_adc);


                h_back_nw_sh_[idx]
                    ->Fill(eh_.back_max_adc);

            }

        }

    }

}

void CorrelaPeak::DrawEnergy(
    const std::array<TH1D*, nenergy>& hists,
    const std::string& title,
    const std::string& outfile
)
{

    TCanvas* c =
        new TCanvas(
            "c_peak_energy",
            title.c_str(),
            800,
            600
        );


    gStyle->SetOptStat(0);


    int colors[nenergy] = {
        kBlack,
        kRed,
        kBlue,
        kGreen+2,
        kMagenta
    };


    TLegend* leg =
        new TLegend(
            0.65,
            0.65,
            0.88,
            0.88
        );


    TLatex latex;

    latex.SetNDC();
    latex.SetTextSize(0.03);


    double y = 0.85;


    for(int i=0;i<nenergy;i++){

        if(!hists[i])
            continue;


        hists[i]->SetLineColor(
            colors[i]
        );

        hists[i]->SetLineWidth(2);


        if(i==0){

            hists[i]->SetTitle(
                title.c_str()
            );

            hists[i]->Draw("hist");

        }
        else{

            hists[i]->Draw(
                "hist same"
            );

        }


        leg->AddEntry(
            hists[i],
            Form("%d GeV",i+1),
            "l"
        );


        latex.DrawLatex(
            0.15,
            y,
            Form(
                "%d GeV : N = %.0f, Mean = %.2f",
                i+1,
                hists[i]->GetEntries(),
                hists[i]->GetMean()
            )
        );

        y -= 0.04;

    }


    leg->Draw();


    c->SaveAs(
        outfile.c_str()
    );


    delete leg;
    delete c;

}

void CorrelaPeak::DrawNW(
    const std::array<TH1D*, nnw>& hists,
    const std::string& title,
    const std::string& outfile
)
{

    TCanvas* c =
        new TCanvas(
            "c_peak_nw",
            title.c_str(),
            800,
            600
        );


    gStyle->SetOptStat(0);


    int colors[nnw] = {
        kBlack,
        kRed,
        kBlue,
        kGreen+2,
        kMagenta,
        kOrange+1
    };


    TLegend* leg =
        new TLegend(
            0.65,
            0.65,
            0.88,
            0.88
        );


    TLatex latex;

    latex.SetNDC();
    latex.SetTextSize(0.03);


    double y = 0.85;


    for(int i=0;i<nnw;i++){

        if(!hists[i])
            continue;


        hists[i]->SetLineColor(
            colors[i]
        );

        hists[i]->SetLineWidth(2);


        if(i==0){

            hists[i]->SetTitle(
                title.c_str()
            );

            hists[i]->Draw("hist");

        }
        else{

            hists[i]->Draw(
                "hist same"
            );

        }


        leg->AddEntry(
            hists[i],
            Form(
                "W=%d",
                w_values[i]
            ),
            "l"
        );


        latex.DrawLatex(
            0.15,
            y,
            Form(
                "W=%d : N = %.0f, Mean = %.2f",
                w_values[i],
                hists[i]->GetEntries(),
                hists[i]->GetMean()
            )
        );


        y -= 0.04;

    }


    leg->Draw();


    c->SaveAs(
        outfile.c_str()
    );


    delete leg;
    delete c;

}

void CorrelaPeak::Draw()
{

    //==========================
    // Energy dependence
    //==========================

    DrawEnergy(
        h_front_energy_sh_,
        "Front Peak ADC Shower;Peak ADC;Events",
        "./../result/front_peak_energy_sh.png"
    );


    DrawEnergy(
        h_back_energy_sh_,
        "Back Peak ADC Shower;Peak ADC;Events",
        "./../result/back_peak_energy_sh.png"
    );


    DrawEnergy(
        h_front_energy_all_,
        "Front Peak ADC All;Peak ADC;Events",
        "./../result/front_peak_energy_all.png"
    );


    DrawEnergy(
        h_back_energy_all_,
        "Back Peak ADC All;Peak ADC;Events",
        "./../result/back_peak_energy_all.png"
    );



    //==========================
    // W dependence
    //==========================

    DrawNW(
        h_front_nw_sh_,
        "Front Peak ADC Shower;Peak ADC;Events",
        "./../result/front_peak_W_sh.png"
    );


    DrawNW(
        h_back_nw_sh_,
        "Back Peak ADC Shower;Peak ADC;Events",
        "./../result/back_peak_W_sh.png"
    );


    DrawNW(
        h_front_nw_all_,
        "Front Peak ADC All;Peak ADC;Events",
        "./../result/front_peak_W_all.png"
    );


    DrawNW(
        h_back_nw_all_,
        "Back Peak ADC All;Peak ADC;Events",
        "./../result/back_peak_W_all.png"
    );

}

void CorrelaPeak::Save(
    const std::string& outdir
)
{

    TFile fout(
        (outdir+"/peak_analysis.root").c_str(),
        "RECREATE"
    );



    for(auto h : h_front_energy_all_)
        h->Write();

    for(auto h : h_back_energy_all_)
        h->Write();


    for(auto h : h_front_energy_sh_)
        h->Write();

    for(auto h : h_back_energy_sh_)
        h->Write();



    for(auto h : h_front_nw_all_)
        h->Write();

    for(auto h : h_back_nw_all_)
        h->Write();


    for(auto h : h_front_nw_sh_)
        h->Write();

    for(auto h : h_back_nw_sh_)
        h->Write();



    fout.Close();

}

