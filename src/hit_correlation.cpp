// #include "./../include/hit_correlation.h"

// #include <TFile.h>

// #include <iostream>
// #include <algorithm>


// HitCorrelation::HitCorrelation(TTree* tree)
//     :
//     tree_(tree)
// {

//     tree_->Branch("energy",&energy_);
//     tree_->Branch("nw",&nw_);

//     tree_->Branch("event",&hit_.event);

//     tree_->Branch("nfront",&hit_.nfront);
//     tree_->Branch("front_charge",&hit_.front_charge);
//     tree_->Branch("front_max_adc",&hit_.front_max_adc);


//     tree_->Branch("nback",&hit_.nback);
//     tree_->Branch("back_charge",&hit_.back_charge);
//     tree_->Branch("back_max_adc",&hit_.back_max_adc);
// }



// void HitCorrelation::AddFile(
//     const std::string& filename,
//     int energy,
//     int nw
// )
// {

//     energy_ = energy;
//     nw_ = nw;


//     TFile* file = TFile::Open(
//         filename.c_str(),
//         "READ"
//     );


//     if(!file || file->IsZombie()){

//         std::cerr
//             << "Cannot open "
//             << filename
//             << std::endl;

//         return;
//     }


//     TTree* tree =
//         (TTree*)file->Get("tree_th30");


//     Long64_t evt;
//     int board;
//     int ch;

//     double peak_adc;
//     double charge;


//     tree->SetBranchAddress("evt",&evt);
//     tree->SetBranchAddress("board",&board);
//     tree->SetBranchAddress("ch",&ch);

//     tree->SetBranchAddress("peak_adc",&peak_adc);
//     tree->SetBranchAddress("charge",&charge);



//     Long64_t current_evt = -1;


//     Long64_t nentries =
//         tree->GetEntries();


//     for(Long64_t i=0; i<nentries; i++){

//         tree->GetEntry(i);


//         // board 1 only
//         if(board != 1)
//             continue;



//         // event change
//         if(evt != current_evt){

//             // 前のイベントを保存
//             if(current_evt != -1){

//                 tree_->Fill();

//             }

//             // 次のイベントに向けて初期化
//             hit_ = EventHit();

//             hit_.event = evt;


//             current_evt = evt;
//         }



//         // front LGAD
//         if(ch < 16){

//             hit_.nfront++;

//             hit_.front_charge += charge;


//             hit_.front_max_adc =
//                 std::min(
//                     hit_.front_max_adc,
//                     peak_adc
//                 );
//         }


//         // back LGAD
//         else if(ch < 32){

//             hit_.nback++;

//             hit_.back_charge += charge;


//             hit_.back_max_adc =
//                 std::min(
//                     hit_.back_max_adc,
//                     peak_adc
//                 );
//         }

//     }


//     // last event
//     if(current_evt != -1){

//         tree_->Fill();

//     }


//     file->Close();

//     delete file;

// }

#include "./../include/hit_correlation.h"

#include <TFile.h>

#include <iostream>
#include <algorithm>


HitCorrelation::HitCorrelation(TTree* tree)
    :
    tree_(tree)
{

    tree_->Branch("energy",&energy_);
    tree_->Branch("nw",&nw_);

    tree_->Branch("event",&hit_.event);


    // front event level
    tree_->Branch("nfront",&hit_.nfront);
    tree_->Branch("front_charge",&hit_.front_charge);
    tree_->Branch("front_max_adc",&hit_.front_max_adc);


    // back event level
    tree_->Branch("nback",&hit_.nback);
    tree_->Branch("back_charge",&hit_.back_charge);
    tree_->Branch("back_max_adc",&hit_.back_max_adc);


    // hit level
    tree_->Branch(
        "front_ch",
        &hit_.front_ch
    );

    tree_->Branch(
        "front_peak_adc",
        &hit_.front_peak_adc
    );

    tree_->Branch(
        "front_charge_each",
        &hit_.front_charge_each
    );

    tree_->Branch(
        "front_peak_time",
        &hit_.front_peak_time
    );

    tree_->Branch(
        "front_tot",
        &hit_.front_tot
    );



    tree_->Branch(
        "back_ch",
        &hit_.back_ch
    );

    tree_->Branch(
        "back_peak_adc",
        &hit_.back_peak_adc
    );

    tree_->Branch(
        "back_charge_each",
        &hit_.back_charge_each
    );

    tree_->Branch(
        "back_peak_time",
        &hit_.back_peak_time
    );

    tree_->Branch(
        "back_tot",
        &hit_.back_tot
    );
}



void HitCorrelation::AddFile(
    const std::string& filename,
    int energy,
    int nw
)
{

    energy_ = energy;
    nw_ = nw;


    TFile* file = TFile::Open(
        filename.c_str(),
        "READ"
    );


    if(!file || file->IsZombie()){

        std::cerr
            << "Cannot open "
            << filename
            << std::endl;

        return;
    }


    TTree* tree =
        (TTree*)file->Get("tree_th30");


    double evt;

    int board;
    int ch;


    double peak_adc;
    double charge;
    int peak_time;
    double tot;



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
        "charge",
        &charge
    );

    tree->SetBranchAddress(
        "peak_time",
        &peak_time
    );

    tree->SetBranchAddress(
        "tot",
        &tot
    );



    Long64_t current_evt = -1;


    Long64_t nentries =
        tree->GetEntries();



    for(Long64_t i=0; i<nentries; i++){

        tree->GetEntry(i);


        // board 1 only
        if(board != 1)
            continue;



        // event change
        if(evt != current_evt){


            // save previous event
            if(current_evt != -1){

                tree_->Fill();

            }


            // initialize new event
            hit_ = EventHit();

            hit_.event = evt;


            current_evt = evt;
        }




        // front LGAD ch0-15
        if(ch < 16){

            hit_.nfront++;

            hit_.front_charge += charge;


            hit_.front_max_adc =
                std::min(
                    hit_.front_max_adc,
                    peak_adc
                );


            hit_.front_ch.push_back(ch);

            hit_.front_peak_adc.push_back(peak_adc);

            hit_.front_charge_each.push_back(charge);

            hit_.front_peak_time.push_back(peak_time);

            hit_.front_tot.push_back(tot);

        }


        // back LGAD ch16-31
        else if(ch < 32){

            hit_.nback++;

            hit_.back_charge += charge;


            hit_.back_max_adc =
                std::min(
                    hit_.back_max_adc,
                    peak_adc
                );


            hit_.back_ch.push_back(ch);

            hit_.back_peak_adc.push_back(peak_adc);

            hit_.back_charge_each.push_back(charge);

            hit_.back_peak_time.push_back(peak_time);

            hit_.back_tot.push_back(tot);

        }

    }



    // save last event
    if(current_evt != -1){

        tree_->Fill();

    }


    file->Close();

    delete file;

}