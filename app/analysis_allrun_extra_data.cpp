#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"

#include <algorithm>
#include <array>
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

#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TString.h>

namespace fs = std::filesystem;

namespace {

// ヒストグラム・フィットの共通設定
constexpr int kHistogramBins = 32;
constexpr double kHistogramMin = -3.0;
constexpr double kHistogramMax = 3.0;

constexpr double kFitHalfWidth = 1.5;
constexpr double kInitialSigma = 0.5;
constexpr double kSigmaMin = 0.01;

struct RunMetadata {
    double energy;
    int nw;
    double position;
};

std::string Trim(const std::string& text)
{
    const auto begin = text.find_first_not_of(" \t\r\n");

    if (begin == std::string::npos) {
        return "";
    }

    const auto end = text.find_last_not_of(" \t\r\n");

    return text.substr(begin, end - begin + 1);
}

// "05088" と "5088" を同じRun番号として扱う。
// "05434-05438" のような複数Run表記は対象外。
int ParseRunNumber(const std::string& text)
{
    const std::string value = Trim(text);

    if (value.empty()
        || value.find_first_not_of("0123456789") != std::string::npos) {
        throw std::runtime_error(
            "Invalid single run number: " + text
        );
    }

    return std::stoi(value);
}

// 数値のみの単純なCSVを想定。
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

        // UTF-8 BOM付きCSVにも対応。
        if (line_number == 1
            && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }

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

            const bool inserted =
                metadata.emplace(run, values).second;

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

    if (metadata.empty()) {
        throw std::runtime_error(
            "Metadata contains no runs: " + path.string()
        );
    }

    return metadata;
}

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

// σの上限制約は前の実装を維持。
double GetSigmaMax(
    int run,
    const std::string& side,
    const std::string& axis
)
{
    if (run == 5265 && side == "back" && axis == "x") {
        return 2.0;
    }

    return 1.0;
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
            << "[Skip] Metadata not found: Run "
            << run << " (" << input_path << ")\n";

        return false;
    }

    const RunMetadata& meta = metadata_it->second;

    const std::array<std::string, 2> axes{"x", "y"};
    const std::array<std::string, 2> sides{"front", "back"};

    // ROOTオブジェクト名の重複を防ぐ。
    const std::string tag =
        "file_" + std::to_string(file_index);

    std::array<std::unique_ptr<TH1D>, 4> histograms;
    std::array<std::unique_ptr<TF1>, 4> functions;

    // histograms/functionsより後に宣言し、先に破棄する。
    std::unique_ptr<TCanvas> canvas(
        PlotSupporter::MakeCanvas1D(
            ("canvas_" + tag).c_str(), 2, 2
        )
    );

    // 1ファイル分の結果を、解析完了後にまとめて書く。
    std::ostringstream rows;
    rows << std::setprecision(12);

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

            // TTree::Drawが名前で見つけられるように登録。
            gROOT->cd();

            histograms[index] = std::make_unique<TH1D>(
                hist_name.c_str(),
                hist_title.c_str(),
                kHistogramBins,
                kHistogramMin,
                kHistogramMax
            );

            TH1D& hist = *histograms[index];
            hist.SetDirectory(gROOT);

            const auto selected = td.tree_->Draw(
                (expression + ">>" + hist_name).c_str(),
                "",
                "goff"
            );

            // 入力ファイルの寿命から切り離す。
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

            const double sigma_max =
                GetSigmaMax(run, side, axis);

            double peak_position = nan;
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

            if (hist.Integral(1, hist.GetNbinsX()) > 0.0) {
                peak_position =
                    hist.GetBinCenter(hist.GetMaximumBin());

                // 全Run・全方向で最大ビン中心±1.5 mm。
                fit_min = peak_position - kFitHalfWidth;
                fit_max = peak_position + kFitHalfWidth;

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
                    kInitialSigma
                );

                fit.SetParLimits(
                    2,
                    kSigmaMin,
                    sigma_max
                );

                // R: TF1で指定した範囲
                // S: 詳細なフィット結果を取得
                // Q: フィットログを抑制
                // N: 関数をヒストグラムに保存・描画しない
                TFitResultPtr result =
                    hist.Fit(&fit, "RSQN");

                fit_status = static_cast<int>(result);

                if (result.Get()) {
                    fit_valid = result->IsValid();

                    covariance_status =
                        result->CovMatrixStatus();

                    peak = fit.GetParameter(0);
                    peak_error = fit.GetParError(0);

                    mean = fit.GetParameter(1);
                    mean_error = fit.GetParError(1);

                    sigma = fit.GetParameter(2);
                    sigma_error = fit.GetParError(2);

                    chi2 = fit.GetChisquare();
                    ndf = fit.GetNDF();

                    if (ndf > 0) {
                        chi2_ndf =
                            chi2 / static_cast<double>(ndf);
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

            latex.DrawLatex(
                0.15, 0.89,
                "Work in Progress"
            );

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

            // positionの単位はメタデータの定義に従う。
            latex.DrawLatex(
                0.15, 0.69,
                Form("Position: %.2f", meta.position)
            );

            if (fit_valid && fit_status == 0) {
                latex.DrawLatex(
                    0.15, 0.64,
                    Form(
                        "#mu = %.3f #pm %.3f mm",
                        mean,
                        mean_error
                    )
                );

                latex.DrawLatex(
                    0.15, 0.59,
                    Form(
                        "#sigma = %.3f #pm %.3f mm",
                        sigma,
                        sigma_error
                    )
                );
            }
            else {
                latex.DrawLatex(
                    0.15, 0.64,
                    "Fit failed / no data"
                );
            }

            rows
                << run << ','
                << meta.energy << ','
                << meta.nw << ','
                << meta.position << ','
                << side << ','
                << axis << ','
                << entries << ','
                << underflow << ','
                << overflow << ','
                << peak_position << ','
                << fit_min << ','
                << fit_max << ','
                << kSigmaMin << ','
                << sigma_max << ','
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
    canvas->Clear();

    result_csv << rows.str();
    result_csv.flush();

    if (!result_csv) {
        throw std::runtime_error(
            "Failed to write result CSV"
        );
    }

    return true;
}

fs::path FindProjectDirectory()
{
    fs::path directory = fs::current_path();

    while (true) {
        const bool has_app =
            fs::is_directory(directory / "app");

        const bool has_metadata =
            fs::is_regular_file(
                directory / "data" / "run_metadata.csv"
            );

        if (has_app && has_metadata) {
            return directory;
        }

        const fs::path parent = directory.parent_path();

        if (parent == directory) {
            break;
        }

        directory = parent;
    }

    throw std::runtime_error(
        "Project directory not found. "
        "Run inside the project containing "
        "app/ and data/run_metadata.csv."
    );
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr
            << "Usage: " << argv[0]
            << " input_directory\n"
            << "Relative input paths are resolved "
            << "from the project root.\n";

        return 1;
    }

    try {
        const fs::path project_dir =
            FindProjectDirectory();

        // 相対パスは、実行位置によらずプロジェクトルート基準。
        // 絶対パスも指定可能。
        const fs::path input_argument = argv[1];

        const fs::path input_dir =
            input_argument.is_absolute()
                ? input_argument
                : project_dir / input_argument;

        const fs::path metadata_path =
            project_dir / "data" / "run_metadata.csv";

        const fs::path output_dir =
            project_dir / "result" / "etc";

        if (!fs::is_directory(input_dir)) {
            throw std::runtime_error(
                "Not a directory: " + input_dir.string()
            );
        }

        const auto metadata =
            ReadMetadata(metadata_path);

        std::vector<fs::path> input_files;

        // 指定した解析条件ディレクトリの直下だけを読む。
        for (const auto& entry : fs::directory_iterator(input_dir)) {
            if (entry.is_regular_file()
                && entry.path().extension() == ".root") {
                input_files.push_back(entry.path());
            }
        }

        std::sort(
            input_files.begin(),
            input_files.end()
        );

        if (input_files.empty()) {
            throw std::runtime_error(
                "No ROOT files found directly in: "
                + input_dir.string()
            );
        }

        fs::create_directories(output_dir);

        const fs::path result_path =
            output_dir / "fit_results.csv";

        std::ofstream result_csv(result_path);

        if (!result_csv) {
            throw std::runtime_error(
                "Cannot open result CSV: "
                + result_path.string()
            );
        }

        result_csv
            << "run_number,energy,nw,position,side,axis,"
            << "entries,underflow,overflow,"
            << "peak_bin_center_mm,"
            << "fit_min_mm,fit_max_mm,"
            << "sigma_min_mm,sigma_max_mm,"
            << "peak,peak_err,"
            << "mean_mm,mean_err_mm,"
            << "sigma_mm,sigma_err_mm,"
            << "chi2,ndf,chi2_ndf,"
            << "fit_status,fit_valid,cov_matrix_status,"
            << "input_file\n";

        gROOT->SetBatch(true);
        gStyle->SetOptStat(0);
        gStyle->SetOptFit(0);

        std::cout
            << "Project : " << project_dir << '\n'
            << "Input   : " << input_dir << '\n'
            << "Metadata: " << metadata_path << '\n'
            << "Output  : " << output_dir << '\n';

        std::size_t processed = 0;
        std::size_t skipped = 0;
        std::size_t failed = 0;

        for (std::size_t i = 0; i < input_files.size(); ++i) {
            std::cout
                << "[" << i + 1
                << "/" << input_files.size() << "] "
                << input_files[i].filename() << '\n';

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
                if (!result_csv) {
                    throw;
                }

                std::cerr
                    << "[Error] " << input_files[i]
                    << ": " << error.what() << '\n';

                ++failed;
            }
        }

        result_csv.close();

        if (!result_csv) {
            throw std::runtime_error(
                "Failed to close result CSV"
            );
        }

        std::cout
            << "\nFinished: "
            << processed << " processed, "
            << skipped << " skipped, "
            << failed << " failed\n"
            << "Results: " << result_path << '\n';

        return failed == 0 ? 0 : 1;
    }
    catch (const std::exception& error) {
        std::cerr
            << "[Error] " << error.what() << '\n';

        return 1;
    }
}