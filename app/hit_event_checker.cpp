#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <iomanip>
#include <algorithm>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraph.h>

// ============================================================
// Pixel map
// ============================================================

struct PixelPosition {
    int x;
    int y;
};

constexpr PixelPosition kChannelMap[2][16]{

    {  // board 1, lgad 0

        {1, 0}, // pixel 0   digi ch 0
        {0, 0}, // pixel 1   digi ch 1
        {1, 1}, // pixel 2   digi ch 2
        {0, 1}, // pixel 3   digi ch 3
        {1, 2}, // pixel 4   digi ch 4
        {0, 2}, // pixel 5   digi ch 5
        {1, 3}, // pixel 6   digi ch 6
        {0, 3}, // pixel 7   digi ch 7
        {3, 3}, // pixel 8   digi ch 8
        {2, 3}, // pixel 9   digi ch 9
        {3, 2}, // pixel 10  digi ch 10
        {2, 2}, // pixel 11  digi ch 11
        {3, 1}, // pixel 12  digi ch 12
        {2, 1}, // pixel 13  digi ch 13
        {3, 0}, // pixel 14  digi ch 14
        {2, 0}  // pixel 15  digi ch 15

    },

    {  // board 1, lgad 1

        {1, 0}, // pixel 0   digi ch 16
        {0, 0}, // pixel 1   digi ch 17
        {1, 1}, // pixel 2   digi ch 18
        {0, 1}, // pixel 3   digi ch 19
        {1, 2}, // pixel 4   digi ch 20
        {0, 2}, // pixel 5   digi ch 21
        {1, 3}, // pixel 6   digi ch 22
        {0, 3}, // pixel 7   digi ch 23
        {3, 3}, // pixel 8   digi ch 24
        {2, 3}, // pixel 9   digi ch 25
        {3, 2}, // pixel 10  digi ch 26
        {2, 2}, // pixel 11  digi ch 27
        {3, 1}, // pixel 12  digi ch 28
        {2, 1}, // pixel 13  digi ch 29
        {3, 0}, // pixel 14  digi ch 30
        {2, 0}  // pixel 15  digi ch 31

    }

};

// ============================================================
// Channel data in one event
// ============================================================

struct ChannelData {

    bool found = false;
    bool is_hit = false;

    double peak_adc = 0.0;
    double charge_th30 = 0.0;
    double tot_th30 = 0.0;
};

// ============================================================
// Find physically adjacent pixels
//
// 4-neighbor:
//           up
//     left center right
//          down
// ============================================================

std::vector<int> GetAdjacentChannels(int center_ch)
{
    const int lgad = center_ch / 16;
    const int local_ch = center_ch % 16;

    if(lgad < 0 || lgad >= 2){
        return {};
    }

    const PixelPosition center =
        kChannelMap[lgad][local_ch];

    std::vector<int> adjacent_channels;

    for(int local = 0; local < 16; ++local){

        if(local == local_ch){
            continue;
        }

        const PixelPosition pos =
            kChannelMap[lgad][local];

        const int dx =
            std::abs(pos.x - center.x);

        const int dy =
            std::abs(pos.y - center.y);

        // physical 4-neighbor
        if(dx + dy == 1){

            const int global_ch =
                lgad * 16 + local;

            adjacent_channels.push_back(
                global_ch
            );
        }
    }

    return adjacent_channels;
}

// ============================================================
// Binomial error
// ============================================================

double BinomialError(
    Long64_t success,
    Long64_t total
)
{
    if(total <= 0){
        return 0.0;
    }

    const double p =
        static_cast<double>(success)
        / static_cast<double>(total);

    return std::sqrt(
        p * (1.0 - p)
        / static_cast<double>(total)
    );
}

// ============================================================
// Main
// ============================================================

int main(int argc, char* argv[])
{
    if(argc < 2){

        std::cerr
            << "Usage: "
            << argv[0]
            << " <input.root> [center_ch]"
            << std::endl;

        return 1;
    }

    const std::string input_filename =
        argv[1];

    const int center_ch =
        (argc >= 3)
        ? std::stoi(argv[2])
        : 13;

    constexpr int target_board = 1;

    // --------------------------------------------------------
    // Check channel
    // --------------------------------------------------------

    if(center_ch < 0 || center_ch >= 32){

        std::cerr
            << "Invalid center channel: "
            << center_ch
            << std::endl;

        return 1;
    }

    const int center_lgad =
        center_ch / 16;

    const int center_local_ch =
        center_ch % 16;

    const PixelPosition center_position =
        kChannelMap[center_lgad][center_local_ch];

    const std::vector<int> adjacent_channels =
        GetAdjacentChannels(center_ch);

    // --------------------------------------------------------
    // Print geometry
    // --------------------------------------------------------

    std::cout
        << "========================================\n"
        << " Pixel adjacent-hit checker\n"
        << "========================================\n";

    std::cout
        << "Board      : "
        << target_board
        << "\n";

    std::cout
        << "LGAD       : "
        << center_lgad
        << "\n";

    std::cout
        << "Center ch  : "
        << center_ch
        << "\n";

    std::cout
        << "Center pos : ("
        << center_position.x
        << ", "
        << center_position.y
        << ")\n";

    std::cout
        << "Adjacent channels:\n";

    for(int adj_ch : adjacent_channels){

        const int lgad =
            adj_ch / 16;

        const int local_ch =
            adj_ch % 16;

        const PixelPosition pos =
            kChannelMap[lgad][local_ch];

        std::cout
            << "  ch "
            << std::setw(2)
            << adj_ch
            << " -> ("
            << pos.x
            << ", "
            << pos.y
            << ")\n";
    }

    std::cout << std::endl;

    // ========================================================
    // Open input ROOT file
    // ========================================================

    TFile* fin =
        TFile::Open(
            input_filename.c_str(),
            "READ"
        );

    if(!fin || fin->IsZombie()){

        std::cerr
            << "Failed to open "
            << input_filename
            << std::endl;

        return 1;
    }

    TTree* tree =
        dynamic_cast<TTree*>(
            fin->Get("tree")
        );

    if(!tree){

        std::cerr
            << "TTree 'tree' was not found."
            << std::endl;

        fin->Close();

        return 1;
    }

    // ========================================================
    // Input branches
    // ========================================================

    Long64_t evt = 0;

    int board = 0;
    int ch = 0;

    double peak_adc = 0.0;
    double charge_th30 = 0.0;
    double tot_th30 = 0.0;

    bool is_hit = false;

    tree->SetBranchAddress(
        "evt",
        &evt
    );

    tree->SetBranchAddress(
        "board",
        &board
    );

    tree->SetBranchAddress(
        "ch",
        &ch
    );

    tree->SetBranchAddress(
        "peak_adc",
        &peak_adc
    );

    tree->SetBranchAddress(
        "charge_th30",
        &charge_th30
    );

    tree->SetBranchAddress(
        "tot_th30",
        &tot_th30
    );

    tree->SetBranchAddress(
        "is_hit",
        &is_hit
    );

    // ========================================================
    // Output ROOT file
    // ========================================================

    const std::string output_filename =
        "hit_event_check_ch"
        + std::to_string(center_ch)
        + ".root";

    TFile* fout =
        new TFile(
            output_filename.c_str(),
            "RECREATE"
        );

    // ========================================================
    // Histograms
    // ========================================================

    // --------------------------------------------------------
    // Number of adjacent hit pixels
    // --------------------------------------------------------

    TH1D* h_n_adjacent_hits_center_hit =
        new TH1D(
            "n_adjacent_hits_center_hit",
            "Center hit;"
            "Number of adjacent hit pixels;"
            "Events",
            5,
            -0.5,
            4.5
        );

    TH1D* h_n_adjacent_hits_center_not_hit =
        new TH1D(
            "n_adjacent_hits_center_not_hit",
            "Center not hit;"
            "Number of adjacent hit pixels;"
            "Events",
            5,
            -0.5,
            4.5
        );

    // --------------------------------------------------------
    // Which adjacent channel fired?
    // --------------------------------------------------------

    TH1D* h_adjacent_channel_center_hit =
        new TH1D(
            "adjacent_channel_center_hit",
            "Adjacent hits when center is hit;"
            "Channel;"
            "Events",
            32,
            -0.5,
            31.5
        );

    TH1D* h_adjacent_channel_center_not_hit =
        new TH1D(
            "adjacent_channel_center_not_hit",
            "Adjacent hits when center is not hit;"
            "Channel;"
            "Events",
            32,
            -0.5,
            31.5
        );

    // --------------------------------------------------------
    // Physical pixel hitmap
    // --------------------------------------------------------

    TH2D* h_adjacent_hitmap =
        new TH2D(
            "adjacent_hitmap",
            "Adjacent hits when center is hit;"
            "Pixel X;"
            "Pixel Y",
            4,
            -0.5,
            3.5,
            4,
            -0.5,
            3.5
        );

    // ========================================================
    // Correlation graphs
    //
    // Graphs contain all events where:
    //   center is_hit == true
    //   adjacent channel exists
    //
    // Adjacent channel does NOT need to pass is_hit.
    //
    // This is useful for seeing charge sharing below the
    // current hit threshold as well.
    // ========================================================

    std::map<int, TGraph*> peak_corr;
    std::map<int, TGraph*> charge_corr;
    std::map<int, TGraph*> tot_corr;

    // Only events where both center and adjacent are is_hit
    std::map<int, TGraph*> peak_corr_both_hit;
    std::map<int, TGraph*> charge_corr_both_hit;

    for(int adj_ch : adjacent_channels){

        // peak ADC
        peak_corr[adj_ch] =
            new TGraph();

        peak_corr[adj_ch]->SetName(
            Form(
                "peak_corr_ch%d_ch%d",
                center_ch,
                adj_ch
            )
        );

        peak_corr[adj_ch]->SetTitle(
            Form(
                "Center ch%d vs adjacent ch%d;"
                "-Peak ADC ch%d;"
                "-Peak ADC ch%d",
                center_ch,
                adj_ch,
                center_ch,
                adj_ch
            )
        );

        // charge
        charge_corr[adj_ch] =
            new TGraph();

        charge_corr[adj_ch]->SetName(
            Form(
                "charge_corr_ch%d_ch%d",
                center_ch,
                adj_ch
            )
        );

        charge_corr[adj_ch]->SetTitle(
            Form(
                "Center ch%d vs adjacent ch%d;"
                "Charge th30 ch%d;"
                "Charge th30 ch%d",
                center_ch,
                adj_ch,
                center_ch,
                adj_ch
            )
        );

        // ToT
        tot_corr[adj_ch] =
            new TGraph();

        tot_corr[adj_ch]->SetName(
            Form(
                "tot_corr_ch%d_ch%d",
                center_ch,
                adj_ch
            )
        );

        tot_corr[adj_ch]->SetTitle(
            Form(
                "Center ch%d vs adjacent ch%d;"
                "ToT th30 ch%d;"
                "ToT th30 ch%d",
                center_ch,
                adj_ch,
                center_ch,
                adj_ch
            )
        );

        // ----------------------------------------------------
        // Both-hit correlations
        // ----------------------------------------------------

        peak_corr_both_hit[adj_ch] =
            new TGraph();

        peak_corr_both_hit[adj_ch]->SetName(
            Form(
                "peak_corr_both_hit_ch%d_ch%d",
                center_ch,
                adj_ch
            )
        );

        peak_corr_both_hit[adj_ch]->SetTitle(
            Form(
                "Both hit: ch%d vs ch%d;"
                "-Peak ADC ch%d;"
                "-Peak ADC ch%d",
                center_ch,
                adj_ch,
                center_ch,
                adj_ch
            )
        );

        charge_corr_both_hit[adj_ch] =
            new TGraph();

        charge_corr_both_hit[adj_ch]->SetName(
            Form(
                "charge_corr_both_hit_ch%d_ch%d",
                center_ch,
                adj_ch
            )
        );

        charge_corr_both_hit[adj_ch]->SetTitle(
            Form(
                "Both hit: ch%d vs ch%d;"
                "Charge th30 ch%d;"
                "Charge th30 ch%d",
                center_ch,
                adj_ch,
                center_ch,
                adj_ch
            )
        );
    }

    // ========================================================
    // Output event tree
    // ========================================================

    TTree* outtree =
        new TTree(
            "event_tree",
            "Event-by-event adjacent hit information"
        );

    Long64_t out_evt = 0;

    bool out_center_hit = false;

    int out_n_adjacent_hits = 0;

    unsigned int out_adjacent_hit_mask = 0;

    outtree->Branch(
        "evt",
        &out_evt
    );

    outtree->Branch(
        "center_hit",
        &out_center_hit
    );

    outtree->Branch(
        "n_adjacent_hits",
        &out_n_adjacent_hits
    );

    outtree->Branch(
        "adjacent_hit_mask",
        &out_adjacent_hit_mask
    );

    // ========================================================
    // Counters
    // ========================================================

    Long64_t n_center_found_events = 0;

    Long64_t n_center_hit_events = 0;
    Long64_t n_center_not_hit_events = 0;

    Long64_t n_center_hit_with_adjacent = 0;
    Long64_t n_center_not_hit_with_adjacent = 0;

    std::map<int, Long64_t>
        adjacent_hit_count_center_hit;

    std::map<int, Long64_t>
        adjacent_hit_count_center_not_hit;

    for(int adj_ch : adjacent_channels){

        adjacent_hit_count_center_hit[
            adj_ch
        ] = 0;

        adjacent_hit_count_center_not_hit[
            adj_ch
        ] = 0;
    }

    // ========================================================
    // Event processing function
    // ========================================================

    auto process_event =
        [&](Long64_t event_id,
            const std::map<int, ChannelData>& channels)
    {
        // ----------------------------------------------------
        // Center channel must exist
        // ----------------------------------------------------

        const auto center_it =
            channels.find(center_ch);

        if(center_it == channels.end()){
            return;
        }

        if(!center_it->second.found){
            return;
        }

        ++n_center_found_events;

        const ChannelData& center_data =
            center_it->second;

        // ----------------------------------------------------
        // Count adjacent hits
        // ----------------------------------------------------

        int n_adjacent_hits = 0;

        unsigned int adjacent_hit_mask = 0;

        for(size_t i = 0;
            i < adjacent_channels.size();
            ++i){

            const int adj_ch =
                adjacent_channels[i];

            const auto adj_it =
                channels.find(adj_ch);

            if(adj_it == channels.end()){
                continue;
            }

            const ChannelData& adj_data =
                adj_it->second;

            if(!adj_data.found){
                continue;
            }

            // ------------------------------------------------
            // Correlation
            //
            // Center hit is required.
            // Adjacent hit is not required.
            //
            // Therefore sub-threshold neighbor signals are also
            // visible.
            // ------------------------------------------------

            if(center_data.is_hit){

                {
                    TGraph* graph =
                        peak_corr[adj_ch];

                    const int point =
                        graph->GetN();

                    graph->SetPoint(
                        point,
                        -center_data.peak_adc,
                        -adj_data.peak_adc
                    );
                }

                {
                    TGraph* graph =
                        charge_corr[adj_ch];

                    const int point =
                        graph->GetN();

                    graph->SetPoint(
                        point,
                        center_data.charge_th30,
                        adj_data.charge_th30
                    );
                }

                {
                    TGraph* graph =
                        tot_corr[adj_ch];

                    const int point =
                        graph->GetN();

                    graph->SetPoint(
                        point,
                        center_data.tot_th30,
                        adj_data.tot_th30
                    );
                }
            }

            // ------------------------------------------------
            // Is adjacent pixel a hit?
            // ------------------------------------------------

            if(!adj_data.is_hit){
                continue;
            }

            ++n_adjacent_hits;

            adjacent_hit_mask |=
                (1u << i);

            // ------------------------------------------------
            // Center hit
            // ------------------------------------------------

            if(center_data.is_hit){

                ++adjacent_hit_count_center_hit[
                    adj_ch
                ];

                h_adjacent_channel_center_hit
                    ->Fill(adj_ch);

                const int adj_lgad =
                    adj_ch / 16;

                const int adj_local_ch =
                    adj_ch % 16;

                const PixelPosition pos =
                    kChannelMap[
                        adj_lgad
                    ][
                        adj_local_ch
                    ];

                h_adjacent_hitmap->Fill(
                    pos.x,
                    pos.y
                );

                // both hit correlations
                {
                    TGraph* graph =
                        peak_corr_both_hit[
                            adj_ch
                        ];

                    const int point =
                        graph->GetN();

                    graph->SetPoint(
                        point,
                        -center_data.peak_adc,
                        -adj_data.peak_adc
                    );
                }

                {
                    TGraph* graph =
                        charge_corr_both_hit[
                            adj_ch
                        ];

                    const int point =
                        graph->GetN();

                    graph->SetPoint(
                        point,
                        center_data.charge_th30,
                        adj_data.charge_th30
                    );
                }
            }

            // ------------------------------------------------
            // Center not hit
            // ------------------------------------------------

            else{

                ++adjacent_hit_count_center_not_hit[
                    adj_ch
                ];

                h_adjacent_channel_center_not_hit
                    ->Fill(adj_ch);
            }
        }

        // ----------------------------------------------------
        // Center hit / not hit statistics
        // ----------------------------------------------------

        if(center_data.is_hit){

            ++n_center_hit_events;

            h_n_adjacent_hits_center_hit
                ->Fill(n_adjacent_hits);

            if(n_adjacent_hits > 0){

                ++n_center_hit_with_adjacent;
            }
        }
        else{

            ++n_center_not_hit_events;

            h_n_adjacent_hits_center_not_hit
                ->Fill(n_adjacent_hits);

            if(n_adjacent_hits > 0){

                ++n_center_not_hit_with_adjacent;
            }
        }

        // ----------------------------------------------------
        // Save event information
        // ----------------------------------------------------

        out_evt =
            event_id;

        out_center_hit =
            center_data.is_hit;

        out_n_adjacent_hits =
            n_adjacent_hits;

        out_adjacent_hit_mask =
            adjacent_hit_mask;

        outtree->Fill();
    };

    // ========================================================
    // Event loop
    // ========================================================

    std::map<int, ChannelData>
        event_channels;

    Long64_t previous_evt = -1;

    bool first_target_entry = true;

    const Long64_t nentries =
        tree->GetEntries();

    for(Long64_t entry = 0;
        entry < nentries;
        ++entry){

        tree->GetEntry(entry);

        // ----------------------------------------------------
        // Only DUT board
        // ----------------------------------------------------

        if(board != target_board){
            continue;
        }

        // ----------------------------------------------------
        // Same LGAD as center
        // ----------------------------------------------------

        if(ch / 16 != center_lgad){
            continue;
        }

        // ----------------------------------------------------
        // First entry
        // ----------------------------------------------------

        if(first_target_entry){

            previous_evt = evt;

            first_target_entry = false;
        }

        // ----------------------------------------------------
        // Event changed
        // ----------------------------------------------------

        if(evt != previous_evt){

            process_event(
                previous_evt,
                event_channels
            );

            event_channels.clear();

            previous_evt = evt;
        }

        // ----------------------------------------------------
        // Store channel data
        // ----------------------------------------------------

        ChannelData& data =
            event_channels[ch];

        data.found = true;
        data.is_hit = is_hit;

        data.peak_adc =
            peak_adc;

        data.charge_th30 =
            charge_th30;

        data.tot_th30 =
            tot_th30;
    }

    // --------------------------------------------------------
    // Last event
    // --------------------------------------------------------

    if(!first_target_entry){

        process_event(
            previous_evt,
            event_channels
        );
    }

    // ========================================================
    // Print result
    // ========================================================

    std::cout
        << "\n"
        << "========================================\n"
        << " Result\n"
        << "========================================\n";

    std::cout
        << "Center found events     : "
        << n_center_found_events
        << "\n";

    std::cout
        << "Center hit events       : "
        << n_center_hit_events
        << "\n";

    std::cout
        << "Center not-hit events   : "
        << n_center_not_hit_events
        << "\n";

    // --------------------------------------------------------
    // Center hit
    // --------------------------------------------------------

    if(n_center_hit_events > 0){

        const double fraction =
            static_cast<double>(
                n_center_hit_with_adjacent
            )
            / static_cast<double>(
                n_center_hit_events
            );

        const double error =
            BinomialError(
                n_center_hit_with_adjacent,
                n_center_hit_events
            );

        std::cout
            << "\n"
            << "[Center hit]\n";

        std::cout
            << "With >=1 adjacent hit : "
            << n_center_hit_with_adjacent
            << " / "
            << n_center_hit_events
            << "\n";

        std::cout
            << std::fixed
            << std::setprecision(2)
            << "Fraction              : "
            << 100.0 * fraction
            << " +- "
            << 100.0 * error
            << " %\n";

        std::cout
            << "\n"
            << "Adjacent channel breakdown:\n";

        for(int adj_ch : adjacent_channels){

            const Long64_t count =
                adjacent_hit_count_center_hit[
                    adj_ch
                ];

            const double ch_fraction =
                static_cast<double>(count)
                / static_cast<double>(
                    n_center_hit_events
                );

            const double ch_error =
                BinomialError(
                    count,
                    n_center_hit_events
                );

            const int lgad =
                adj_ch / 16;

            const int local_ch =
                adj_ch % 16;

            const PixelPosition pos =
                kChannelMap[
                    lgad
                ][
                    local_ch
                ];

            std::cout
                << "  ch "
                << std::setw(2)
                << adj_ch
                << "  ("
                << pos.x
                << ", "
                << pos.y
                << ") : "
                << std::setw(5)
                << count
                << " / "
                << n_center_hit_events
                << " = "
                << std::setw(6)
                << 100.0 * ch_fraction
                << " +- "
                << std::setw(6)
                << 100.0 * ch_error
                << " %\n";
        }
    }

    // --------------------------------------------------------
    // Center not hit
    // --------------------------------------------------------

    if(n_center_not_hit_events > 0){

        const double fraction =
            static_cast<double>(
                n_center_not_hit_with_adjacent
            )
            / static_cast<double>(
                n_center_not_hit_events
            );

        const double error =
            BinomialError(
                n_center_not_hit_with_adjacent,
                n_center_not_hit_events
            );

        std::cout
            << "\n"
            << "[Center not hit]\n";

        std::cout
            << "With >=1 adjacent hit : "
            << n_center_not_hit_with_adjacent
            << " / "
            << n_center_not_hit_events
            << "\n";

        std::cout
            << "Fraction              : "
            << 100.0 * fraction
            << " +- "
            << 100.0 * error
            << " %\n";

        std::cout
            << "\n"
            << "Adjacent channel breakdown:\n";

        for(int adj_ch : adjacent_channels){

            const Long64_t count =
                adjacent_hit_count_center_not_hit[
                    adj_ch
                ];

            const double ch_fraction =
                static_cast<double>(count)
                / static_cast<double>(
                    n_center_not_hit_events
                );

            const double ch_error =
                BinomialError(
                    count,
                    n_center_not_hit_events
                );

            const int lgad =
                adj_ch / 16;

            const int local_ch =
                adj_ch % 16;

            const PixelPosition pos =
                kChannelMap[
                    lgad
                ][
                    local_ch
                ];

            std::cout
                << "  ch "
                << std::setw(2)
                << adj_ch
                << "  ("
                << pos.x
                << ", "
                << pos.y
                << ") : "
                << std::setw(5)
                << count
                << " / "
                << n_center_not_hit_events
                << " = "
                << std::setw(6)
                << 100.0 * ch_fraction
                << " +- "
                << std::setw(6)
                << 100.0 * ch_error
                << " %\n";
        }
    }
    else{

        std::cout
            << "\n"
            << "[Center not hit]\n"
            << "No center-not-hit events are contained "
            << "in this ROOT file.\n"
            << "Baseline comparison cannot be performed.\n";
    }

    // ========================================================
    // Write
    // ========================================================

    fout->cd();

    h_n_adjacent_hits_center_hit->Write();
    h_n_adjacent_hits_center_not_hit->Write();

    h_adjacent_channel_center_hit->Write();
    h_adjacent_channel_center_not_hit->Write();

    h_adjacent_hitmap->Write();

    outtree->Write();

    for(int adj_ch : adjacent_channels){

        peak_corr[
            adj_ch
        ]->Write();

        charge_corr[
            adj_ch
        ]->Write();

        tot_corr[
            adj_ch
        ]->Write();

        peak_corr_both_hit[
            adj_ch
        ]->Write();

        charge_corr_both_hit[
            adj_ch
        ]->Write();
    }

    fout->Close();

    fin->Close();

    std::cout
        << "\n"
        << "Output ROOT file: "
        << output_filename
        << std::endl;

    return 0;
}