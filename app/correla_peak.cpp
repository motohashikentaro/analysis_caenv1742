#include "./../include/correla_peak.h"

#include <string>


int main()
{

    CorrelaPeak ana(
        "./../data/correlation/correlation.root"
    );


    ana.Analyze();


    ana.Draw();


    ana.Save(
        "./../result"
    );


    return 0;

}