# Bash completion for ud
#
# Usage (no system install needed):
#   source /absolute/path/to/ud-completion.bash
#
# To also cover a specific binary at an absolute path:
#   ud_register /absolute/path/to/ud

_ud()
{
    local cur="${COMP_WORDS[COMP_CWORD]}"

    # Walk up from CWD and collect every ancestor directory name.
    local dir names
    dir="$PWD"
    names=()
    while true; do
        local base
        base="$(basename -- "$dir")"
        [[ -n "$base" ]] && names+=("$base")
        local parent
        parent="$(dirname -- "$dir")"
        [[ "$parent" == "$dir" ]] && break   # reached filesystem root
        dir="$parent"
    done

    COMPREPLY=($(compgen -W "${names[*]}" -- "$cur"))
}

# Wrap the binary in a shell function so completion works without PATH install.
# Usage: ud_use /absolute/path/to/ud
#
# Example:
#   ud_use /mnt/d/github/path_elevator/ud
#   ud path_elevator mnt          # Tab completion works on "ud"
ud_use() {
    local bin="$1"
    eval "ud() { \"$bin\" \"\$@\"; }"
    complete -F _ud ud
}

# If the binary is in CWD, call ud_use automatically.
if [[ -x "./ud" ]]; then
    ud_use "$PWD/ud"
fi
