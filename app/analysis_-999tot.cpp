#include "./../include/feature_reader.h"
#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>

#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>

int main(int argc, char* argv[]){

    if(argc != 3){
        std::cerr
            << "Usage: " << argv[0]
            << " <feature.root> <through_event.root>"
            << std::endl;
        return 1;
    }

    FeatureReader fr(argv[1]);
    ThroughEventReader tr(argv[2]);

    FeatureData& fd = fr.GetFeatureData();
    ThroughEventData& td = tr.GetThroughEventData();

    // ============================================================
    // through_event.root から6層貫通イベント番号を取得
    // ============================================================

    std::unordered_set<Long64_t> through_events;

    Long64_t td_evt;

    td.tree_->SetBranchAddress("evt", &td_evt);

    for(Long64_t i = 0; i < td.tree_->GetEntries(); ++i){

        td.tree_->GetEntry(i);

        through_events.insert(td_evt);
    }

    std::cout
        << "Number of 6-through events : "
        << through_events.size()
        << '\n';

    // ============================================================
    // feature.root
    // ============================================================

    Long64_t fd_evt;
    int board;
    int ch;
    double peak_adc;

    fd.tree_->SetBranchAddress("evt", &fd_evt);
    fd.tree_->SetBranchAddress("board", &board);
    fd.tree_->SetBranchAddress("ch", &ch);
    fd.tree_->SetBranchAddress("peak_adc", &peak_adc);

    // -20 ADCを1つ以上持つ6層貫通イベント
    std::unordered_set<Long64_t> through_events_with_minus20;

    // 各イベントに peak_adc==-20 のDUT chが何個あったか
    std::unordered_map<Long64_t, int> n_minus20_ch_per_event;

    for(Long64_t i = 0; i < fd.nentries_; ++i){

        fd.tree_->GetEntry(i);

        // 6層貫通イベントでなければ無視
        if(through_events.find(fd_evt) == through_events.end()){
            continue;
        }

        // DUTだけ
        if(board != 1){
            continue;
        }

        // threshold 20 ADCにちょうど一致
        if(peak_adc != -20.0){
            continue;
        }

        through_events_with_minus20.insert(fd_evt);

        ++n_minus20_ch_per_event[fd_evt];
    }

    // ============================================================
    // 結果
    // ============================================================

    const size_t n_through =
        through_events.size();

    const size_t n_through_with_minus20 =
        through_events_with_minus20.size();

    const double fraction =
        n_through > 0
            ? 100.0 *
              static_cast<double>(n_through_with_minus20) /
              static_cast<double>(n_through)
            : 0.0;

    std::cout
        << "========================================\n"
        << "6-through event / peak_adc == -20 check\n"
        << "========================================\n"
        << "6-through events             : "
        << n_through << '\n'
        << "With >=1 DUT peak_adc == -20 : "
        << n_through_with_minus20 << '\n'
        << "Without DUT peak_adc == -20  : "
        << n_through - n_through_with_minus20 << '\n'
        << "Fraction                     : "
        << fraction << " %\n";

    // ============================================================
    // Histogram 1
    // 6層貫通イベント中、-20 ADC DUT ch が何個あったか
    // ============================================================

    TH1D* hist_n_minus20 = new TH1D(
        "n_minus20_dut_ch",
        "DUT Channels with Peak ADC = -20 in 6-Through Events;"
        "Number of DUT channels with Peak ADC = -20;"
        "Events",
        33, -0.5, 32.5
    );

    for(const Long64_t evt : through_events){

        int n_minus20 = 0;

        auto it = n_minus20_ch_per_event.find(evt);

        if(it != n_minus20_ch_per_event.end()){
            n_minus20 = it->second;
        }

        hist_n_minus20->Fill(n_minus20);
    }

    // ============================================================
    // Histogram 2
    // -20 ADCあり / なし
    // ============================================================

    TH1D* hist_summary = new TH1D(
        "minus20_summary",
        "Peak ADC = -20 in 6-Through Events;;Events",
        2, 0.5, 2.5
    );

    hist_summary->GetXaxis()->SetBinLabel(
        1, "Without -20"
    );

    hist_summary->GetXaxis()->SetBinLabel(
        2, "With -20"
    );

    hist_summary->SetBinContent(
        1,
        n_through - n_through_with_minus20
    );

    hist_summary->SetBinContent(
        2,
        n_through_with_minus20
    );

    // ============================================================
    // Save
    // ============================================================

    PlotSupporter ps(td);

    const std::filesystem::path save_dir =
        ps.MakeSaveDir();

    TFile* fout = TFile::Open(
        (save_dir / "minus20_through_event_check.root")
            .c_str(),
        "RECREATE"
    );

    hist_n_minus20->Write();
    hist_summary->Write();

    TCanvas* canvas1 =
        new TCanvas(
            "canvas1",
            "Number of -20 ADC channels",
            1000,
            700
        );

    hist_n_minus20->Draw("HIST");

    canvas1->SaveAs(
        (
            save_dir /
            "n_minus20_dut_ch_in_through_event.png"
        ).string().c_str()
    );

    TCanvas* canvas2 =
        new TCanvas(
            "canvas2",
            "With / Without -20 ADC",
            800,
            700
        );

    hist_summary->Draw("HIST TEXT");

    canvas2->SaveAs(
        (
            save_dir /
            "minus20_through_event_summary.png"
        ).string().c_str()
    );

    fout->Close();

    delete canvas1;
    delete canvas2;
    delete hist_n_minus20;
    delete hist_summary;
    delete fout;

    return 0;
}
