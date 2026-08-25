#include "./../include/feature_extractor.h"

#include <iostream>
#include <string>
#include <array>

#include <TFile.h>
#include <TTree.h>
#include <TNamed.h>

int main(int argc, char* argv[]){
    if(argc < 4){
        std::cerr << "Usage: " << argv[0] << " <output_root_file> <input_root_file1> <input_root_file2> ..." << std::endl;
        return 1;
    }

    const std::string output_filename = argv[1];

    // +----------------+
    // |output variables|
    // +----------------+
    Long64_t evt_out = 0;

    int board;
    int ch;

    double pedestal;
    double peak_adc;
    int peak_time;

    std::array<double, ThresholdData::nthres> raise_time;
    std::array<double, ThresholdData::nthres> fall_time;
    std::array<double, ThresholdData::nthres> charge;
    std::array<double, ThresholdData::nthres> tot;

    // +------------------+
    // |create output file|
    // +------------------+
    TFile* fout = TFile::Open(output_filename.c_str(), "RECREATE");
    TTree* tree_out = new TTree("tree", "merged events features");

    tree_out->Branch("evt", &evt_out);
    tree_out->Branch("board", &board);
    tree_out->Branch("ch", &ch);
    tree_out->Branch("pedestal", &pedestal);
    tree_out->Branch("peak_adc", &peak_adc);
    tree_out->Branch("peak_time", &peak_time);
    
    for(size_t i = 0; i < ThresholdData::nthres; ++i){
        tree_out->Branch(("raise_time_th" + std::to_string(ThresholdData::thresholds[i])).c_str(), &raise_time[i]);
        tree_out->Branch(("fall_time_th" + std::to_string(ThresholdData::thresholds[i])).c_str(), &fall_time[i]);
        tree_out->Branch(("charge_th" + std::to_string(ThresholdData::thresholds[i])).c_str(), &charge[i]);
        tree_out->Branch(("tot_th" + std::to_string(ThresholdData::thresholds[i])).c_str(), &tot[i]);
    }

    // +--------------------+
    // |global event counter|
    // +--------------------+
    Long64_t global_evt = 0;

    std::string source_files;

    // +---------------+
    // |input file loop|
    // +---------------+
    for(int file_idx = 2; file_idx < argc; ++file_idx){
        const std::string input_filename = argv[file_idx];

        std::cout << "Reading input file: " << input_filename << std::endl;

        if(!source_files.empty()){
            source_files += ",";
        }
        source_files += input_filename;

        TFile* fin = TFile::Open(input_filename.c_str(), "READ");

        TTree* tree_in = dynamic_cast<TTree*>(fin->Get("tree"));

        // +---------------+
        // |input variables|
        // +---------------+
        Long64_t evt_in = 0;

        tree_in->SetBranchAddress("evt", &evt_in);
        tree_in->SetBranchAddress("board", &board);
        tree_in->SetBranchAddress("ch", &ch);
        tree_in->SetBranchAddress("pedestal", &pedestal);
        tree_in->SetBranchAddress("peak_adc", &peak_adc);
        tree_in->SetBranchAddress("peak_time", &peak_time);
        for(size_t i = 0; i < ThresholdData::nthres; ++i){
            tree_in->SetBranchAddress(("raise_time_th" + std::to_string(ThresholdData::thresholds[i])).c_str(), &raise_time[i]);
            tree_in->SetBranchAddress(("fall_time_th" + std::to_string(ThresholdData::thresholds[i])).c_str(), &fall_time[i]);
            tree_in->SetBranchAddress(("charge_th" + std::to_string(ThresholdData::thresholds[i])).c_str(), &charge[i]);
            tree_in->SetBranchAddress(("tot_th" + std::to_string(ThresholdData::thresholds[i])).c_str(), &tot[i]);
        }

        const Long64_t nentries = tree_in->GetEntries();

        Long64_t previous_evt = -1;
        bool first_event = true;

        // +----------+
        // |entry loop|
        // +----------+
        for(Long64_t entry = 0; entry < nentries; ++entry){
            tree_in->GetEntry(entry);

            if(first_event || evt_in != previous_evt){
                if(!first_event){
                    ++global_evt;
                }
                first_event = false;
                previous_evt = evt_in;
            }

            evt_out = global_evt;
            tree_out->Fill();
        }

        if(!first_event){
            ++global_evt;
        }

        std::cout << "Entries: " << nentries << std::endl;

        fin->Close();
        delete fin;
    }

    // +--------------------------+
    // |save and write output file|
    // +--------------------------+
    fout->cd();
    tree_out->Write();

    std::cout << "Output file: " << output_filename << std::endl;
    std::cout << "Source files: " << source_files << std::endl;
    std::cout << "Total events: " << global_evt << std::endl;
    std::cout << "Total entries: " << tree_out->GetEntries() << std::endl;

    fout->Close();
    delete fout;

    return 0;
}