#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TColor.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TROOT.h>
#include <TStyle.h>

namespace fs = std::filesystem;

namespace {

struct Reference {
    double sigma;
    double error;
};

struct Direction {
    std::string side;
    std::string axis;
    Reference reference;
};

struct Point {
    int run;
    double energy;
    double sigma_diff;
    double error;
};

// 全エネルギーで共通に使用するタングステンなしの基準。
const std::array<Direction, 4> kDirections{{
    {"front", "x", {0.385919, 0.030263}},
    {"back",  "x", {0.354746, 0.025924}},
    {"front", "y", {0.375240, 0.029639}},
    {"back",  "y", {0.283033, 0.012860}}
}};

// 方向ごとに、nw -> 点の一覧を保持。
using PointsByPlate = std::map<int, std::vector<Point>>;

std::string Trim(const std::string& text)
{
    const auto begin = text.find_first_not_of(" \t\r\n");

    if (begin == std::string::npos) {
        return "";
    }

    const auto end = text.find_last_not_of(" \t\r\n");

    return text.substr(begin, end - begin + 1);
}

// 引用符付きフィールドにも対応。
// fit_results.csvのinput_file列を含めて読み取る。
std::vector<std::string> ParseCsvLine(const std::string& line)
{
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];

        if (c == '"') {
            if (quoted
                && i + 1 < line.size()
                && line[i + 1] == '"') {
                field += '"';
                ++i;
            }
            else {
                quoted = !quoted;
            }
        }
        else if (c == ',' && !quoted) {
            fields.push_back(Trim(field));
            field.clear();
        }
        else {
            field += c;
        }
    }

    if (quoted) {
        throw std::runtime_error("Unclosed CSV quotation");
    }

    fields.push_back(Trim(field));

    return fields;
}

int PlateColor(int nw)
{
    switch (nw) {
        case 2: return kBlue + 1;
        case 3: return kOrange + 7;
        case 4: return kGreen + 2;
        case 5: return kMagenta + 1;
        case 6: return kRed + 1;
        default: return kBlack;
    }
}

int PlateMarker(int nw)
{
    switch (nw) {
        case 2: return 20;
        case 3: return 21;
        case 4: return 22;
        case 5: return 23;
        case 6: return 33;
        default: return 24;
    }
}

std::array<PointsByPlate, 4> ReadPoints(const fs::path& path)
{
    std::ifstream input(path);

    if (!input) {
        throw std::runtime_error(
            "Cannot open: " + path.string()
        );
    }

    std::string line;

    if (!std::getline(input, line)) {
        throw std::runtime_error("CSV is empty");
    }

    if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
        line.erase(0, 3);
    }

    const auto header = ParseCsvLine(line);

    std::map<std::string, std::size_t> columns;

    for (std::size_t i = 0; i < header.size(); ++i) {
        columns[header[i]] = i;
    }

    const std::array<std::string, 10> required{{
        "run_number",
        "energy",
        "nw",
        "position",
        "side",
        "axis",
        "sigma_mm",
        "sigma_err_mm",
        "fit_status",
        "fit_valid"
    }};

    for (const auto& name : required) {
        if (columns.find(name) == columns.end()) {
            throw std::runtime_error(
                "Missing CSV column: " + name
            );
        }
    }

    std::array<PointsByPlate, 4> points;
    std::size_t line_number = 1;

    while (std::getline(input, line)) {
        ++line_number;

        if (Trim(line).empty()) {
            continue;
        }

        try {
            const auto fields = ParseCsvLine(line);

            if (fields.size() != header.size()) {
                throw std::runtime_error(
                    "Column count does not match header"
                );
            }

            const auto get = [&](const std::string& name)
                -> const std::string& {
                return fields.at(columns.at(name));
            };

            const double position = std::stod(get("position"));

            if (!std::isfinite(position)) {
                throw std::runtime_error("Invalid position");
            }

            if (std::abs(position + 1.0) > 1e-6) {
                continue;
            }

            const int run = std::stoi(get("run_number"));

            // 基準測定をタングステンありの系列に入れない。
            if (run >= 5434 && run <= 5438) {
                continue;
            }

            const int fit_status =
                std::stoi(get("fit_status"));

            const std::string valid_text = get("fit_valid");

            const bool fit_valid =
                valid_text == "1" || valid_text == "true";

            if (fit_status != 0 || !fit_valid) {
                std::cerr
                    << "[Skip] Invalid fit: Run " << run
                    << " " << get("side")
                    << "_" << get("axis") << '\n';

                continue;
            }

            const int nw = std::stoi(get("nw"));

            if (nw <= 0) {
                continue;
            }

            const double energy = std::stod(get("energy"));
            const double sigma = std::stod(get("sigma_mm"));
            const double sigma_error =
                std::stod(get("sigma_err_mm"));

            if (!std::isfinite(energy)
                || !std::isfinite(sigma)
                || !std::isfinite(sigma_error)
                || energy <= 0.0
                || sigma <= 0.0
                || sigma_error < 0.0) {
                throw std::runtime_error(
                    "Invalid energy, sigma, or sigma error"
                );
            }

            bool direction_found = false;

            for (std::size_t d = 0; d < kDirections.size(); ++d) {
                const auto& direction = kDirections[d];

                if (get("side") != direction.side
                    || get("axis") != direction.axis) {
                    continue;
                }

                direction_found = true;

                const auto& reference = direction.reference;

                const double variance_difference =
                    sigma * sigma
                    - reference.sigma * reference.sigma;

                if (variance_difference <= 0.0) {
                    std::cerr
                        << "[Skip] Non-positive variance difference: "
                        << "Run " << run << " "
                        << direction.side << "_"
                        << direction.axis << '\n';

                    break;
                }

                const double sigma_diff =
                    std::sqrt(variance_difference);

                const double error =
                    std::hypot(
                        sigma * sigma_error,
                        reference.sigma * reference.error
                    ) / sigma_diff;

                points[d][nw].push_back({
                    run,
                    energy,
                    sigma_diff,
                    error
                });

                break;
            }

            if (!direction_found) {
                throw std::runtime_error(
                    "Unknown side/axis: "
                    + get("side") + "_" + get("axis")
                );
            }
        }
        catch (const std::exception& error) {
            // 不正な行を黙って無視しない。
            throw std::runtime_error(
                "CSV line " + std::to_string(line_number)
                + ": " + error.what()
            );
        }
    }

    return points;
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3) {
        std::cerr
            << "Usage: " << argv[0]
            << " fit_results.csv [output_directory]\n";

        return 1;
    }

    try {
        const fs::path input_path = argv[1];

        // 出力先省略時は入力CSVと同じディレクトリ。
        const fs::path output_dir =
            argc == 3
                ? fs::path(argv[2])
                : (
                    input_path.has_parent_path()
                        ? input_path.parent_path()
                        : fs::path(".")
                );

        auto points = ReadPoints(input_path);

        bool has_points = false;

        for (const auto& direction_points : points) {
            if (!direction_points.empty()) {
                has_points = true;
                break;
            }
        }

        if (!has_points) {
            throw std::runtime_error(
                "No valid points for position = -1"
            );
        }

        fs::create_directories(output_dir);

        gROOT->SetBatch(true);
        gStyle->SetOptStat(0);

        // TMultiGraphが追加されたTGraphErrorsを所有する。
        std::array<std::unique_ptr<TMultiGraph>, 4> multigraphs;
        std::array<std::unique_ptr<TLegend>, 4> legends;

        // 描画対象より先にキャンバスを破棄する宣言順。
        TCanvas canvas(
            "energy_dependence",
            "Energy dependence",
            1400,
            1000
        );

        canvas.Divide(2, 2);

        for (std::size_t d = 0; d < kDirections.size(); ++d) {
            canvas.cd(d + 1);

            const auto& direction = kDirections[d];

            multigraphs[d] = std::make_unique<TMultiGraph>();

            const std::string title =
                direction.side + " " + direction.axis
                + " (position = -1)"
                + ";Beam energy [GeV]"
                + ";#sqrt{#sigma_{W}^{2}-#sigma_{0}^{2}} [mm]";

            multigraphs[d]->SetTitle(title.c_str());

            legends[d] = std::make_unique<TLegend>(
                0.68, 0.66, 0.88, 0.88
            );

            legends[d]->SetBorderSize(0);
            legends[d]->SetFillStyle(0);

            for (auto& [nw, values] : points[d]) {
                // 同じエネルギーの点も削除せず保持する。
                std::stable_sort(
                    values.begin(),
                    values.end(),
                    [](const Point& a, const Point& b) {
                        return a.energy < b.energy;
                    }
                );

                auto graph = std::make_unique<TGraphErrors>();

                const std::string name =
                    "graph_" + direction.side
                    + "_" + direction.axis
                    + "_nw" + std::to_string(nw);

                graph->SetName(name.c_str());
                graph->SetMarkerColor(PlateColor(nw));
                graph->SetLineColor(PlateColor(nw));
                graph->SetMarkerStyle(PlateMarker(nw));
                graph->SetMarkerSize(1.0);

                for (std::size_t i = 0; i < values.size(); ++i) {
                    graph->SetPoint(
                        static_cast<int>(i),
                        values[i].energy,
                        values[i].sigma_diff
                    );

                    // 横軸の誤差は指定せず0とする。
                    graph->SetPointError(
                        static_cast<int>(i),
                        0.0,
                        values[i].error
                    );
                }

                const std::string label =
                    "W plates: " + std::to_string(nw);

                legends[d]->AddEntry(
                    graph.get(),
                    label.c_str(),
                    "pe"
                );

                // 同一エネルギーに複数点があるため線で結ばない。
                multigraphs[d]->Add(graph.release(), "P");

                std::cout
                    << direction.side << "_" << direction.axis
                    << ", nw=" << nw
                    << ": " << values.size() << " points\n";
            }

            if (!points[d].empty()) {
                multigraphs[d]->Draw("AP");
                legends[d]->Draw();
            }
        }

        canvas.SaveAs(
            (output_dir / "energy_dependence_position_minus1.png")
                .string().c_str()
        );

        canvas.SaveAs(
            (output_dir / "energy_dependence_position_minus1.pdf")
                .string().c_str()
        );

        canvas.Clear();

        return 0;
    }
    catch (const std::exception& error) {
        std::cerr
            << "[Error] " << error.what() << '\n';

        return 1;
    }
}