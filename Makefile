# https://freertos.org/Creating-a-new-FreeRTOS-project.html
# Based on /opt/optsync/FreeRTOS/FreeRTOS/Demo/Posix_GCC/Makefile

CC := gcc
BIN := modelo-posix-gcc

BUILD_DIR := build

FREERTOS_DIR_REL := ${FREERTOS_PATH}/FreeRTOS
FREERTOS_DIR := $(abspath $(FREERTOS_DIR_REL))

FREERTOS_PLUS_DIR_REL := ${FREERTOS_PATH}/FreeRTOS-Plus
FREERTOS_PLUS_DIR := $(abspath $(FREERTOS_PLUS_DIR_REL))

INCLUDE_DIRS := -I./include
INCLUDE_DIRS += -I${FREERTOS_DIR}/Source/include
INCLUDE_DIRS += -I${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix
INCLUDE_DIRS += -I./master/include
INCLUDE_DIRS += -I./slave/include
INCLUDE_DIRS += -I./types
INCLUDE_DIRS += -I./config
INCLUDE_DIRS += -I./logger/include

SOURCE_FILES := ./main.c
SOURCE_FILES += $(wildcard ./master/src/*.c)
SOURCE_FILES += $(wildcard ./slave/src/*.c)
SOURCE_FILES += ${FREERTOS_DIR}/Source/tasks.c
SOURCE_FILES += ${FREERTOS_DIR}/Source/queue.c
SOURCE_FILES += ${FREERTOS_DIR}/Source/list.c
SOURCE_FILES += ${FREERTOS_DIR}/Source/timers.c
# Memory manager (use malloc() / free())
SOURCE_FILES += ${FREERTOS_DIR}/Source/portable/MemMang/heap_3.c
# Posix port
SOURCE_FILES += ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/port.c
SOURCE_FILES += ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
SOURCE_FILES += ./logger/src/logger.c

CFLAGS := -ggdb3 -O0
LDFLAGS := -ggdb3 -O0 -pthread

OBJ_FILES = $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)

DEP_FILE = $(OBJ_FILES:%.o=%.d)

${BIN} : $(BUILD_DIR)/$(BIN)

${BUILD_DIR}/${BIN} : ${OBJ_FILES}
	-mkdir -p ${@D}
	$(CC) $^ $(CFLAGS) $(INCLUDE_DIRS) ${LDFLAGS} -o $@

-include ${DEP_FILE}

${BUILD_DIR}/%.o : %.c
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) ${INCLUDE_DIRS} -MMD -c $< -o $@

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)

# Setup command
.PHONY: setup
setup:
	@echo "Running FreeRTOS setup script..."
	./freertos_setup.sh

# Run the binary
.PHONY: run
run: ${BUILD_DIR}/${BIN}
	@echo "Running ${BIN}..."
	./${BUILD_DIR}/${BIN}

# Test perform command
.PHONY: run_master_comm_test
run_master_comm_test:
	@echo "Running master communication test..."
	./test_scripts/run_master_comm_test.sh

.PHONY: run_master_handler_test
run_master_handler_test:
	@echo "Running master handler test..."
	./test_scripts/run_master_handler_test.sh

.PHONY: run_master_state_mashine_test
run_master_state_mashine_test:
	@echo "Running master state machine test..."
	./test_scripts/run_master_state_mashine_test.sh

.PHONY: run_slave_comm_test
run_slave_comm_test:
	@echo "Running slave communication test..."
	./test_scripts/run_slave_comm_test.sh

.PHONY: run_slave_handler_test
run_slave_handler_test:
	@echo "Running slave handler test..."
	./test_scripts/run_slave_handler_test.sh

.PHONY: run_slave_restart_threads_test
run_slave_restart_threads_test:
	@echo "Running slave restart threads test..."
	./test_scripts/run_slave_restart_threads_test.sh

.PHONY: run_slave_state_machine_test
run_slave_state_machine_test:
	@echo "Running slave state machine test..."
	./test_scripts/run_slave_state_machine_test.sh
