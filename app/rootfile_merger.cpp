#include <iostream>
#include <string>
#include <cstdio>

#include <TFile.h>
#include <TTree.h>

int main(int argc, char* argv[]){
    if(argc < 4){
        std::cerr << "Usage: " << argv[0] << " <output_root_file> <input_root_file1> <input_root_file2> ..." << std::endl;
        return 1;
    }

    constexpr int nboard = 2;
    constexpr int nch = 32;
    constexpr int nsamples = 1024;

    const std::string output_filename = argv[1];

    // +----------------+
    // |output variables|
    // +----------------+
    Long64_t ev_id_out = 0;
    float amp[nboard][nch][nsamples]{};

    // +------------------+
    // |create output file|
    // +------------------+
    TFile* fout = TFile::Open(output_filename.c_str(), "RECREATE");
    TTree* tree_out = new TTree("tree", "merged events waveforms");

    tree_out->Branch("ev_id", &ev_id_out, "ev_id/L");
    for(int board=0; board<nboard; ++board){
        for(int ch=0; ch<nch; ++ch){
            char branch_name[32];
            char leaf_list[64];

            std::snprintf(branch_name, sizeof(branch_name), "amp_b%d_ch%02d", board, ch);
            std::snprintf(leaf_list, sizeof(leaf_list), "%s[%d]/F", branch_name, nsamples);
            tree_out->Branch(branch_name, amp[board][ch], leaf_list);
        }
    }

    // +--------------------+
    // |global event counter|
    // +--------------------+
    Long64_t global_ev_id = 0;

    // +---------------+
    // |input file loop|
    // +---------------+
    for(int file_idx=2; file_idx<argc; ++file_idx){
        const std::string input_filename = argv[file_idx];

        std::cout << "Reading input file: " << input_filename << std::endl;

        TFile* fin = TFile::Open(input_filename.c_str(), "READ");
        if(!fin || fin->IsZombie()){
            std::cerr << "Error opening input file: " << input_filename << std::endl;
            return 1;
        }

        TTree* tree_in = (TTree*)fin->Get("tree");
        if(!tree_in){
            fin->Close();
            delete fin;
            std::cerr << "Error: TTree 'tree' not found in input file: " << input_filename << std::endl;
            return 1;
        }

        // +---------------+
        // |input variables|
        // +---------------+
        Long64_t ev_id_in = 0;
        tree_in->SetBranchAddress("ev_id", &ev_id_in);
        for(int board=0; board<nboard; ++board){
            for(int ch=0; ch<nch; ++ch){
                char branch_name[32];

                std::snprintf(branch_name, sizeof(branch_name), "amp_b%d_ch%02d", board, ch);
                tree_in->SetBranchAddress(branch_name, amp[board][ch]);
            }
        }

        const Long64_t nentries = tree_in->GetEntries();

        // +----------------+
        // |event loop      |
        // +----------------+
        Long64_t previous_ev_id = -1;
        for(Long64_t entry=0; entry<nentries; ++entry){
            tree_in->GetEntry(entry);

            if(entry == 0 && ev_id_in != 0){
                std::cerr << "Warning: First event ID in file " << input_filename << " is not zero. Adjusting global event ID accordingly." << std::endl;
            }
            if(entry > 0 && ev_id_in != previous_ev_id + 1){
                std::cerr << "Warning: Non-consecutive event IDs in file " << input_filename << ". Previous: " << previous_ev_id << ", Current: " << ev_id_in << std::endl;
            }

            previous_ev_id = ev_id_in;

            ev_id_out = global_ev_id;

            tree_out->Fill();

            ++global_ev_id;
        }
        std::cout << "Entries: " << nentries << std::endl;

        fin->Close();
        delete fin;
    }

    // +-------+
    // | write |
    // +-------+
    fout->cd();
    tree_out->Write();
    fout->Close();
    delete fout;

    return 0;
}