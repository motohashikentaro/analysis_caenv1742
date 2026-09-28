#ifndef ALIGNMENT_DATA
#define ALIGNMENT_DATA

struct AlignmentData{
    double front_x = -0.594;
    double front_y = 0.048;
    double back_x = -0.412;
    double back_y = -0.876;
};

struct AlignmentStripData{
    double x = -0.471;
    double y = -0.464;
};

struct AlignmentIncludeStripData{
    double front_x = -1.215;
    double front_y = -0.966;
    double back_x = -0.599;
    double back_y = -1.486;
};

struct NonCorNewAlignmentStripData{
    double x = -0.83;
    double y = -1.44;
};

struct NonCorNewAlignmentDutData{
    double front_x = -2.41;
    double front_y = -3.01;
    double back_x = -2.24;
    double back_y = -3.42;
};

struct NewAlignmentStripData{
    double x = -1.07;
    double y = -1.92;
};

struct NewAlignmentDutData{
    double front_x = -1.69;
    double front_y = -2.66;
    double back_x = -1.70;
    double back_y = -2.74;
};

#endif // ALIGNMENT_DATA