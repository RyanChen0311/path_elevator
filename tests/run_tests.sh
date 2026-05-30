#!/usr/bin/env bash
# Smoke tests for ud

set -euo pipefail

BINARY="$(realpath "${1:-./ud}")"
PASS=0
FAIL=0

check() {
    local name="$1" expected_exit="$2" pattern="$3"
    shift 3
    local actual_exit=0
    local output
    output=$("$@" 2>&1) || actual_exit=$?

    local status_ok=true pattern_ok=true
    [[ "$actual_exit" -eq "$expected_exit" ]] || status_ok=false
    [[ -z "$pattern" ]] || echo "$output" | grep -q "$pattern" || pattern_ok=false

    if $status_ok && $pattern_ok; then
        echo "  PASS  $name"
        PASS=$((PASS + 1))
    else
        echo "  FAIL  $name"
        $status_ok  || echo "        exit: expected $expected_exit, got $actual_exit"
        $pattern_ok || echo "        missing pattern: '$pattern'"
        echo "        output: $output"
        FAIL=$((FAIL + 1))
    fi
}

# Controlled directory tree for traversal tests
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT
mkdir -p "$TMPDIR/alpha/beta/gamma"

echo "Running ud tests..."
echo ""

# ── Argument errors ────────────────────────────────────────────
check "no args: exit 1"           1  "Usage"     "$BINARY"
check "too few args: usage hint"  1  "usage"     "$BINARY" "alpha"
check "unknown name"              1  "not found" "$BINARY" "nonexistent_xyz_abc" "alpha"

# ── Valid traversals (from controlled temp tree) ───────────────
pushd "$TMPDIR/alpha/beta/gamma" > /dev/null

check "same start and end"   1  "different"  "$BINARY" alpha alpha
check "DESCEND alpha→gamma"  0  "Floor"      "$BINARY" alpha gamma
check "ASCEND  gamma→alpha"  0  "Floor"      "$BINARY" gamma alpha
check "stop at middle floor" 0  "Floor"      "$BINARY" alpha gamma beta

popd > /dev/null

echo ""
echo "Results: $PASS passed, $FAIL failed"
[[ "$FAIL" -eq 0 ]]
