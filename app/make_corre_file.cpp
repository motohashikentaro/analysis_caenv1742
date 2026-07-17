#include "./../include/hit_correlation.h"

#include <TFile.h>


int main()
{

    TFile fout(
        "./../data/correlation/correlation.root",
        "RECREATE"
    );


    TTree tree(
        "tree",
        "event correlation"
    );


    HitCorrelation hc(&tree);


    hc.AddFile(
        "./../data/hit/hit_05088.root",
        2,
        2
    );


    hc.AddFile(
        "./../data/hit/hit_05141.root",
        3,
        2
    );

    hc.AddFile(
        "./../data/hit/hit_05158.root",
        4,
        2
    );
    
    hc.AddFile(
        "./../data/hit/hit_05174.root",
        5,
        2
    );

    hc.AddFile(
        "./../data/hit/hit_05193.root",
        1,
        2
    );

    hc.AddFile(
        "./../data/hit/hit_05218.root",
        2,
        4
    );

    hc.AddFile(
        "./../data/hit/hit_05298.root",
        2,
        6
    );

    hc.AddFile(
        "./../data/hit/hit_05335.root",
        2,
        3
    );

    hc.AddFile(
        "./../data/hit/hit_05394.root",
        2,
        5
    );

    hc.AddFile(
        "./../data/hit/hit_05434.root",
        2,
        0
    );

    fout.cd();
    tree.Write();

    fout.Close();

}