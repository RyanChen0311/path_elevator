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

    /** @brief A single filesystem entry cached during construction. */
    struct CachedEntry {
        std::string    name;     ///< Filename component only (not a full path).
        bool           is_dir;   ///< True if the entry is a directory.
        bool           is_file;  ///< True if the entry is a regular file.
        std::uintmax_t size;     ///< File size in bytes; 0 for non-regular files.
    };

    /**
     * @brief Construct a PathElevator and pre-scan all levels into the cache.
     *
     * @param target_path  Absolute or relative path to the destination directory.
     * @param root_path    Absolute or relative path to the ancestor directory
     *                     that acts as the topmost level (floor 1).
     * @throws std::invalid_argument if either path does not exist or is not a
     *         directory, or if @p root_path is not an ancestor of @p target_path.
     */
    PathElevator(const std::string &target_path, const std::string &root_path);

    /**
     * @brief Traverse and display cached levels between root and target.
     *
     * Outputs one block per floor to stdout using ANSI colour codes.
     * On Windows, virtual terminal processing is enabled automatically.
     *
     * @param mode     ASCEND (target → root) or DESCEND (root → target).
     * @param stop_at  If non-empty, stop displaying once this level is shown.
     *                 Must be one of the canonical paths between root and target.
     *                 An empty path (default) means display all levels.
     */
    void ride(Mode mode,
              const std::filesystem::path &stop_at = std::filesystem::path{}) const;

    /**
     * @brief Entry point for the @c ud command-line tool.
     *
     * Resolves two (or three) directory segment names against the CWD ancestor
     * chain, constructs a PathElevator, and calls ride().
     *
     * Usage: ud \<start\> \<end\> [\<stop\>]
     *
     *   start / end  — directory names present in the CWD ancestor chain.
     *                  Their relative depths determine direction:
     *                    start shallower than end → DESCEND
     *                    start deeper   than end  → ASCEND
     *                  When the same name appears more than once in the chain
     *                  (e.g. /a/b/a), the shallowest (closest to root) match
     *                  is used.
     *
     *   stop         — optional display limit; must lie between start and end.
     *
     * @param args  Command-line arguments after the program name (argv[1..]).
     * @return 0 on success, 1 on any error (bad arguments, unresolvable paths).
     */
    static int ud(const std::vector<std::string> &args);

private:
    std::filesystem::path target_;
    std::filesystem::path root_;

    /** Ordered levels: root at index 0, target at the last index. */
    std::vector<std::filesystem::path> levels_;

    /** Pre-scanned directory contents, keyed by canonical absolute path. */
    std::map<std::filesystem::path, std::vector<CachedEntry>> cache_;

    /**
     * @brief Build the ordered list of paths from root (index 0) to target (last).
     *
     * Walks upward from @c target_ to @c root_, collecting each canonical path,
     * then reverses the result so index 0 is the shallowest directory.
     *
     * @return Vector of canonical paths ordered root → target.
     */
    [[nodiscard]] std::vector<std::filesystem::path> buildLevels() const;

    /**
     * @brief Pre-scan every level in @c levels_ into @c cache_.
     *
     * Calls @c fs::directory_iterator on each level using the error_code
     * overloads to avoid exceptions.  Unreadable directories are cached as
     * empty with a warning printed to stderr.  Called exactly once from the
     * constructor.
     */
    void buildCache();

    /**
     * @brief Print the cached contents of one directory level to stdout.
     *
     * Output format:
     * @code
     * [Floor N/Total]  /absolute/path
     * ------------------------------------------------------------
     *   [DIR ] subdir/
     *   [FILE] file.txt
     *   [OTHER] symlink
     * @endcode
     *
     * @param floor  1-based floor number shown in the label.
     * @param total  Total number of floors (for the "N/total" label).
     * @param dir    Canonical path of the directory to display.
     */
    void displayLevel(int floor, int total, const std::filesystem::path &dir) const;
};

#endif /* PATH_ELEVATOR_H */
