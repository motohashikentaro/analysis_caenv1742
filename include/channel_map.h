#ifndef CHANNEL_MAP
#define CHANNEL_MAP

struct PixelPosition{
    int x;
    int y;
};

constexpr PixelPosition kChannelMap[2][16]{
    {  // lgad 0
        {1, 0}, // pixel 0   digi ch 0
        {0, 0}, // pixel 1   digi ch 1
        {1, 1}, // pixel 2   digi ch 2
        {0, 1}, // pixel 3   digi ch 3
        {1, 2}, // pixel 4   digi ch 4
        {0, 2}, // pixel 5   digi ch 5
        {1, 3}, // pixel 6   digi ch 6
        {0, 3}, // pixel 7   digi ch 7
        {3, 3}, // pixel 8   digi ch 8
        {2, 3}, // pixel 9   digi ch 9
        {3, 2}, // pixel 10  digi ch 10 
        {2, 2}, // pixel 11  digi ch 11
        {3, 1}, // pixel 12  digi ch 12
        {2, 1}, // pixel 13  digi ch 13
        {3, 0}, // pixel 14  digi ch 14
        {2, 0}  // pixel 15  digi ch 15
    }, {  // lgad 1
        {1, 0}, // pixel 0   digi ch 16
        {0, 0}, // pixel 1   digi ch 17
        {1, 1}, // pixel 2   digi ch 18
        {0, 1}, // pixel 3   digi ch 19
        {1, 2}, // pixel 4   digi ch 20
        {0, 2}, // pixel 5   digi ch 21
        {1, 3}, // pixel 6   digi ch 22
        {0, 3}, // pixel 7   digi ch 23
        {3, 3}, // pixel 8   digi ch 24
        {2, 3}, // pixel 9   digi ch 25
        {3, 2}, // pixel 10  digi ch 26
        {2, 2}, // pixel 11  digi ch 27
        {3, 1}, // pixel 12  digi ch 28
        {2, 1}, // pixel 13  digi ch 29
        {3, 0}, // pixel 14  digi ch 30
        {2, 0}  // pixel 15  digi ch 31
    }
};

#endif