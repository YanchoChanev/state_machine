#include "gtest/gtest.h"
#include "gmock/gmock.h"

// ==========================
// **Include Dependencies**
// ==========================
extern "C" {
    #include "slave_handler.h"  // Include the header file for the module under test
    #include "logger.h"        // Include necessary header files for dependencies
    #include "queue.h"
    #include "slave_comm.h"
    #include "slave_state_machine.h"
    #include "slave_restart_threads.h"
    #include "slave_TCP_comm.h"
    #include "FreeRTOS.h"
    #include "task.h"
    #include "types.h"
    #include "state_mashine_types.h"
}

// ==========================
// **Mock Classes for Dependencies**
// ==========================
// Use clear and concise names for mock classes
class MockQueue {
public:
    MOCK_METHOD(BaseType_t, xQueueReceive, (QueueHandle_t queue, void *pvBuffer, TickType_t xTicksToWait), ());
};

class MockLogger {
public:
    MOCK_METHOD(void, logMessage, (LogLevel priority, const char* module, const char* message), ());
};

class MockSlaveComm {
public:
    MOCK_METHOD(RetVal_t, reciveMsgSlave, (void*), ());
    MOCK_METHOD(RetVal_t, sendMsgSlave, (const void*), ());
};

class MockFreeRTOS {
public:
    MOCK_METHOD(void, vTaskDelay, (TickType_t xTicksToDelay), ());
};

class MockStateMachine {
public:
    MOCK_METHOD(RetVal_t, getState, (SlaveStates*), ());
    MOCK_METHOD(RetVal_t, handelStatus, (SlaveInputStates), ());
};

class MockTCPComm {
public:
    MOCK_METHOD(void, tcpEchoServerTask, (), ());
};

class MockRestart {
public:
    MOCK_METHOD(RetVal_t, restartAllTasks, (), ());
};

using ::testing::_;
using ::testing::Return;

// ==========================
// **Global Mock Objects**
// ==========================
// Create global mock objects for easy access in tests
MockQueue* mockQueue;
MockLogger* mockLogger;
MockSlaveComm* mockSlaveComm;
MockStateMachine* mockStateMachine;
MockTCPComm* mockTCPComm;
MockFreeRTOS* mockFreeRTOS;
MockRestart* mockRestart;

// ==========================
// **Mocked C Functions**
// ==========================
// Redirect C functions to their corresponding mock objects
extern "C" {
    void logMessage(LogLevel priority, const char* thread, const char* message) {
        mockLogger->logMessage(priority, thread, message);
    }

    void vTaskDelay(TickType_t xTicksToDelay) {
        mockFreeRTOS->vTaskDelay(xTicksToDelay);
    }

    RetVal_t restartAllTasks() {
        return mockRestart->restartAllTasks();
    }

    BaseType_t xQueueReceive(QueueHandle_t queue, void *pvBuffer, TickType_t xTicksToWait) {
        return mockQueue->xQueueReceive(queue, pvBuffer, xTicksToWait);
    }

    RetVal_t reciveMsgSlave(void* data) {
        return mockSlaveComm->reciveMsgSlave(data);
    }

    RetVal_t getState(SlaveStates* state) {
        return mockStateMachine->getState(state);
    }

    RetVal_t sendMsgSlave(const void* state) {
        return mockSlaveComm->sendMsgSlave(state);
    }

    RetVal_t handelStatus(SlaveInputStates state) {
        return mockStateMachine->handelStatus(state);
    }

    void tcpEchoServerTask() {
        mockTCPComm->tcpEchoServerTask();
    }
}

// ==========================
// **Test Fixture**
// ==========================
// Create a test fixture for consistent setup and teardown
class SlaveHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockQueue = new MockQueue();
        mockLogger = new MockLogger();
        mockSlaveComm = new MockSlaveComm();
        mockStateMachine = new MockStateMachine();
        mockTCPComm = new MockTCPComm();
        mockFreeRTOS = new MockFreeRTOS();
        mockRestart = new MockRestart();
    }

    void TearDown() override {
        delete mockQueue;
        delete mockLogger;
        delete mockSlaveComm;
        delete mockStateMachine;
        delete mockTCPComm;
        delete mockFreeRTOS;
        delete mockRestart;
    }
};

// ==========================
// **1. Restart Handler Tests**
// ==========================
// Test successful restart of tasks
TEST_F(SlaveHandlerTest, RestartHandler_ReceivesSignalAndRestartsTasks) {
    uint8_t signal = 1;
    QueueHandle_t testQueue = (QueueHandle_t)1;

    EXPECT_CALL(*mockLogger, logMessage(LOG_LEVEL_DEBUG, _, _)).Times(1);
    EXPECT_CALL(*mockQueue, xQueueReceive(testQueue, _, _))
        .WillOnce([](QueueHandle_t, void* pvBuffer, TickType_t) {
            *(uint8_t*)pvBuffer = 1;
            return pdPASS;
        });
    EXPECT_CALL(*mockFreeRTOS, vTaskDelay(_)).Times(2);
    EXPECT_CALL(*mockRestart, restartAllTasks()).WillOnce(Return(RET_OK));

    vRestartHandler((void*)testQueue);
}

// **2. Status Observation Handler Tests**
// Test handling valid slave state
TEST_F(SlaveHandlerTest, SlaveStatusObservationHandler_ReceivesStateAndHandlesStatus) {
    EXPECT_CALL(*mockSlaveComm, reciveMsgSlave(_)).WillOnce(Return(RET_OK));
    EXPECT_CALL(*mockStateMachine, getState(_))
        .WillOnce([](SlaveStates* state) {
            *state = SLAVE_STATE_ACTIVE;
            return RET_OK;
        });
    EXPECT_CALL(*mockSlaveComm, sendMsgSlave(_)).WillOnce(Return(RET_OK));

    vSlaveStatusObservationHandler(nullptr);
}

// **3. TCP Echo Server Task Tests**
// Ensure TCP server task starts
TEST_F(SlaveHandlerTest, TCPEchoServerTask_LogsStartAndCallsTCPServer) {
    EXPECT_CALL(*mockTCPComm, tcpEchoServerTask()).Times(1);
    vTCPEchoServerTask(nullptr);
}

// ==========================
// **Main Test Runner**
// ==========================
// Entry point for the test runner
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}