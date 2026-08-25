#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <cstdint>

#include <TFile.h>
#include <TTree.h>

struct ChannelKey{
    Long64_t evt;
    int board;
    int ch;

    bool operator==(const ChannelKey& other) const{
        return evt == other.evt && board == other.board && ch == other.ch;
    }
};

struct ChannelKeyHash{
    std::size_t operator()(const ChannelKey& key) const{
        std::size_t h1 = std::hash<Long64_t>{}(key.evt);
        std::size_t h2 = std::hash<int>{}(key.board);
        std::size_t h3 = std::hash<int>{}(key.ch);
        return h1 ^ (h2 << 1) ^ (h3 << 2); // Combine the hash values
    }
};

int main(int argc, char* argv[]){
    if(argc != 5){
        std::cerr << "Usage: " << argv[0] << " <feature.root> <hit.root> <output.root> <target_channel>" << std::endl;
        return 1;
    }

    const std::string feature_filename = argv[1];
    const std::string hit_filename = argv[2];
    const std::string output_filename = argv[3];
    const int target_ch = std::stoi(argv[4]);
    if(target_ch < 0 || target_ch > 15){
        std::cerr << "Error: target_channel must be between 0 and 15." << std::endl;
        return 1;
    }

    constexpr int max_events = 100;

    // open hit file
    TFile* hit_file = TFile::Open(hit_filename.c_str(), "READ");
    if(!hit_file || hit_file->IsZombie()){
        std::cerr << "Error opening hit file: " << hit_file << std::endl;
        return 1;
    }

    TTree* hit_tree = dynamic_cast<TTree*>(hit_file->Get("tree"));
    if(!hit_tree){
        std::cerr << "Error: 'tree' not found in hit file: " << hit_file << std::endl;
        hit_file->Close();
        return 1;
    }

    Long64_t hit_evt;
    int hit_board;
    int hit_ch;

    hit_tree->SetBranchAddress("evt", &hit_evt);
    hit_tree->SetBranchAddress("board", &hit_board);
    hit_tree->SetBranchAddress("ch", &hit_ch);

    // store selected channels
    std::unordered_set<ChannelKey, ChannelKeyHash> hit_channels;

    std::unordered_set<Long64_t> selected_events;

    for(Long64_t entry=0; entry<hit_tree->GetEntries(); ++entry){
        hit_tree->GetEntry(entry);

        hit_channels.insert({hit_evt, hit_board, hit_ch});

        if(hit_board == 1 && hit_ch == target_ch){
            selected_events.insert(hit_evt);
        }
    }

    std::cout << "Events with Front DUT hit: " << selected_events.size() << std::endl;

    // open feature file
    TFile* feature_file = TFile::Open(feature_filename.c_str(), "READ");
    if(!feature_file || feature_file->IsZombie()){
        std::cerr << "Error opening feature file: " << feature_file << std::endl;
        hit_file->Close();
        return 1;
    }

    TTree* feature_tree = dynamic_cast<TTree*>(feature_file->Get("tree"));
    if(!feature_tree){
        std::cerr << "Error: 'tree' not found in feature file: " << feature_file << std::endl;
        feature_file->Close();
        hit_file->Close();
        return 1;
    }

    Long64_t evt;
    int board;
    int ch;

    feature_tree->SetBranchAddress("evt", &evt);
    feature_tree->SetBranchAddress("board", &board);
    feature_tree->SetBranchAddress("ch", &ch);

    // output
    TFile* output_file = TFile::Open(output_filename.c_str(), "RECREATE");
    if(!output_file || output_file->IsZombie()){
        std::cerr << "Error creating output file: " << output_file << std::endl;
        feature_file->Close();
        hit_file->Close();
        return 1;
    }
    output_file->cd();
    TTree* output_tree = feature_tree->CloneTree(0); // Clone the structure, but not the entries

    output_tree->SetName("tree");
    output_tree->SetTitle("full events containing selected hit");

    bool is_hit = false;

    output_tree->Branch("is_hit", &is_hit, "is_hit/O");

    // feature tree loop
    Long64_t previous_evt = -1;
    int saved_events = 0;
    bool save_this_event = false;
    const Long64_t nentries = feature_tree->GetEntries();

    for(Long64_t entry=0; entry<nentries; ++entry){
        feature_tree->GetEntry(entry);
        
        // new event
        if(evt != previous_evt){

            previous_evt = evt;

            save_this_event = selected_events.contains(evt);

            if(save_this_event){
                if(saved_events >= max_events) break;

                ++saved_events;

                std::cout << "save event: " << evt << " ( " << saved_events << " / " << max_events << ")" << std::endl;
            }
        }
        if(!save_this_event) continue;

        // did this channe pass hit selection?
        ChannelKey key{evt, board, ch};
        is_hit = hit_channels.contains(key);
        output_tree->Fill();
    }

    output_file->cd();
    output_tree->Write();
    output_file->Close();
    feature_file->Close();
    hit_file->Close();


    return 0;
}