// #ifndef HIT_CORRELATION_H
// #define HIT_CORRELATION_H

// #include <vector>
// #include <string>

// #include <RtypesCore.h>
// #include <TTree.h>

// struct Hit{
//     int ch;

//     double peak_adc;
//     double charge;
//     double peak_time;
//     double tot;
// };

// struct EventHit{
//     Long64_t event;

//     // front LGAD
//     int nfront;
//     double front_charge;
//     double front_max_adc;

//     std::vector<int> front_ch;

//     // back LGAD
//     int nback;
//     double back_charge;
//     double back_max_adc;

//     std::vector<int> back_ch;
// };

// class HitCorrelation{

//     public:
//         HitCorrelation(TTree* tree);

//         void AddFile(
//             const std::string& filename,
//             int energy,
//             int nw
//         );

//     private:
//         TTree* tree_;

//         int energy_;
//         int nw_;

//         EventHit hit_;
// };

// #endif

#ifndef HIT_CORRELATION_H
#define HIT_CORRELATION_H

#include <vector>
#include <string>

#include <RtypesCore.h>
#include <TTree.h>


// struct Hit{

//     int ch;

//     double peak_adc;
//     double charge;
//     int peak_time;
//     double tot;

// };


struct EventHit{

    double event = -1;


    // front LGAD (ch 0-15)

    // event level
    int nfront = 0;
    double front_charge = 0;
    double front_max_adc = 0;


    // hit level
    std::vector<int> front_ch;
    std::vector<double> front_peak_adc;
    std::vector<double> front_charge_each;
    std::vector<int> front_peak_time;
    std::vector<double> front_tot;



    // back LGAD (ch 16-31)

    // event level
    int nback = 0;
    double back_charge = 0;
    double back_max_adc = 0;


    // hit level
    std::vector<int> back_ch;
    std::vector<double> back_peak_adc;
    std::vector<double> back_charge_each;
    std::vector<int> back_peak_time;
    std::vector<double> back_tot;

};



class HitCorrelation{

    public:

        HitCorrelation(TTree* tree);


        void AddFile(
            const std::string& filename,
            int energy,
            int nw
        );


    private:

        TTree* tree_;


        int energy_;
        int nw_;


        EventHit hit_;

};


#endif