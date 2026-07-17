#include "./../include/correla_multiplicity.h"


int main()
{

    CorrelaMultiplicity cm(
        "./../data/correlation/correlation.root"
    );


    cm.Analyze();

    cm.Draw();

    cm.Save(
        "./../result"
    );


    return 0;
}