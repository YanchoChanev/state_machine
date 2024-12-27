#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "master_handler.h"
#include "types.h"

// Include headers for the functions under test
extern "C" {
    #include "master_handler.h"
    #include "master_comm.h"
    #include "master_state_machine.h"
    #include "logger.h"
    #include "task.h"
    #include "types.h"
}

// ==========================
// Constants Definition
// ==========================
#define TASTK_TIME_MASTER_COMM_HANDLER               10 ///< Time interval for Master Communication Handler.
#define TASTK_TIME_MASTER_STATUS_CHECK_HANDLER       500 ///< Time interval for Master Status Check Handler.

// ==========================
// Mock Classes
// ==========================
// Mock class for Master Communication
class MockMasterComm {
public:
    MOCK_METHOD(RetVal_t, reciveMsgMaster, (void*), ());
    MOCK_METHOD(RetVal_t, sendMsgMaster, (const void*), ());
};

// Mock class for Master State Machine
class MockMasterStateMachine {
public:
    MOCK_METHOD(RetVal_t, stateDispatcher, (SlaveStates), ());
    MOCK_METHOD(RetVal_t, getCurrentState, (MasterStates*), ());
};

// Mock class for Logger
class MockLogger {
public:
    MOCK_METHOD(void, logMessage, (LogLevel level, const char* module, const char* message), ());
};

// Mock class for Task Delays
class MockTask {
public:
    MOCK_METHOD(void, vTaskDelay, (TickType_t), ());
};

// ==========================
// Global Mock Instances
// ==========================
// Global instances to redirect calls from C functions to mocks
MockMasterComm* mockMasterComm;
MockMasterStateMachine* mockMasterStateMachine;
MockLogger* mockLogger;
MockTask* mockTask;

// ==========================
// C-style Function Stubs
// ==========================
// Redirect C function calls to corresponding mock methods
extern "C" {
RetVal_t reciveMsgMaster(void* data) {
    return mockMasterComm->reciveMsgMaster(data);
}

RetVal_t sendMsgMaster(const void* data) {
    return mockMasterComm->sendMsgMaster(data);
}

RetVal_t stateDispatcher(SlaveStates data) {
    return mockMasterStateMachine->stateDispatcher(data);
}

RetVal_t getCurrentState(MasterStates* data) {
    return mockMasterStateMachine->getCurrentState(data);
}

void logMessage(LogLevel level, const char* module, const char* message) {
    mockLogger->logMessage(level, module, message);
}

void vTaskDelay(TickType_t ticks) {
    mockTask->vTaskDelay(ticks);
}
}

// ==========================
// Test Fixture
// ==========================
// Test environment setup and teardown for MasterHandler
class MasterHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockMasterComm = new MockMasterComm();
        mockMasterStateMachine = new MockMasterStateMachine();
        mockLogger = new MockLogger();
        mockTask = new MockTask();
    }

    void TearDown() override {
        delete mockMasterComm;
        delete mockMasterStateMachine;
        delete mockLogger;
        delete mockTask;
    }
};

// ==========================
// Unit Tests for vMasterReciverHandler
// ==========================
// Test case when message receiving fails
TEST_F(MasterHandlerTest, vMasterReciverHandler_ReceiveMessageFails) {
    EXPECT_CALL(*mockMasterComm, reciveMsgMaster(testing::_))
        .WillOnce(testing::Return(RET_ERROR));
    EXPECT_CALL(*mockTask, vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_MASTER_COMM_HANDLER)));

    vMasterReciverHandler(nullptr);
}

// Test case when state dispatcher fails
TEST_F(MasterHandlerTest, vMasterReciverHandler_StateDispatcherFails) {
    EXPECT_CALL(*mockMasterComm, reciveMsgMaster(testing::_))
        .WillOnce(testing::Return(RET_OK));
    EXPECT_CALL(*mockMasterStateMachine, stateDispatcher(testing::_))
        .WillOnce(testing::Return(RET_ERROR));
    EXPECT_CALL(*mockTask, vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_MASTER_COMM_HANDLER)));

    vMasterReciverHandler(nullptr);
}

// Test case when vMasterReciverHandler executes successfully
TEST_F(MasterHandlerTest, vMasterReciverHandler_Success) {
    EXPECT_CALL(*mockMasterComm, reciveMsgMaster(testing::_))
        .WillOnce(testing::Return(RET_OK));
    EXPECT_CALL(*mockMasterStateMachine, stateDispatcher(testing::_))
        .WillOnce(testing::Return(RET_OK));
    EXPECT_CALL(*mockTask, vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_MASTER_COMM_HANDLER)));

    vMasterReciverHandler(nullptr);
}

// ==========================
// Unit Tests for vMasterSenderHandler
// ==========================
// Test case when sending a message fails
TEST_F(MasterHandlerTest, vMasterSenderHandler_SendMessageFails) {
    EXPECT_CALL(*mockMasterStateMachine, getCurrentState(testing::_))
        .WillOnce(testing::Return(RET_OK));
    EXPECT_CALL(*mockMasterComm, sendMsgMaster(testing::_))
        .WillOnce(testing::Return(RET_ERROR));
    EXPECT_CALL(*mockTask, vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_MASTER_STATUS_CHECK_HANDLER)));

    vMasterSenderHandler(nullptr);
}

// Test case when vMasterSenderHandler executes successfully
TEST_F(MasterHandlerTest, vMasterSenderHandler_Success) {
    EXPECT_CALL(*mockMasterStateMachine, getCurrentState(testing::_))
        .WillOnce(testing::Return(RET_OK));
    EXPECT_CALL(*mockMasterComm, sendMsgMaster(testing::_))
        .WillOnce(testing::Return(RET_OK));
    EXPECT_CALL(*mockTask, vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_MASTER_STATUS_CHECK_HANDLER)));

    vMasterSenderHandler(nullptr);
}

// ==========================
// Main Function
// ==========================
// Entry point for the test runner
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
