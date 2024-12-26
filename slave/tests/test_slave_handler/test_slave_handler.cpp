#include "gtest/gtest.h"
#include "gmock/gmock.h"

// ==========================
// **Include Dependencies**
// ==========================
extern "C" {
    #include "slave_handler.h"
    #include "logger.h"
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
// Mock class for Queue operations
class MockQueue {
public:
    MOCK_METHOD(BaseType_t, xQueueReceive, (QueueHandle_t queue, void *pvBuffer, TickType_t xTicksToWait), ());
};

// Mock class for Logger operations
class MockLogger {
public:
    MOCK_METHOD(void, logMessage, (LogLevel priority, const char* module, const char* message), ());
};

// Mock class for Slave Communication
class MockSlaveComm {
public:
    MOCK_METHOD(RetVal_t, reciveMsgSlave, (void*), ());
    MOCK_METHOD(RetVal_t, sendMsgSlave, (const void*), ());
};

// Mock class for FreeRTOS functionality
class MockFreeRTOS {
public:
    MOCK_METHOD(void, vTaskDelay, (TickType_t xTicksToDelay), ());
};

// Mock class for State Machine operations
class MockStateMachine {
public:
    MOCK_METHOD(RetVal_t, getState, (SlaveStates*), ());
    MOCK_METHOD(RetVal_t, handelStatus, (SlaveInputStates), ());
};

// Mock class for TCP Communication
class MockTCPComm {
public:
    MOCK_METHOD(void, tcpEchoServerTask, (), ());
};

// Mock class for Task Restart functionality
class MockRestart {
public:
    MOCK_METHOD(RetVal_t, restartAllTasks, (), ());
};

using ::testing::_;
using ::testing::Return;

// ==========================
// **Global Mock Objects**
// ==========================
// Global mock object declarations
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
// Redirect C functions to global mock objects
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
// Provides consistent setup and teardown for SlaveHandler tests
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
// Test that tasks restart when a valid signal is received
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
