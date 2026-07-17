#include "./../include/correla_multiplicity.h"

#include <iostream>

#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TGraph.h>

CorrelaMultiplicity::CorrelaMultiplicity(
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
            << "Cannot open file: "
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

CorrelaMultiplicity::~CorrelaMultiplicity()
{

    if(file_){

        file_->Close();

        delete file_;

        file_ = nullptr;
    }

}

void CorrelaMultiplicity::SetBranchAddresses()
{

    tree_->SetBranchAddress(
        "event",
        &eh_.event
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

void CorrelaMultiplicity::CreateHistograms()
{

    constexpr int nbins = 17;

    constexpr double xmin = -0.5;

    constexpr double xmax = 16.5;

    //------------------------------
    // Energy dependence (W = 2)
    //------------------------------

    for(int i=0; i<nenergy; i++){

        int energy = i + 1;

        h_front_energy_all_[i] =
            new TH1D(
                Form("h_front_energy_all_%d", energy),
                Form("Front Multiplicity E=%dGeV W=2;Multiplicity;Events", energy),
                nbins,
                xmin,
                xmax
            );

        h_back_energy_all_[i] =
            new TH1D(
                Form("h_back_energy_all_%d", energy),
                Form("Back Multiplicity E=%dGeV W=2;Multiplicity;Events", energy),
                nbins,
                xmin,
                xmax
            );

        h_front_energy_sh_[i] =
            new TH1D(
                Form("h_front_energy_sh_%d", energy),
                Form("Front Multiplicity Shower E=%dGeV W=2;Multiplicity;Events", energy),
                nbins,
                xmin,
                xmax
            );

        h_back_energy_sh_[i] =
            new TH1D(
                Form("h_back_energy_sh_%d", energy),
                Form("Back Multiplicity Shower E=%dGeV W=2;Multiplicity;Events", energy),
                nbins,
                xmin,
                xmax
            );
    }

    //------------------------------
    // W dependence (Energy = 2)
    //------------------------------

    for(int i=0; i<nnw; i++){

        int w = w_values[i];

        h_front_nw_all_[i] =
            new TH1D(
                Form("h_front_nw_all_%d", w),
                Form("Front Multiplicity E=2GeV W=%d;Multiplicity;Events", w),
                nbins,
                xmin,
                xmax
            );

        h_back_nw_all_[i] =
            new TH1D(
                Form("h_back_nw_all_%d", w),
                Form("Back Multiplicity E=2GeV W=%d;Multiplicity;Events", w),
                nbins,
                xmin,
                xmax
            );

        h_front_nw_sh_[i] =
            new TH1D(
                Form("h_front_nw_sh_%d", w),
                Form("Front Multiplicity Shower E=2GeV W=%d;Multiplicity;Events", w),
                nbins,
                xmin,
                xmax
            );

        h_back_nw_sh_[i] =
            new TH1D(
                Form("h_back_nw_sh_%d", w),
                Form("Back Multiplicity Shower E=2GeV W=%d;Multiplicity;Events", w),
                nbins,
                xmin,
                xmax
            );
    }

}

void CorrelaMultiplicity::Analyze()
{

    Long64_t nentries =
        tree_->GetEntries();

    for(Long64_t i=0; i<nentries; i++){

        tree_->GetEntry(i);

        bool shower =
            (eh_.nfront > 0 &&
             eh_.nback  > 0);

        //------------------------------
        // Energy dependence (W = 2)
        //------------------------------

        if(nw_ == 2 &&
           energy_ >= 1 &&
           energy_ <= nenergy){

            int idx = energy_ - 1;

            h_front_energy_all_[idx]
                ->Fill(eh_.nfront);

            h_back_energy_all_[idx]
                ->Fill(eh_.nback);

            if(shower){

                h_front_energy_sh_[idx]
                    ->Fill(eh_.nfront);

                h_back_energy_sh_[idx]
                    ->Fill(eh_.nback);
            }
        }

        //------------------------------
        // W dependence (Energy = 2)
        //------------------------------

        if(energy_ == 2){

            int idx = -1;

            for(int j=0; j<nnw; j++){

                if(nw_ == w_values[j]){

                    idx = j;

                    break;
                }
            }

            if(idx < 0)
                continue;

            h_front_nw_all_[idx]
                ->Fill(eh_.nfront);

            h_back_nw_all_[idx]
                ->Fill(eh_.nback);

            if(shower){

                h_front_nw_sh_[idx]
                    ->Fill(eh_.nfront);

                h_back_nw_sh_[idx]
                    ->Fill(eh_.nback);
            }
        }
    }

}

void CorrelaMultiplicity::Draw()
{

    //=========================
    // Energy dependence
    //=========================

    DrawEnergy(
        h_front_energy_all_,
        "Front Multiplicity (All);Multiplicity;Events",
        "./../result/front_multiplicity_energy_all.png"
    );

    DrawEnergy(
        h_back_energy_all_,
        "Back Multiplicity (All);Multiplicity;Events",
        "./../result/back_multiplicity_energy_all.png"
    );

    DrawEnergy(
        h_front_energy_sh_,
        "Front Multiplicity (Shower);Multiplicity;Events",
        "./../result/front_multiplicity_energy_sh.png"
    );

    DrawEnergy(
        h_back_energy_sh_,
        "Back Multiplicity (Shower);Multiplicity;Events",
        "./../result/back_multiplicity_energy_sh.png"
    );

    DrawEnergySummary(
        h_front_energy_sh_,
        h_back_energy_sh_,
        "./../result"
    );



    //=========================
    // W dependence
    //=========================

    DrawNW(
        h_front_nw_all_,
        "Front Multiplicity (All);Multiplicity;Events",
        "./../result/front_multiplicity_W_all.png"
    );

    DrawNW(
        h_back_nw_all_,
        "Back Multiplicity (All);Multiplicity;Events",
        "./../result/back_multiplicity_W_all.png"
    );

    DrawNW(
        h_front_nw_sh_,
        "Front Multiplicity (Shower);Multiplicity;Events",
        "./../result/front_multiplicity_W_sh.png"
    );

    DrawNW(
        h_back_nw_sh_,
        "Back Multiplicity (Shower);Multiplicity;Events",
        "./../result/back_multiplicity_W_sh.png"
    );

    DrawNWSummary(
        h_front_nw_sh_,
        h_back_nw_sh_,
        "./../result"
    );

}

void CorrelaMultiplicity::Save(
    const std::string& outdir
)
{

    TFile fout(
        (outdir + "/correlation_multiplicity.root").c_str(),
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

void CorrelaMultiplicity::DrawEnergy(
    const std::array<TH1D*, nenergy>& hists,
    const std::string& title,
    const std::string& outfile
)
{
    gStyle->SetOptStat(0);
    TCanvas* c =
        new TCanvas(
            "c_energy",
            title.c_str(),
            800,
            600
        );


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


    for(int i=0; i<nenergy; i++){

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
                "%d GeV",
                i+1
            ),
            "l"
        );

    }

    TLatex latex;

    latex.SetNDC();
    latex.SetTextSize(0.035);


    double y = 0.55;

    for(int i=0; i<nenergy; i++){

        if(!hists[i])
            continue;


        latex.DrawLatex(
            0.55,
            y,
            Form(
                "%d GeV : N=%lld Mean=%.2f",
                i+1,
                (Long64_t)hists[i]->GetEntries(),
                hists[i]->GetMean()
            )
        );


        y -= 0.05;

    }


    leg->Draw();


    c->SaveAs(
        outfile.c_str()
    );


    delete leg;
    delete c;

}

void CorrelaMultiplicity::DrawNW(
    const std::array<TH1D*, nnw>& hists,
    const std::string& title,
    const std::string& outfile
)
{

    gStyle->SetOptStat(0);
    TCanvas* c =
        new TCanvas(
            "c_nw",
            title.c_str(),
            800,
            600
        );


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


    for(int i=0; i<nnw; i++){

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
                "W = %d",
                w_values[i]
            ),
            "l"
        );

    }

    TLatex latex;

    latex.SetNDC();
    latex.SetTextSize(0.035);


    double y = 0.55;


    for(int i=0; i<nnw; i++){

        if(!hists[i])
            continue;


        latex.DrawLatex(
            0.55,
            y,
            Form(
                "W=%d : N=%lld Mean=%.2f",
                w_values[i],
                (Long64_t)hists[i]->GetEntries(),
                hists[i]->GetMean()
            )
        );


        y -= 0.05;

    }

    leg->Draw();


    c->SaveAs(
        outfile.c_str()
    );


    delete leg;
    delete c;

}

void CorrelaMultiplicity::DrawNWSummary(
    const std::array<TH1D*, nnw>& front_hists,
    const std::array<TH1D*, nnw>& back_hists,
    const std::string& outdir
)
{

    double x[nnw];


    double front_mean[nnw];
    double back_mean[nnw];

    double front_entries[nnw];
    double back_entries[nnw];


    for(int i=0; i<nnw; i++){

        x[i] = w_values[i];


        front_mean[i] =
            front_hists[i]->GetMean();

        back_mean[i] =
            back_hists[i]->GetMean();


        front_entries[i] =
            front_hists[i]->GetEntries();

        back_entries[i] =
            back_hists[i]->GetEntries();

    }



    // Mean

    TGraph* g_front_mean =
        new TGraph(
            nnw,
            x,
            front_mean
        );


    TGraph* g_back_mean =
        new TGraph(
            nnw,
            x,
            back_mean
        );


    TCanvas* c1 =
        new TCanvas(
            "c_w_mean",
            "W Mean",
            800,
            600
        );


    g_front_mean->SetMarkerStyle(20);
    g_front_mean->SetMarkerColor(kRed);
    g_front_mean->SetLineColor(kRed);

    g_back_mean->SetMarkerStyle(21);
    g_back_mean->SetMarkerColor(kBlue);
    g_back_mean->SetLineColor(kBlue);


    g_front_mean->SetTitle(
        "Mean Multiplicity vs W;W thickness;Mean Multiplicity"
    );


    g_front_mean->Draw("AP");
    g_front_mean->GetYaxis()->SetRangeUser(
        0,
        8
    );
    g_back_mean->Draw("P SAME");


    TLegend* leg1 =
        new TLegend(
            0.65,
            0.75,
            0.85,
            0.85
        );


    leg1->AddEntry(g_front_mean,"Front","p");
    leg1->AddEntry(g_back_mean,"Back","p");

    leg1->Draw();


    c1->SaveAs(
        (outdir+"/front_back_mean_W.png").c_str()
    );



    // Entries

    TGraph* g_front_entries =
        new TGraph(
            nnw,
            x,
            front_entries
        );


    TGraph* g_back_entries =
        new TGraph(
            nnw,
            x,
            back_entries
        );


    TCanvas* c2 =
        new TCanvas(
            "c_w_entries",
            "W Entries",
            800,
            600
        );


    g_front_entries->SetMarkerStyle(20);
    g_front_entries->SetMarkerColor(kRed);
    g_front_entries->SetLineColor(kRed);

    g_back_entries->SetMarkerStyle(21);
    g_back_entries->SetMarkerColor(kBlue);
    g_back_entries->SetLineColor(kBlue);


    g_front_entries->SetTitle(
        "Entries vs W;W thickness;Entries"
    );


    g_front_entries->Draw("AP");
        g_front_entries->GetYaxis()->SetRangeUser(
        0,
        6000
    );
    g_back_entries->Draw("P SAME");


    TLegend* leg2 =
        new TLegend(
            0.65,
            0.75,
            0.85,
            0.85
        );


    leg2->AddEntry(g_front_entries,"Front","p");
    leg2->AddEntry(g_back_entries,"Back","p");

    leg2->Draw();


    c2->SaveAs(
        (outdir+"/front_back_entries_W.png").c_str()
    );

}

void CorrelaMultiplicity::DrawEnergySummary(
    const std::array<TH1D*, nenergy>& front_hists,
    const std::array<TH1D*, nenergy>& back_hists,
    const std::string& outdir
)
{

    double x[nenergy];


    double front_mean[nenergy];
    double back_mean[nenergy];

    double front_entries[nenergy];
    double back_entries[nenergy];


    for(int i=0; i<nenergy; i++){

        x[i] = i + 1;


        front_mean[i] =
            front_hists[i]->GetMean();

        back_mean[i] =
            back_hists[i]->GetMean();


        front_entries[i] =
            front_hists[i]->GetEntries();

        back_entries[i] =
            back_hists[i]->GetEntries();

    }



    //========================
    // Mean
    //========================

    TGraph* g_front_mean =
        new TGraph(
            nenergy,
            x,
            front_mean
        );


    TGraph* g_back_mean =
        new TGraph(
            nenergy,
            x,
            back_mean
        );


    TCanvas* c1 =
        new TCanvas(
            "c_energy_mean",
            "Energy Mean",
            800,
            600
        );


    g_front_mean->SetMarkerStyle(20);
    g_front_mean->SetMarkerColor(kRed);
    g_front_mean->SetLineColor(kRed);

    g_back_mean->SetMarkerStyle(21);
    g_back_mean->SetMarkerColor(kBlue);
    g_back_mean->SetLineColor(kBlue);


    g_front_mean->SetTitle(
        "Mean Multiplicity vs Energy;Energy [GeV];Mean Multiplicity"
    );

    g_front_mean->Draw("AP");
    g_front_mean->GetYaxis()->SetRangeUser(
        0,
        8
    );
    g_back_mean->Draw("P SAME");


    TLegend* leg1 =
        new TLegend(
            0.65,
            0.75,
            0.85,
            0.85
        );


    leg1->AddEntry(
        g_front_mean,
        "Front",
        "p"
    );

    leg1->AddEntry(
        g_back_mean,
        "Back",
        "p"
    );

    leg1->Draw();


    c1->SaveAs(
        (outdir+"/front_back_mean_energy.png").c_str()
    );



    //========================
    // Entries
    //========================

    TGraph* g_front_entries =
        new TGraph(
            nenergy,
            x,
            front_entries
        );


    TGraph* g_back_entries =
        new TGraph(
            nenergy,
            x,
            back_entries
        );


    TCanvas* c2 =
        new TCanvas(
            "c_energy_entries",
            "Energy Entries",
            800,
            600
        );


    g_front_entries->SetMarkerStyle(20);
    g_front_entries->SetMarkerColor(kRed);
    g_front_entries->SetLineColor(kRed);

    g_back_entries->SetMarkerStyle(21);
    g_back_entries->SetMarkerColor(kBlue);
    g_back_entries->SetLineColor(kBlue);


    g_front_entries->SetTitle(
        "Entries vs Energy;Energy [GeV];Entries"
    );


    g_front_entries->Draw("AP");
    g_front_entries->GetYaxis()->SetRangeUser(
        0,
        6000
    );
    g_back_entries->Draw("P SAME");


    TLegend* leg2 =
        new TLegend(
            0.65,
            0.75,
            0.85,
            0.85
        );


    leg2->AddEntry(
        g_front_entries,
        "Front",
        "p"
    );

    leg2->AddEntry(
        g_back_entries,
        "Back",
        "p"
    );


    leg2->Draw();


    c2->SaveAs(
        (outdir+"/front_back_entries_energy.png").c_str()
    );

}