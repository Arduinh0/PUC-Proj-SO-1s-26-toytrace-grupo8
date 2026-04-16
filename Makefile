CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11 -g -O0 -Iinclude
LDFLAGS :=

BIN := toytrace

COMMON_SRCS := \
	src/cli.c \
	src/syscall_names.c \
	src/trace_helpers.c \
	src/trace_runtime.c \
	src/student/pairer.c \
	src/student/formatter.c

SOLUTION_COMMON_SRCS := \
	src/cli.c \
	src/syscall_names.c \
	src/trace_helpers.c \
	solutions/trace_runtime.c \
	solutions/student/pairer.c \
	solutions/student/formatter.c

MAIN_SRCS := src/main.c $(COMMON_SRCS)
MAIN_OBJS := $(MAIN_SRCS:.c=.o)

SOLUTION_MAIN_SRCS := src/main.c $(SOLUTION_COMMON_SRCS)
SOLUTION_MAIN_OBJS := $(SOLUTION_MAIN_SRCS:.c=.solution.o)

TARGET_SRCS := $(wildcard tests/targets/*.c)
TARGET_BINS := $(TARGET_SRCS:.c=)

.PHONY: all clean test test-unit test-integration test-solution targets

all: $(BIN) targets

$(BIN): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/unit/test_student: tests/unit/test_student.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.solution.o: %.c
	$(CC) $(CFLAGS) -Isolutions/student -c -o $@ $<

toytrace-solution: $(SOLUTION_MAIN_OBJS)
	$(CC) $(CFLAGS) -Isolutions/student -o $@ $^ $(LDFLAGS)

tests/unit/test_student_solution: tests/unit/test_student.c $(SOLUTION_COMMON_SRCS)
	$(CC) $(CFLAGS) -Isolutions/student -o $@ $^ $(LDFLAGS)

targets: $(TARGET_BINS)

tests/targets/%: tests/targets/%.c
	$(CC) $(CFLAGS) -o $@ $<

test-unit: tests/unit/test_student
	./tests/unit/test_student

test-integration: all
	python3 -m pytest -q tests/test_integration.py

test: test-unit test-integration

test-solution: toytrace-solution tests/unit/test_student_solution targets
	./tests/unit/test_student_solution
	./toytrace-solution trace -- ./tests/targets/hello_write >/tmp/toytrace-solution-trace.txt
	grep 'write(' /tmp/toytrace-solution-trace.txt

clean:
	rm -f $(BIN) toytrace-solution $(MAIN_OBJS) $(SOLUTION_MAIN_OBJS) \
		src/*.o src/student/*.o solutions/*.o solutions/student/*.o \
		tests/unit/test_student tests/unit/test_student_solution $(TARGET_BINS)
