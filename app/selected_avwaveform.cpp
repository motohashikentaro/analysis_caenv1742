#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

int main(int argc, char* argv[]){

    TFile* file = TFile::Open(argv[1]);
    TTree* tree = (TTree*)file->Get("tree");

    double evt;
    int board;
    int ch;
    double thres;
    double pedestal;
}