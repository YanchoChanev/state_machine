#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdarg>

// Include dependencies
extern "C" {
    #include "state_mashine_types.h"
    #include "FreeRTOS.h"
    #include "slave_restart_threads.h"
    #include "slave_handler.h"
    #include "slave_state_machine.h"
    #include "logger.h"
    #include "thread_handler_cfg.h"
    #include "state_mashine_types.h"
}

// ==========================
// **Mock Classes for Dependencies**
// ==========================
class MockLogger {
public:
    MOCK_METHOD(void, logMessage, (LogLevel level, const char* tag, const char* message), ());
    MOCK_METHOD(void, logMessageFormattedHelper, (LogLevel level, const char* component, const char* format), ());
    void logMessageFormatted(LogLevel level, const char* component, const char* format, ...) {
        va_list args;
        va_start(args, format);
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        logMessageFormattedHelper(level, component, buffer);
    }
};

class MockFreeRTOS {
public:
    MOCK_METHOD(void, vTaskDelete, (TaskHandle_t xTaskToDelete), ());
    MOCK_METHOD(BaseType_t, xTaskCreate, (TaskFunction_t pxTaskCode, const char* const pcName, uint32_t usStackDepth,
                                         void* const pvParameters, UBaseType_t uxPriority, TaskHandle_t* const pxCreatedTask), ());
};

class MockStateMachine {
public:
    MOCK_METHOD(RetVal_t, handelStatus, (SlaveInputStates state), ());
};

using ::testing::_;
using ::testing::Return;

// ==========================
// **Global Mock Objects**
// ==========================
MockLogger* mockLogger;
MockFreeRTOS* mockFreeRTOS;
MockStateMachine* mockStateMachine;
static TaskHandler taskHandlers_[SLAVE_TAKS_HANDLERS_SIZE] = {
    {SLAVE_STATUS_OBSERVATION_HANDLER_ID, vSlaveStatusObservationHandler, "SlaveStatusObservationHandler", 
    TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING, NULL},
    {TCP_ECHO_SERVER_TASK, vTCPEchoServerTask, "TCPEchoServerTask", TASTK_PRIO_ECHO_SERVER_HANDLER, NULL},
};

// ==========================
// **Mocked C Functions**
// ==========================
extern "C" {
    void logMessage(LogLevel priority, const char* module, const char* message) {
        mockLogger->logMessage(priority, module, message);
    }

    void logMessageFormatted(LogLevel level, const char* component, const char* format, ...) {
        va_list args;
        va_start(args, format);
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        mockLogger->logMessageFormattedHelper(level, component, buffer);
    }

    void vTaskDelete(TaskHandle_t xTaskToDelete) {
        mockFreeRTOS->vTaskDelete(xTaskToDelete);
    }

    BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* const pcName, uint32_t usStackDepth,
                           void* const pvParameters, UBaseType_t uxPriority, TaskHandle_t* const pxCreatedTask) {
        return mockFreeRTOS->xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
    }

    RetVal_t handelStatus(SlaveInputStates state) {
        return mockStateMachine->handelStatus(state);
    }

    // Mock implementations of undefined functions
    void vSlaveStatusObservationHandler(void* params) {
        // Mock implementation
    }

    void vTCPEchoServerTask(void* params) {
        // Mock implementation
    }
}

// ==========================
// **Test Fixture**
// ==========================
class SlaveRestartThreadsTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockLogger = new MockLogger();
        mockFreeRTOS = new MockFreeRTOS();
        mockStateMachine = new MockStateMachine();
    }

    void TearDown() override {
        delete mockLogger;
        delete mockFreeRTOS;
        delete mockStateMachine;
    }
};

// ==========================
// **Tests for restartAllTasks**
// ==========================

// Test successful restart of all tasks
TEST_F(SlaveRestartThreadsTest, RestartAllTasks_Success) {
    EXPECT_CALL(*mockLogger, logMessage(LOG_LEVEL_INFO, _, _)).Times(1);
    EXPECT_CALL(*mockFreeRTOS, xTaskCreate(_, _, _, _, _, _)).WillRepeatedly(Return(pdPASS));
    EXPECT_CALL(*mockStateMachine, handelStatus(SLAVE_INPUT_STATE_IDEL_OR_SLEEP)).WillOnce(Return(RET_OK));

    RetVal_t result = restartAllTasks();

    EXPECT_EQ(result, RET_OK);
}

// Test failure in recreating tasks during restart
TEST_F(SlaveRestartThreadsTest, RestartAllTasks_RecreateFailure) {
    EXPECT_CALL(*mockLogger, logMessage(LOG_LEVEL_INFO, _, _)).Times(1);
    EXPECT_CALL(*mockFreeRTOS, xTaskCreate(_, _, _, _, _, _)).WillOnce(Return(pdFAIL));
    EXPECT_CALL(*mockLogger, logMessage(LOG_LEVEL_ERROR, _, _)).Times(1);

    RetVal_t result = restartAllTasks();

    EXPECT_EQ(result, RET_ERROR);
}

// Test failure in setting state during restart
TEST_F(SlaveRestartThreadsTest, RestartAllTasks_SetStateFailure) {
    EXPECT_CALL(*mockLogger, logMessage(LOG_LEVEL_INFO, _, _)).Times(1);
    EXPECT_CALL(*mockFreeRTOS, xTaskCreate(_, _, _, _, _, _)).WillRepeatedly(Return(pdPASS));
    EXPECT_CALL(*mockStateMachine, handelStatus(SLAVE_INPUT_STATE_IDEL_OR_SLEEP)).WillOnce(Return(RET_ERROR));
    EXPECT_CALL(*mockLogger, logMessage(LOG_LEVEL_ERROR, _, _)).Times(1);

    RetVal_t result = restartAllTasks();

    EXPECT_EQ(result, RET_ERROR);
}

// ==========================
// **Main Test Runner**
// ==========================
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
