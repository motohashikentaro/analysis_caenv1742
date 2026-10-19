#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"

#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TROOT.h>
#include <TStyle.h>

namespace fs = std::filesystem;

// メタデータ
struct RunMetadata {
    double energy;
    int nw;
    double position;
};

// "05088" と "5088" を同じRunとして扱う。
// "05434-05438" のような複数Runの表記は、この実装では対象外。
int ParseRunNumber(const std::string& text)
{
    std::size_t parsed = 0;
    const int run = std::stoi(text, &parsed);

    if (parsed != text.size() || run < 0) {
        throw std::runtime_error(
            "Invalid single run number: " + text
        );
    }

    return run;
}

std::string Trim(const std::string& text)
{
    const auto begin = text.find_first_not_of(" \t\r\n");

    if (begin == std::string::npos) {
        return "";
    }

    const auto end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

// 単純な数値CSV用。
// 引用符付きフィールドや、フィールド内のカンマは扱わない。
std::vector<std::string> SplitCsv(const std::string& line)
{
    std::vector<std::string> columns;
    std::stringstream stream(line);
    std::string column;

    while (std::getline(stream, column, ',')) {
        columns.push_back(Trim(column));
    }

    return columns;
}

std::map<int, RunMetadata> ReadMetadata(const fs::path& path)
{
    std::ifstream input(path);

    if (!input) {
        throw std::runtime_error(
            "Cannot open metadata: " + path.string()
        );
    }

    std::map<int, RunMetadata> metadata;
    std::string line;
    std::size_t line_number = 0;

    while (std::getline(input, line)) {
        ++line_number;

        line = Trim(line);

        if (line.empty() || line.front() == '#') {
            continue;
        }

        const auto columns = SplitCsv(line);

        if (!columns.empty() && columns[0] == "run_number") {
            continue;
        }

        if (columns.size() < 4) {
            throw std::runtime_error(
                "Metadata needs 4 columns at line "
                + std::to_string(line_number)
            );
        }

        try {
            const int run = ParseRunNumber(columns[0]);

            const RunMetadata values{
                std::stod(columns[1]),
                std::stoi(columns[2]),
                std::stod(columns[3])
            };

            const auto [it, inserted] = metadata.emplace(run, values);

            if (!inserted) {
                throw std::runtime_error(
                    "Duplicate run number: " + columns[0]
                );
            }
        }
        catch (const std::exception& error) {
            throw std::runtime_error(
                "Metadata line " + std::to_string(line_number)
                + ": " + error.what()
            );
        }
    }

    return metadata;
}

// CSV内のファイルパスなどを安全に出力する。
std::string CsvQuote(const std::string& text)
{
    std::string quoted = "\"";

    for (const char c : text) {
        if (c == '"') {
            quoted += "\"\"";
        }
        else {
            quoted += c;
        }
    }

    quoted += '"';
    return quoted;
}

struct FitRange {
    double min;
    double max;
    double sigma_max;
};

// 元コードの設定を維持。
FitRange GetFitRange(
    int run,
    const std::string& side,
    const std::string& axis,
    double peak_position
)
{
    double half_width = 0.5;
    double sigma_max = 1.0;

    if (run == 5265) {
        if (side == "back" && axis == "x") {
            half_width = 2.0;
            sigma_max = 2.0;
        }
        else {
            half_width = 1.0;
        }
    }

    return {
        peak_position - half_width,
        peak_position + half_width,
        sigma_max
    };
}

// ROOTファイル1個を解析。
// メタデータ未登録の場合はスキップしてfalseを返す。
bool AnalyzeFile(
    const fs::path& input_path,
    const std::map<int, RunMetadata>& metadata,
    const fs::path& output_dir,
    std::ofstream& result_csv,
    std::size_t file_index
)
{
    // ThroughEventReaderがconst char*を受け取る想定。
    const std::string input_name = input_path.string();
    ThroughEventReader reader(input_name.c_str());
    ThroughEventData& td = reader.GetThroughEventData();

    if (!td.tree_) {
        throw std::runtime_error("Input tree is null");
    }

    const int run = ParseRunNumber(td.run_number_);

    const auto metadata_it = metadata.find(run);

    if (metadata_it == metadata.end()) {
        std::cerr
            << "[Skip] Metadata not found: Run " << run
            << " (" << input_path << ")\n";

        return false;
    }

    const RunMetadata& meta = metadata_it->second;

    const std::array<std::string, 2> axes{"x", "y"};
    const std::array<std::string, 2> sides{"front", "back"};

    // ファイル間でROOTオブジェクト名が重ならないようにする。
    const std::string tag =
        "file_" + std::to_string(file_index);

    std::unique_ptr<TCanvas> canvas(
        PlotSupporter::MakeCanvas1D(
            ("canvas_" + tag).c_str(), 2, 2
        )
    );

    // キャンバス保存までヒストグラムと関数を保持する。
    std::array<std::unique_ptr<TH1D>, 4> histograms;
    std::array<std::unique_ptr<TF1>, 4> functions;

    std::size_t index = 0;

    for (const auto& axis : axes) {
        for (const auto& side : sides) {
            canvas->cd(index + 1);

            const std::string hist_name =
                "hist_" + tag + "_" + side + "_" + axis;

            const std::string fit_name =
                "fit_" + tag + "_" + side + "_" + axis;

            const std::string hist_title =
                ";" + side + " " + axis
                + "_{reco} - " + axis
                + "_{extrapolated} [mm];Entries";

            const std::string expression =
                "dut_position_" + side + "_" + axis
                + " - extrapolated_" + side + "_" + axis;

            // 入力ファイルにヒストグラムを所有させない。
            // TTree::Drawが名前で見つけられるよう、一時的にgROOTへ登録。
            gROOT->cd();

            histograms[index] = std::make_unique<TH1D>(
                hist_name.c_str(),
                hist_title.c_str(),
                32, -3.0, 3.0
            );

            TH1D& hist = *histograms[index];

            const auto selected = td.tree_->Draw(
                (expression + ">>" + hist_name).c_str(),
                "",
                "goff"
            );

            hist.SetDirectory(nullptr);

            if (selected < 0) {
                throw std::runtime_error(
                    "TTree::Draw failed: " + expression
                );
            }

            const double entries = hist.GetEntries();

            const double underflow =
                hist.GetBinContent(0);

            const double overflow =
                hist.GetBinContent(hist.GetNbinsX() + 1);

            const double nan =
                std::numeric_limits<double>::quiet_NaN();

            double fit_min = nan;
            double fit_max = nan;
            double peak = nan;
            double peak_error = nan;
            double mean = nan;
            double mean_error = nan;
            double sigma = nan;
            double sigma_error = nan;
            double chi2 = nan;
            double chi2_ndf = nan;

            int ndf = 0;
            int fit_status = -1;
            int covariance_status = -1;
            bool fit_valid = false;

            // 通常ビンにデータがあるときだけフィットする。
            if (hist.Integral(1, hist.GetNbinsX()) > 0.0) {
                const double peak_position =
                    hist.GetBinCenter(hist.GetMaximumBin());

                const FitRange range =
                    GetFitRange(run, side, axis, peak_position);

                fit_min = range.min;
                fit_max = range.max;

                functions[index] = std::make_unique<TF1>(
                    fit_name.c_str(),
                    "[0]*exp(-0.5*((x-[1])/[2])^2)",
                    fit_min,
                    fit_max
                );

                TF1& fit = *functions[index];

                fit.SetParNames("Peak", "Mean", "#sigma");

                fit.SetParameters(
                    hist.GetMaximum(),
                    peak_position,
                    0.5
                );

                fit.SetParLimits(2, 0.01, range.sigma_max);

                // R: 指定範囲
                // S: 詳細結果を取得
                // Q: フィットログを抑制
                // N: 関数をヒストグラムへ保存・描画しない
                TFitResultPtr result = hist.Fit(&fit, "RSQN");

                fit_status = static_cast<int>(result);

                if (result.Get()) {
                    fit_valid = result->IsValid();
                    covariance_status = result->CovMatrixStatus();

                    peak = fit.GetParameter(0);
                    peak_error = fit.GetParError(0);

                    mean = fit.GetParameter(1);
                    mean_error = fit.GetParError(1);

                    sigma = fit.GetParameter(2);
                    sigma_error = fit.GetParError(2);

                    chi2 = fit.GetChisquare();
                    ndf = fit.GetNDF();

                    if (ndf > 0) {
                        chi2_ndf = chi2 / static_cast<double>(ndf);
                    }
                }
            }

            PlotSupporter::SetHistStyle1D(&hist);
            hist.Draw("HIST");

            if (functions[index]) {
                functions[index]->Draw("SAME");
            }

            TLatex latex;
            latex.SetNDC();
            latex.SetTextSize(0.035);

            latex.DrawLatex(0.15, 0.89, "Work in Progress");

            latex.DrawLatex(
                0.15, 0.84,
                Form("Run %d", run)
            );

            latex.DrawLatex(
                0.15, 0.79,
                Form("Energy: %.1f GeV", meta.energy)
            );

            latex.DrawLatex(
                0.15, 0.74,
                Form("W plate: %d", meta.nw)
            );

            latex.DrawLatex(
                0.15, 0.69,
                Form("Position: %.2f", meta.position)
            );

            if (fit_valid && fit_status == 0) {
                latex.DrawLatex(
                    0.15, 0.64,
                    Form("#mu = %.3f #pm %.3f mm",
                         mean, mean_error)
                );

                latex.DrawLatex(
                    0.15, 0.59,
                    Form("#sigma = %.3f #pm %.3f mm",
                         sigma, sigma_error)
                );
            }
            else {
                latex.DrawLatex(0.15, 0.64, "Fit failed / no data");
            }

            // 1行 = 1 Run × 1 side × 1 axis
            result_csv
                << run << ','
                << meta.energy << ','
                << meta.nw << ','
                << meta.position << ','
                << side << ','
                << axis << ','
                << entries << ','
                << underflow << ','
                << overflow << ','
                << fit_min << ','
                << fit_max << ','
                << peak << ','
                << peak_error << ','
                << mean << ','
                << mean_error << ','
                << sigma << ','
                << sigma_error << ','
                << chi2 << ','
                << ndf << ','
                << chi2_ndf << ','
                << fit_status << ','
                << (fit_valid ? 1 : 0) << ','
                << covariance_status << ','
                << CsvQuote(input_path.string())
                << '\n';

            ++index;
        }
    }

    const fs::path image_path =
        output_dir / (
            "run_" + std::to_string(run)
            + "_" + tag
            + "_single_gaussian.png"
        );

    canvas->SaveAs(image_path.string().c_str());

    // パッド上の参照を、ヒストグラム破棄前に除去する。
    canvas->Clear();

    result_csv.flush();

    if (!result_csv) {
        throw std::runtime_error("Failed to write result CSV");
    }

    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr
            << "Usage: " << argv[0]
            << " input_directory metadata.csv output_directory\n";

        return 1;
    }

    try {
        const fs::path input_dir = argv[1];
        const fs::path metadata_path = argv[2];
        const fs::path output_dir = argv[3];

        if (!fs::is_directory(input_dir)) {
            throw std::runtime_error(
                "Not a directory: " + input_dir.string()
            );
        }

        const auto metadata = ReadMetadata(metadata_path);

        // 入力ディレクトリ直下の.rootファイルを列挙。
        std::vector<fs::path> input_files;

        for (const auto& entry : fs::directory_iterator(input_dir)) {
            if (entry.is_regular_file()
                && entry.path().extension() == ".root") {
                input_files.push_back(entry.path());
            }
        }

        std::sort(input_files.begin(), input_files.end());

        if (input_files.empty()) {
            throw std::runtime_error("No ROOT files found");
        }

        fs::create_directories(output_dir);

        std::ofstream result_csv(output_dir / "fit_results.csv");

        if (!result_csv) {
            throw std::runtime_error("Cannot open result CSV");
        }

        result_csv << std::setprecision(12);

        result_csv
            << "run_number,energy,nw,position,side,axis,"
            << "entries,underflow,overflow,"
            << "fit_min_mm,fit_max_mm,"
            << "peak,peak_err,"
            << "mean_mm,mean_err_mm,"
            << "sigma_mm,sigma_err_mm,"
            << "chi2,ndf,chi2_ndf,"
            << "fit_status,fit_valid,cov_matrix_status,"
            << "input_file\n";

        // 全Run解析時にウィンドウを開かない。
        gROOT->SetBatch(true);
        gStyle->SetOptStat(0);
        gStyle->SetOptFit(0);

        std::size_t processed = 0;
        std::size_t skipped = 0;
        std::size_t failed = 0;

        for (std::size_t i = 0; i < input_files.size(); ++i) {
            std::cout
                << "[" << i + 1 << "/" << input_files.size() << "] "
                << input_files[i] << '\n';

            try {
                if (AnalyzeFile(
                    input_files[i],
                    metadata,
                    output_dir,
                    result_csv,
                    i
                )) {
                    ++processed;
                }
                else {
                    ++skipped;
                }
            }
            catch (const std::exception& error) {
                // 出力失敗の場合は処理全体を止める。
                if (!result_csv) {
                    throw;
                }

                std::cerr
                    << "[Error] " << input_files[i]
                    << ": " << error.what() << '\n';

                ++failed;
            }
        }

        std::cout
            << "Finished: "
            << processed << " processed, "
            << skipped << " skipped, "
            << failed << " failed\n";

        return failed == 0 ? 0 : 1;
    }
    catch (const std::exception& error) {
        std::cerr << "[Error] " << error.what() << '\n';
        return 1;
    }
}