/**
 * @file PathElevator.h
 * @brief PathElevator class — traverse a filesystem path hierarchy
 *
 * Given a target path and a root path (an ancestor of the target),
 * PathElevator lists every directory level between them — either
 * ascending (target → root, like cd .. repeatedly) or descending
 * (root → target, like drilling down into a tree).
 *
 * Each level shows the directory's contents, mirroring the output
 * of `ls` at every stop along the way.
 *
 * @author Ryan Chen
 * @date 2024–2026
 */

#ifndef PATH_ELEVATOR_H
#define PATH_ELEVATOR_H

#include <filesystem>
#include <map>
#include <string>
#include <vector>

/**
 * @brief Traverses the directory hierarchy between a root and a target path.
 *
 * Directory levels are numbered from 1 (root) to N (target), analogous to
 * floors in a building.  Two traversal modes are supported:
 *
 *   - ASCEND  : display levels N → N-1 → … → 1  (target up to root)
 *   - DESCEND : display levels 1 → 2  → … → N  (root down to target)
 *
 * The entire path is scanned once on construction and cached; subsequent
 * calls to ride() use the cache rather than re-reading the filesystem.
 */
class PathElevator {
public:
    enum class Mode { ASCEND, DESCEND };

    struct CachedEntry {
        std::string    name;
        bool           is_dir;
        bool           is_file;
        std::uintmax_t size;   /* 0 for non-regular-files */
    };

    /**
     * @brief Construct a PathElevator and pre-scan all levels into the cache.
     *
     * @param target_path  Absolute or relative path to the destination directory.
     * @param root_path    Absolute or relative path to the ancestor directory
     *                     that acts as the topmost level (floor 1).
     * @throws std::invalid_argument if root_path is not an ancestor of target_path,
     *         or if either path does not exist.
     */
    PathElevator(const std::string &target_path, const std::string &root_path);

    /**
     * @brief Traverse and display cached levels between root and target.
     *
     * @param mode     ASCEND or DESCEND.
     * @param stop_at  If non-empty, stop displaying once this level is shown.
     *                 Must be one of the levels between root and target.
     *                 An empty path (default) means display all levels.
     */
    void ride(Mode mode,
              const std::filesystem::path &stop_at = std::filesystem::path{}) const;

    /**
     * @brief ud command — resolve two (or three) segment names and ride.
     *
     * Usage: ud <start> <end> [<stop>]
     *
     *   start / end  — two directory names from the CWD ancestor chain.
     *                  Their relative depths determine direction:
     *                    start shallower than end → DESCEND
     *                    start deeper   than end → ASCEND
     *                  The full range [start, end] is pre-scanned into the cache.
     *
     *   stop         — optional display limit; must lie between start and end.
     *                  Display begins at start and halts once stop is shown.
     *                  Omit to display the entire start→end range.
     *
     * Examples (CWD ancestor chain contains a/b/c/d/e):
     *   ud({"a","d","c"})  → DESCEND, cache a..d, display a b c
     *   ud({"d","a","c"})  → ASCEND,  cache a..d, display d c
     *   ud({"a","d"})      → DESCEND, cache a..d, display a b c d
     */
    static void ud(const std::vector<std::string> &args);

private:
    std::filesystem::path target_;
    std::filesystem::path root_;

    /** Ordered levels root (index 0) … target (last), built once in the ctor. */
    std::vector<std::filesystem::path> levels_;

    /** Pre-scanned directory contents keyed by absolute path. */
    std::map<std::filesystem::path, std::vector<CachedEntry>> cache_;

    /** Build the ordered list of paths from root (index 0) to target (last). */
    std::vector<std::filesystem::path> buildLevels() const;

    /** Pre-scan every level into cache_. Called once from the constructor. */
    void buildCache();

    /** Print the cached contents of @p dir, labelled as floor @p floor of @p total. */
    void displayLevel(int floor, int total, const std::filesystem::path &dir) const;
};

#endif /* PATH_ELEVATOR_H */
