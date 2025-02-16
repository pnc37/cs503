#!/usr/bin/env bats

# File: student_tests.sh
#
# Create your unit tests suite in this file

# Test for the ls command running without errors
@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF
    # Assertions
    [ "$status" -eq 0 ]
}

# Test for cd with no arguments (should do nothing)
@test "cd with no arguments does nothing" {
    original_dir=$(pwd)
    run ./dsh cd
    [ "$status" -eq 0 ]
    [ "$(pwd)" = "$original_dir" ]
}

# Test for cd with valid directory (e.g., /tmp)
@test "cd with valid directory" {
    run ./dsh cd /tmp
    [ "$status" -eq 0 ]
    [ "$(pwd)" = "/tmp" ]
}

# Test for cd with invalid directory (should return an error)
@test "cd with invalid directory returns error" {
    run ./dsh cd /nonexistent_directory
    [ "$status" -ne 0 ]
}

# Test for exit command (should exit the shell)
@test "exit command should terminate the shell" {
    run ./dsh exit
    [ "$status" -eq 0 ]
}

# Test for external command execution (e.g., ls)
@test "ls runs without errors" {
    run ./dsh ls
    [ "$status" -eq 0 ]
}

# Test for the rc command (should return the last return code)
@test "rc should return the last return code" {
    run ./dsh ls
    [ "$status" -eq 0 ]
    run ./dsh rc
    [ "$output" -eq 0 ]
}
