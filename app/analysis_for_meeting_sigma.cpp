#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>
#include <iomanip>
#include <limits>

struct SigmaData {
    double sigma = 0.0;
    double sigma_err = 0.0;
};

struct FitFileData {
    std::string run_number;
    std::map<std::string, SigmaData> sigma_data;
};

FitFileData ReadFitFile(const std::string& filename)
{
    std::ifstream ifs(filename);

    if(!ifs){
        std::cerr << "Failed to open: " << filename << std::endl;
        std::exit(1);
    }

    FitFileData result;

    std::string line;
    std::string current_layer;

    while(std::getline(ifs, line)){

        // Run_number = 05088
        if(line.find("Run_number") != std::string::npos){

            const size_t equal_pos = line.find('=');

            if(equal_pos != std::string::npos){
                result.run_number = line.substr(equal_pos + 1);

                // Remove leading spaces
                const size_t first =
                    result.run_number.find_first_not_of(" \t");

                if(first != std::string::npos){
                    result.run_number =
                        result.run_number.substr(first);
                }
            }

            continue;
        }

        // [front_x], [back_x], [front_y], [back_y]
        if(!line.empty() &&
           line.front() == '[' &&
           line.back() == ']'){

            current_layer =
                line.substr(1, line.size() - 2);

            continue;
        }

        // Sigma_single = value +/- error mm
        if(line.find("Sigma_single") != std::string::npos){

            const size_t equal_pos = line.find('=');
            const size_t plusminus_pos = line.find("+/-");

            if(equal_pos == std::string::npos ||
               plusminus_pos == std::string::npos){
                continue;
            }

            const std::string sigma_str =
                line.substr(
                    equal_pos + 1,
                    plusminus_pos - equal_pos - 1
                );

            const std::string err_str =
                line.substr(plusminus_pos + 3);

            const double sigma =
                std::stod(sigma_str);

            const double sigma_err =
                std::stod(err_str);

            result.sigma_data[current_layer] = {
                sigma,
                sigma_err
            };
        }
    }

    return result;
}


int main(int argc, char* argv[])
{
    if(argc != 4){
        std::cerr
            << "Usage: "
            << argv[0]
            << " <file1.txt> <file2.txt> <output.txt>"
            << std::endl;

        return 1;
    }

    const FitFileData data1 = ReadFitFile(argv[1]);
    const FitFileData data2 = ReadFitFile(argv[2]);

    std::ofstream ofs(argv[3]);

    if(!ofs){
        std::cerr
            << "Failed to open output file: "
            << argv[3]
            << std::endl;

        return 1;
    }

    const std::string layers[] = {
        "front_x",
        "back_x",
        "front_y",
        "back_y"
    };

    ofs << std::fixed << std::setprecision(6);

    // ---------------------------------------------------
    // Header
    // ---------------------------------------------------

    ofs << "Run_1 = "
        << data1.run_number
        << "\n";

    ofs << "Run_2 = "
        << data2.run_number
        << "\n";

    ofs << "Difference = Run_1^2 - Run_2^2\n\n";

    // ---------------------------------------------------
    // Each layer
    // ---------------------------------------------------

    for(const auto& layer : layers){

        if(!data1.sigma_data.contains(layer) ||
           !data2.sigma_data.contains(layer)){

            std::cerr
                << "Missing layer: "
                << layer
                << std::endl;

            continue;
        }

        const double sigma1 =
            data1.sigma_data.at(layer).sigma;

        const double sigma2 =
            data2.sigma_data.at(layer).sigma;

        const double sigma1_err =
            data1.sigma_data.at(layer).sigma_err;

        const double sigma2_err =
            data2.sigma_data.at(layer).sigma_err;


        // ---------------------------------------------------
        // sigma1^2 - sigma2^2
        // ---------------------------------------------------

        const double sigma_squared_diff =
            sigma1 * sigma1
            - sigma2 * sigma2;


        // Error propagation:
        //
        // D = sigma1^2 - sigma2^2
        //
        // delta_D =
        // sqrt(
        //   (2 sigma1 delta_sigma1)^2
        // + (2 sigma2 delta_sigma2)^2
        // )

        const double sigma_squared_diff_err =
            std::sqrt(
                std::pow(
                    2.0 * sigma1 * sigma1_err,
                    2
                )
                +
                std::pow(
                    2.0 * sigma2 * sigma2_err,
                    2
                )
            );


        // ---------------------------------------------------
        // sqrt(sigma1^2 - sigma2^2)
        // ---------------------------------------------------

        double sigma_diff =
            std::numeric_limits<double>::quiet_NaN();

        double sigma_diff_err =
            std::numeric_limits<double>::quiet_NaN();

        if(sigma_squared_diff > 0.0){

            sigma_diff =
                std::sqrt(sigma_squared_diff);

            // Error propagation:
            //
            // R = sqrt(D)
            //
            // delta_R = delta_D / (2 sqrt(D))

            sigma_diff_err =
                sigma_squared_diff_err
                / (2.0 * sigma_diff);
        }


        // ---------------------------------------------------
        // Output
        // ---------------------------------------------------

        ofs << "[" << layer << "]\n";

        ofs << "Sigma_1            = "
            << sigma1
            << " +/- "
            << sigma1_err
            << " mm\n";

        ofs << "Sigma_2            = "
            << sigma2
            << " +/- "
            << sigma2_err
            << " mm\n";

        ofs << "Sigma_squared_diff = "
            << sigma_squared_diff
            << " +/- "
            << sigma_squared_diff_err
            << " mm^2\n";

        ofs << "Sigma_diff         = "
            << sigma_diff
            << " +/- "
            << sigma_diff_err
            << " mm\n";

        ofs << "\n";
    }

    ofs.close();

    return 0;
}