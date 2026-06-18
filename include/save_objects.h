#ifndef SAVE_OBJECTS_H
#define SAVE_OBJECTS_H

#include "./../include/rootfile_analyzer.h"

#include <string>

class SaveObjects{
    public:
        SaveObjects(RootData& rd): rd_(rd){};
        std::string MakeSavename(const std::string& picname,
                                 int target_board = -1, 
                                 int target_ch = -1);

    private:
        RootData& rd_;
};

#endif