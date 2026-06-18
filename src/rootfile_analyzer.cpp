#include <iostream>
#include <filesystem>
#include <algorithm>

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TApplication.h>

#include "./../include/rootfile_analyzer.h"

RootfileAnalyzer::RootfileAnalyzer(char* input_path){

    rd_.file_ = TFile::Open(input_path);

    rd_.tree_ = (TTree*)rd_.file_->Get("tree");

    rd_.tree_->SetBranchAddress("ev_id", &rd_.ev_.ev_id);
    for(int board=0; board<rd_.nboard; board++){
        for(int ch=0; ch<rd_.nch; ch++){
            rd_.tree_->SetBranchAddress(Form("amp_b%d_ch%02d", board, ch), rd_.ev_.amp[board][ch]);
        }
    }

    rd_.nentries_ = rd_.tree_->GetEntries();
}

RootfileAnalyzer::~RootfileAnalyzer(){
    if(rd_.file_){
        rd_.file_->Close();
        delete rd_.file_;
    }
}