#include "./../include/save_objects.h"
#include "./../include/rootfile_analyzer.h"

#include <string>
#include <filesystem>

std::string SaveObjects::MakeSavename(const std::string& picname, int target_board, int target_ch){
    std::string parent_path = "./../result/";

    std::filesystem::path root_path(rd_.file_->GetName());
    std::string root_name = root_path.stem().string();
    if(root_name.starts_with("run_")){
        root_name.erase(3, 1);
    }

    std::string b_name = "";
    std::string c_name = "";

    if(target_board != -1){
        b_name = std::string("_b") + std::to_string(target_board);
    }
    if(target_ch != -1){
        c_name = std::string("_c") + std::to_string(target_ch);
    }

    std::string path = parent_path
                     + picname
                     + std::string("_")
                     + root_name
                     + b_name 
                     + c_name 
                     + std::string(".png");
    return path;
}