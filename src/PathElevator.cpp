/**
 * @file PathElevator.cpp
 * @brief PathElevator class implementation
 */

#include "PathElevator.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

/* -------------------------------------------------------------------------
 * ANSI colour helpers (Windows 10+ and all Unix terminals)
 * ---------------------------------------------------------------------- */

namespace clr {
    static constexpr const char *rst  = "\033[0m";
    static constexpr const char *dim  = "\033[2m";
    static constexpr const char *bcyn = "\033[1;36m";  /* bold cyan    – banners     */
    static constexpr const char *bwht = "\033[1;37m";  /* bold white   – floor label */
    static constexpr const char *cyn  = "\033[36m";    /* cyan         – path        */
    static constexpr const char *bgrn = "\033[1;32m";  /* bold green   – [FILE] tag  */
    static constexpr const char *yel  = "\033[33m";    /* yellow       – [OTHER] tag */
    static constexpr const char *byel = "\033[1;33m";  /* bold yellow  – mode label  */
    static constexpr const char *bmag = "\033[1;35m";  /* bold magenta – arrived     */
}

static void enable_ansi()
{
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  m = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &m))
        SetConsoleMode(h, m | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

/* -------------------------------------------------------------------------
 * Construction & validation
 * ---------------------------------------------------------------------- */

PathElevator::PathElevator(const std::string &target_path,
                           const std::string &root_path)
{
    std::error_code ec;

    target_ = fs::canonical(fs::path(target_path), ec);
    if (ec)
        throw std::invalid_argument("target path does not exist: " + target_path);

    root_ = fs::canonical(fs::path(root_path), ec);
    if (ec)
        throw std::invalid_argument("root path does not exist: " + root_path);

    bool target_is_dir = fs::is_directory(target_, ec);
    if (ec || !target_is_dir)
        throw std::invalid_argument("target path is not a directory: " + target_path);

    bool root_is_dir = fs::is_directory(root_, ec);
    if (ec || !root_is_dir)
        throw std::invalid_argument("root path is not a directory: " + root_path);

    fs::path cur = target_;
    bool found   = false;
    while (true) {
        if (cur == root_) { found = true; break; }
        fs::path parent = cur.parent_path();
        if (parent == cur) break;
        cur = parent;
    }
    if (!found)
        throw std::invalid_argument("root path is not an ancestor of target path");

    levels_ = buildLevels();
    buildCache();
}

/* -------------------------------------------------------------------------
 * Internal helpers
 * ---------------------------------------------------------------------- */

std::vector<fs::path> PathElevator::buildLevels() const
{
    std::vector<fs::path> levels;
    fs::path cur = target_;
    while (true) {
        levels.push_back(cur);
        if (cur == root_) break;
        cur = cur.parent_path();
    }
    std::reverse(levels.begin(), levels.end());
    return levels;
}

void PathElevator::buildCache()
{
    for (const auto &dir : levels_) {
        std::vector<CachedEntry> entries;
        std::error_code iter_ec;

        for (const auto &e : fs::directory_iterator(dir, iter_ec)) {
            std::error_code entry_ec;
            CachedEntry ce;

            ce.name    = e.path().filename().string();
            ce.is_dir  = e.is_directory(entry_ec);
            if (entry_ec) { ce.is_dir = false; entry_ec.clear(); }

            ce.is_file = e.is_regular_file(entry_ec);
            if (entry_ec) { ce.is_file = false; entry_ec.clear(); }

            ce.size = ce.is_file ? e.file_size(entry_ec) : 0;
            if (entry_ec) ce.size = 0;

            entries.push_back(std::move(ce));
        }

        if (iter_ec)
            std::cerr << "ud: warning: could not fully read '"
                      << dir.string() << "': " << iter_ec.message() << "\n";

        std::sort(entries.begin(), entries.end(),
                  [](const CachedEntry &a, const CachedEntry &b) {
                      if (a.is_dir != b.is_dir) return a.is_dir > b.is_dir;
                      return a.name < b.name;
                  });

        cache_[dir] = std::move(entries);
    }
}

void PathElevator::displayLevel(int floor, int total,
                                const fs::path &dir) const
{
    std::cout << "\n"
              << clr::bwht << "[Floor " << floor << "/" << total << "]"
              << clr::rst  << "  "
              << clr::cyn  << dir.string()
              << clr::rst  << "\n";
    std::cout << clr::dim << std::string(60, '-') << clr::rst << "\n";

    auto it = cache_.find(dir);
    if (it == cache_.end()) {
        std::cout << "  (not cached)\n";
        return;
    }

    const auto &entries = it->second;
    if (entries.empty()) {
        std::cout << "  (empty)\n";
        return;
    }

    for (const auto &e : entries) {
        if (e.is_dir)
            std::cout << "  "
                      << clr::bcyn << "[DIR ]" << clr::rst << " "
                      << clr::cyn  << e.name   << "/" << clr::rst << "\n";
        else if (e.is_file)
            std::cout << "  "
                      << clr::bgrn << "[FILE]" << clr::rst << " "
                      << e.name << "\n";
        else
            std::cout << "  "
                      << clr::yel << "[OTHER]" << clr::rst << " "
                      << e.name << "\n";
    }
}

/* -------------------------------------------------------------------------
 * Public interface
 * ---------------------------------------------------------------------- */

void PathElevator::ride(Mode mode, const fs::path &stop_at) const
{
    enable_ansi();

    const int   total = static_cast<int>(levels_.size());
    const char *line  = "============================================================";
    fs::path    last_shown;

    const char *mode_str = (mode == Mode::ASCEND)
        ? "ASCEND  (target -> root)"
        : "DESCEND  (root -> target)";

    std::cout << "\n"
              << clr::bcyn << line << clr::rst << "\n"
              << " Path Elevator  |  " << clr::byel << mode_str << clr::rst << "\n"
              << clr::bcyn << line << clr::rst << "\n";

    if (mode == Mode::ASCEND) {
        for (int i = total - 1; i >= 0; --i) {
            displayLevel(i + 1, total, levels_[i]);
            last_shown = levels_[i];
            if (!stop_at.empty() && levels_[i] == stop_at) break;
        }
    } else {
        for (int i = 0; i < total; ++i) {
            displayLevel(i + 1, total, levels_[i]);
            last_shown = levels_[i];
            if (!stop_at.empty() && levels_[i] == stop_at) break;
        }
    }

    std::cout << "\n"
              << clr::bcyn << line << clr::rst << "\n"
              << clr::bmag << " Arrived at: " << clr::rst
              << clr::cyn  << last_shown.string() << clr::rst << "\n"
              << clr::bcyn << line << clr::rst << "\n";
}

/* -------------------------------------------------------------------------
 * ud command
 * ---------------------------------------------------------------------- */

int PathElevator::ud(const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        std::cerr << "ud: usage: ud <start> <end> [<stop>]\n"
                  << "  start/end  two directory names from the CWD ancestor chain;\n"
                  << "             their relative depths determine direction\n"
                  << "  stop       optional: display halts at this level\n";
        return 1;
    }

    const std::string &start_seg = args[0];
    const std::string &end_seg   = args[1];
    const std::string *stop_seg  = args.size() >= 3 ? &args[2] : nullptr;

    std::error_code ec;
    fs::path cwd_raw = fs::current_path(ec);
    if (ec) {
        std::cerr << "ud: cannot resolve current directory: " << ec.message() << "\n";
        return 1;
    }
    fs::path cwd = fs::canonical(cwd_raw, ec);
    if (ec) {
        std::cerr << "ud: cannot canonicalize current directory: " << ec.message() << "\n";
        return 1;
    }

    std::vector<fs::path> all_levels;
    for (fs::path cur = cwd; ; cur = cur.parent_path()) {
        all_levels.push_back(cur);
        if (cur == cur.parent_path()) break;
    }
    std::reverse(all_levels.begin(), all_levels.end());

    std::vector<std::string> names;
    for (const auto &p : all_levels)
        names.push_back(p.filename().string());

    auto find_seg = [&](const std::string &seg) -> int {
        auto it = std::find(names.begin(), names.end(), seg);
        if (it == names.end()) return -1;
        return static_cast<int>(std::distance(names.begin(), it));
    };

    int start_idx = find_seg(start_seg);
    int end_idx   = find_seg(end_seg);

    if (start_idx < 0) {
        std::cerr << "ud: '" << start_seg << "' not found in: "
                  << cwd.string() << "\n";
        return 1;
    }
    if (end_idx < 0) {
        std::cerr << "ud: '" << end_seg << "' not found in: "
                  << cwd.string() << "\n";
        return 1;
    }
    if (start_idx == end_idx) {
        std::cerr << "ud: start and end must be different directories\n";
        return 1;
    }

    int stop_idx = -1;
    if (stop_seg) {
        stop_idx = find_seg(*stop_seg);
        if (stop_idx < 0) {
            std::cerr << "ud: '" << *stop_seg << "' not found in: "
                      << cwd.string() << "\n";
            return 1;
        }
        int lo = std::min(start_idx, end_idx);
        int hi = std::max(start_idx, end_idx);
        if (stop_idx < lo || stop_idx > hi) {
            std::cerr << "ud: stop '" << *stop_seg
                      << "' is not between '" << start_seg
                      << "' and '" << end_seg << "'\n";
            return 1;
        }
    }

    Mode mode = (start_idx < end_idx) ? Mode::DESCEND : Mode::ASCEND;

    int      root_idx   = std::min(start_idx, end_idx);
    int      target_idx = std::max(start_idx, end_idx);
    fs::path root_p     = all_levels[root_idx];
    fs::path target_p   = all_levels[target_idx];
    fs::path stop_p     = (stop_idx >= 0) ? all_levels[stop_idx] : fs::path{};

    try {
        PathElevator pe(target_p.string(), root_p.string());
        pe.ride(mode, stop_p);
    } catch (const std::exception &e) {
        std::cerr << "ud: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
