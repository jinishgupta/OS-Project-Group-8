CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -I./include
LDFLAGS =

SRC_DIR = src
BIN_DIR = bin

# All utility source files linked into the shell (must use SHELL_BUILD guard)
UTIL_SRCS = \
    $(SRC_DIR)/custom_ls.c \
    $(SRC_DIR)/custom_grep.c \
    $(SRC_DIR)/custom_cat.c \
	$(SRC_DIR)/custom_cp.c \
	$(SRC_DIR)/custom_rm.c

UTIL_OBJS_SHELL = $(UTIL_SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/shell_%.o)

STANDALONES = \
    $(BIN_DIR)/custom_ls \
    $(BIN_DIR)/custom_grep \
    $(BIN_DIR)/custom_cat \
    $(BIN_DIR)/custom_wc \
    $(BIN_DIR)/custom_cp \
    $(BIN_DIR)/custom_rm

.PHONY: all clean shell standalones run

all: shell standalones

# Create bin directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build the interactive shell
# SHELL_BUILD suppresses each util's own main()
$(BIN_DIR)/shell_%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -DSHELL_BUILD -c $< -o $@

shell: $(BIN_DIR)/myshell
$(BIN_DIR)/myshell: $(SRC_DIR)/shell.c $(UTIL_OBJS_SHELL) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Built: $@"

# Build standalone utilities
standalones: $(STANDALONES)

$(BIN_DIR)/custom_ls: $(SRC_DIR)/custom_ls.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Built: $@"

$(BIN_DIR)/custom_grep: $(SRC_DIR)/custom_grep.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Built: $@"

$(BIN_DIR)/custom_cat: $(SRC_DIR)/custom_cat.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Built: $@"


$(BIN_DIR)/custom_wc: $(SRC_DIR)/custom_wc.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Built: $@"

$(BIN_DIR)/custom_cp: $(SRC_DIR)/custom_cp.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Built: $@"

$(BIN_DIR)/custom_rm: $(SRC_DIR)/custom_rm.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Built: $@"






# Clean
clean:
	rm -rf $(BIN_DIR)
	@echo "Cleaned."

# Run the shell
run: shell
	./$(BIN_DIR)/myshell
