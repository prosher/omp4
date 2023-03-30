#!/usr/bin/env bash

OMP4_SRC_PATH="./omp4"
TEST_IMAGE1_IN="test_data/in.pgm"
TEST_IMAGE1_OUT="test_data/out.pgm"
TEST_IMAGE2_IN="test_data/in8k.pgm"
TEST_IMAGE2_OUT="test_data/out8k.pgm"

LAUNCHES=$1
SLEEP=$2
THREADS=8



function avg() {
    arr=("$@")
    sum=0
    for i in "${arr[@]}"; do
        sum=$(echo "$sum + $i" | bc -l)
    done

    echo "$sum / ${#arr[@]}" | bc -l
}

printf "Testing parallel execution with OMP\n"
function test_range() {
    printf "Test %d: %d test cases, %0.2f seconds interval\n" $1 $LAUNCHES $SLEEP
    printf "input: %s, output: %s\n" $2 $3
    for (( thread_num = 1; thread_num <= THREADS; thread_num++ )); do
        sleep $SLEEP
        printf "%d threads: " $thread_num
        test $thread_num $2 $3
    done
}

function test() {
#    printf "%s\n" "$OMP4_SRC_PATH $1 $2 $3"
    runtimes=()
        for (( launch = 1; launch <= LAUNCHES; launch++ )); do
            result=$($OMP4_SRC_PATH $1 $2 $3)
            result=$(echo $result | cut -d " " -f 7)
            runtimes+=("$result")
            printf "%0.2f ms | " $result
        done
    printf "avg %0.2f ms\n" "$(avg "${runtimes[@]}")"
}

test_range 1 $TEST_IMAGE1_IN $TEST_IMAGE1_OUT
test_range 2 $TEST_IMAGE2_IN $TEST_IMAGE2_OUT

printf "Testing with OMP off\n"
test -1 $TEST_IMAGE1_IN $TEST_IMAGE1_OUT
test -1 $TEST_IMAGE2_IN $TEST_IMAGE2_OUT
printf "Testing with OMP 1 thread\n"
test 1 $TEST_IMAGE1_IN $TEST_IMAGE1_OUT
test 1 $TEST_IMAGE2_IN $TEST_IMAGE2_OUT
