#! /usr/bin/env bats

PROG_NAME="$(pwd)/align"

setup () {
  TEST_FILE=$(mktemp)
}

@test "test single-line input" {
  OUTPUT="test"
  echo -n "$OUTPUT" > $TEST_FILE
  run $PROG_NAME < $TEST_FILE
  [ "$status" -eq 0 ]
  [ "$output" == "$OUTPUT" ]
}

@test "test empty input" {
  echo -n "" > $TEST_FILE
  run $PROG_NAME < $TEST_FILE
  [ "$status" -eq 0 ]
  [ "$output" == "" ]
}

@test "test width small oneline" {
  echo -n "apple\n" > $TEST_FILE
  run $PROG_NAME --width 2 < $TEST_FILE
  [ "$status" -eq 0 ]
  [ "$output" == "ap\n" ]
}
