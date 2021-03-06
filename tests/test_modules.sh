#!/usr/bin/env bash

tdir="tests/modules"

function assert_equ() {
  if [ "$1" != "$2" ]; then
    echo -e "${FUNCNAME[1]}() - Assertion fail: \"$1\" == \"$2\""
    return 1
  fi

  return 0
}


function test_lia() {
  local expects="Abcd"
  local output=$(./lia "$tdir/test_lia.lia" -o- | ases)

  assert_equ "$expects" "$output"
  return
}

function test_var() {
  local expects="abcdef"
  local output=$(./lia "$tdir/test_var.lia" -o- | ases)

  assert_equ "$expects" "$output"
  return
}

function test_expr() {
  local expects="ABCDEFGH"
  local output=$(./lia "$tdir/test_expr.lia" -o- | ases)

  assert_equ "$expects" "$output"
  return
}

function test_io() {
  local expects="2345 -5432"
  local file=$(mktemp)

  ./lia "$tdir/test_io.lia" -o $file
  local output=$(echo -e "2345\n-5432" | $file)

  assert_equ "$expects" "$output"
}


test_lia || exit 1
test_var || exit 2
test_expr || exit 3
test_io || exit 4

echo "Modules OK!"
exit 0