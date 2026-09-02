#ifndef CHANNEL_MAP
#define CHANNEL_MAP

struct PixelPosition{
    int x;
    int y;
};

constexpr PixelPosition kChannelMap[2][16]{
    {  // board 1, lgad 0
        {2, 0}, // pixel 0   digi ch 0
        {3, 0}, // pixel 1   digi ch 1
        {2, 1}, // pixel 2   digi ch 2
        {3, 1}, // pixel 3   digi ch 3
        {2, 2}, // pixel 4   digi ch 4
        {3, 2}, // pixel 5   digi ch 5
        {2, 3}, // pixel 6   digi ch 6
        {3, 3}, // pixel 7   digi ch 7
        {0, 3}, // pixel 8   digi ch 8
        {1, 3}, // pixel 9   digi ch 9
        {0, 2}, // pixel 10  digi ch 10
        {1, 2}, // pixel 11  digi ch 11
        {0, 1}, // pixel 12  digi ch 12
        {1, 1}, // pixel 13  digi ch 13
        {0, 0}, // pixel 14  digi ch 14
        {1, 0}  // pixel 15  digi ch 15
    }, {  // board 1, lgad 1
        {2, 0}, // pixel 0   digi ch 16
        {3, 0}, // pixel 1   digi ch 17
        {2, 1}, // pixel 2   digi ch 18
        {3, 1}, // pixel 3   digi ch 19
        {2, 2}, // pixel 4   digi ch 20
        {3, 2}, // pixel 5   digi ch 21
        {2, 3}, // pixel 6   digi ch 22
        {3, 3}, // pixel 7   digi ch 23
        {0, 3}, // pixel 8   digi ch 24
        {1, 3}, // pixel 9   digi ch 25
        {0, 2}, // pixel 10  digi ch 26
        {1, 2}, // pixel 11  digi ch 27
        {0, 1}, // pixel 12  digi ch 28
        {1, 1}, // pixel 13  digi ch 29
        {0, 0}, // pixel 14  digi ch 30
        {1, 0}  // pixel 15  digi ch 31
    }
};

inline constexpr std::array<int, 5> dead_channels = {1, 8, 16, 18, 28}; // digi ch

inline int Digi2PixelCh(int digi_ch){
    if(digi_ch >= 0 && digi_ch < 16){
        return digi_ch;
    }else if(digi_ch >= 16 && digi_ch < 32){
        return digi_ch - 16;
    }else{
        return -1;
    }
}

inline PixelPosition Digi2PixelPosition(int digi_ch){
    if(digi_ch >= 0 && digi_ch < 16){
        return kChannelMap[0][digi_ch];
    }else if(digi_ch >= 16 && digi_ch < 32){
        return kChannelMap[1][digi_ch - 16];
    }else{
        return PixelPosition{-1, -1};
    }
}

inline int Digi2Lgad(int digi_ch){
    if(digi_ch >= 0 && digi_ch < 16){
        return 0;
    }else if(digi_ch >= 16 && digi_ch < 32){
        return 1;
    }else{
        return -1;
    }
}

inline int PixelPosition2Digi(int lgad, const PixelPosition& pos){
    if(lgad < 0 || lgad > 1) return -1;
    if(pos.x < 0 || pos.x > 3) return -1;
    if(pos.y < 0 || pos.y > 3) return -1;

    for(int ch=0; ch<16; ++ch){
        if(kChannelMap[lgad][ch].x == pos.x && kChannelMap[lgad][ch].y == pos.y){
            return ch + lgad * 16;
        }
    }

    return -1;
}

struct StripPosition{
    int layer;
    int strip;
};

constexpr StripPosition kStripMap[4][8]{
    {  // board 0, front x
        {0, 0}, // front x strip 0  digi ch 0
        {0, 1}, // front x strip 1  digi ch 1
        {0, 2}, // front x strip 2  digi ch 2
        {0, 3}, // front x strip 3  digi ch 3
        {0, 4}, // front x strip 4  digi ch 4
        {0, 5}, // front x strip 5  digi ch 5
        {0, 6}, // front x strip 6  digi ch 6
        {0, 7}  // front x strip 7  digi ch 7
    }, {  // front y
        {1, 8}, // front y strip 8  digi ch 8
        {1, 9}, // front y strip 9  digi ch 9
        {1, 10}, // front y strip 10  digi ch 10
        {1, 11}, // front y strip 11  digi ch 11
        {1, 12}, // front y strip 12  digi ch 12
        {1, 13}, // front y strip 13  digi ch 13
        {1, 14}, // front y strip 14  digi ch 14
        {1, 15}  // front y strip 15  digi ch 15
    }, {  // back x
        {2, 1}, // back x strip 1  digi ch 16
        {2, 2}, // back x strip 2  digi ch 17
        {2, 3}, // back x strip 3  digi ch 18
        {2, 4}, // back x strip 4  digi ch 19
        {2, 5}, // back x strip 5  digi ch 20
        {2, 6}, // back x strip 6  digi ch 21
        {2, 7}, // back x strip 7  digi ch 22
        {2, 8}  // back x strip 8  digi ch 23
    }, {  // back y
        {3, 9}, // back y strip 9  digi ch 24
        {3, 10}, // back y strip 10  digi ch 25
        {3, 11}, // back y strip 11  digi ch 26
        {3, 12}, // back y strip 12  digi ch 27
        {3, 13}, // back y strip 13  digi ch 28
        {3, 14}, // back y strip 14  digi ch 29
        {3, 15}, // back y strip 15  digi ch 30
        {3, 16}  // back y strip 16  digi ch 31
    }
};

inline StripPosition Digi2StripPosition(int digi_ch){
    if(digi_ch >= 0 && digi_ch < 8){
        return kStripMap[0][digi_ch];
    }else if(digi_ch >= 8 && digi_ch < 16){
        return kStripMap[1][digi_ch - 8];
    }else if(digi_ch >= 16 && digi_ch < 24){
        return kStripMap[2][digi_ch - 16];
    }else if(digi_ch >= 24 && digi_ch < 32){
        return kStripMap[3][digi_ch - 24];
    }else{
        return StripPosition{-1, -1};
    }
}

inline StripPosition Digi2CorrectStripPosition(int digi_ch){
    if(digi_ch < 0 || digi_ch >= 32){
        return StripPosition{-1, -1};
    }

    return StripPosition{digi_ch / 8, digi_ch % 8};
}



#endif