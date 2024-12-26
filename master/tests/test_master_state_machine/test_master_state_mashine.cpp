#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdarg> // Include for va_list, va_start, and va_end
#include "master_state_machine.h"
#include "types.h"

// ==========================
// **Include Dependencies**
// ==========================
extern "C" {
    #include "queue.h"
    #include "semphr.h"
    #include "logger.h"
}

// ==========================
// **Constants Definition**
// ==========================
#define SEMAPHOR_TICKS 10 ///< Timeout value for semaphore operations in ticks.

extern MasterStatesConditionHandler masterStateMachineCondition;

// ==========================
// **Handle Macro Conflicts**
// ==========================
#ifdef xSemaphoreTake
    #undef xSemaphoreTake
#endif

#ifdef xSemaphoreGive
    #undef xSemaphoreGive
#endif

#ifdef xSemaphoreCreateBinary
    #undef xSemaphoreCreateBinary
#endif

// ==========================
// **Mock Classes for Dependencies**
// ==========================
// Mock class for Semaphore operations
class MockSemaphore {
public:
    MOCK_METHOD(BaseType_t, xSemaphoreTake, (SemaphoreHandle_t, TickType_t), ());
    MOCK_METHOD(BaseType_t, xSemaphoreGive, (SemaphoreHandle_t), ());
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateBinary, (), ());
};

// Mock class for Logger operations
class MockLogger {
public:
    MOCK_METHOD(void, logMessage, (LogLevel, const char*, const char*), ());
    MOCK_METHOD(void, logMessageFormattedHelper, (LogLevel, const char*, const char*), ());
    
    void logMessageFormatted(LogLevel level, const char* component, const char* format, ...) {
        va_list args;
        va_start(args, format);
        logMessageFormattedHelper(level, component, format);
        va_end(args);
    }
};

// Mock class for Master Communication operations
class MockMasterComm {
public:
    MOCK_METHOD(RetVal_t, reciveMsgMaster, (void*), ());
    MOCK_METHOD(RetVal_t, sendMsgMaster, (const void*), ());
};

// ==========================
// **Global Mock Objects**
// ==========================
MockSemaphore* mockSemaphore;
MockLogger* mockLogger;
MockMasterComm* mockMasterComm;

// ==========================
// **Fake Implementations for C Functions**
// ==========================
extern "C" {
    BaseType_t xSemaphoreTake(SemaphoreHandle_t semaphore, TickType_t ticks) {
        return mockSemaphore->xSemaphoreTake(semaphore, ticks);
    }

    BaseType_t xSemaphoreGive(SemaphoreHandle_t semaphore) {
        return mockSemaphore->xSemaphoreGive(semaphore);
    }

    SemaphoreHandle_t xSemaphoreCreateBinary() {
        return mockSemaphore->xSemaphoreCreateBinary();
    }

    void logMessage(LogLevel level, const char* module, const char* message) {
        mockLogger->logMessage(level, module, message);
    }

    void logMessageFormatted(LogLevel level, const char* component, const char* format, ...) {
        mockLogger->logMessageFormatted(level, component, format);
    }

    RetVal_t reciveMsgMaster(void* data) {
        return mockMasterComm->reciveMsgMaster(data);
    }

    RetVal_t sendMsgMaster(const void* data) {
        return mockMasterComm->sendMsgMaster(data);
    }

    BaseType_t xQueueSemaphoreTake(QueueHandle_t xQueue, TickType_t xTicksToWait) {
        return mockSemaphore->xSemaphoreTake((SemaphoreHandle_t)xQueue, xTicksToWait);
    }

    BaseType_t xQueueGenericSend(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait, BaseType_t xCopyPosition) {
        return pdTRUE; // Always return success in the test environment
    }

    QueueHandle_t xQueueGenericCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize, uint8_t ucQueueType) {
        return (QueueHandle_t)mockSemaphore->xSemaphoreCreateBinary();
    }
}

// ==========================
// **Test Fixture**
// ==========================
class MasterStateMachineTest : public ::testing::Test {
protected:
    SemaphoreHandle_t semaphore = nullptr;
    void SetUp() override {
        mockSemaphore = new MockSemaphore();
        mockLogger = new MockLogger();
        mockMasterComm = new MockMasterComm();

        semaphore = reinterpret_cast<SemaphoreHandle_t>(0x1234);
        masterStateMachineCondition.stateSemaphore = reinterpret_cast<SemaphoreHandle_t>(0x1234);
        masterStateMachineCondition.currentState = MASTESR_STATE_IDLE;
    }

    void TearDown() override {
        delete mockSemaphore;
        delete mockLogger;
        delete mockMasterComm;
    }
};

// ==========================
// **1. State Dispatcher Tests**
// ==========================
// Test transition to Idle State
TEST_F(MasterStateMachineTest, StateDispatcher_TransitionToIdleState) {

    MasterStates state;
    EXPECT_EQ(getCurrentState(&state), RET_OK);
    EXPECT_EQ(state, MASTESR_STATE_IDLE);
}

// Test transition to Processing State
TEST_F(MasterStateMachineTest, StateDispatcher_TransitionToProcessingState) {
    EXPECT_CALL(*mockSemaphore, xSemaphoreTake(::testing::_, SEMAPHOR_TICKS))
        .WillOnce(::testing::Return(pdTRUE));
    EXPECT_EQ(stateDispatcher(SLAVE_STATE_ACTIVE), RET_OK);

    MasterStates state;
    EXPECT_EQ(getCurrentState(&state), RET_OK);
    EXPECT_EQ(state, MASTESR_STATE_PROCESSING);
}

// Test transition to Error State
TEST_F(MasterStateMachineTest, StateDispatcher_TransitionToErrorState) {
    EXPECT_CALL(*mockSemaphore, xSemaphoreTake(::testing::_, SEMAPHOR_TICKS))
        .WillOnce(::testing::Return(pdTRUE));
    EXPECT_EQ(stateDispatcher(SLAVE_STATE_FAULT), RET_OK);

    MasterStates state;
    EXPECT_EQ(getCurrentState(&state), RET_OK);
    EXPECT_EQ(state, MASTESR_STATE_ERROR);
}

// Test handling of Invalid State
TEST_F(MasterStateMachineTest, StateDispatcher_InvalidState) {
    EXPECT_EQ(stateDispatcher(SLAVE_STATE_MAX), RET_ERROR);
}

// ==========================
// **2. Semaphore Initialization Tests**
// ==========================
// Test successful semaphore initialization
TEST_F(MasterStateMachineTest, InitStateSemaphoreMaster_Success) {
    EXPECT_CALL(*mockSemaphore, xSemaphoreCreateBinary())
        .WillOnce(::testing::Return(reinterpret_cast<SemaphoreHandle_t>(0x1234)));
    EXPECT_EQ(initStateSemaphoreMaster(), RET_OK);
}

// Test semaphore initialization failure
TEST_F(MasterStateMachineTest, InitStateSemaphoreMaster_Failure) {
    EXPECT_CALL(*mockSemaphore, xSemaphoreCreateBinary())
        .WillOnce(::testing::Return(nullptr));
    EXPECT_EQ(initStateSemaphoreMaster(), RET_ERROR);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
