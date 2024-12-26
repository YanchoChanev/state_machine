#include "gtest/gtest.h"
#include "gmock/gmock.h"

// Include the header file under test
extern "C" {
    #include "slave_state_machine.h"
    #include "semphr.h"
    #include "logger.h"
    #include "types.h"
}

// Undefine FreeRTOS Macros for Mocking (more concise)
#undef xSemaphoreTake
#undef xSemaphoreGive
#undef xQueueSend
#undef xQueueGenericSend
#undef xQueueSemaphoreTake
#undef xQueueCreateMutex

// Mock Dependencies (with more descriptive names)
class MockSemaphore {
public:
    MOCK_METHOD(BaseType_t, xSemaphoreTake, (SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait), ());
    MOCK_METHOD(BaseType_t, xSemaphoreGive, (SemaphoreHandle_t xSemaphore), ());
    MOCK_METHOD(QueueHandle_t, xQueueCreateMutex, (uint8_t ucQueueType), ());
};

class MockQueue {
public:
    MOCK_METHOD(BaseType_t, xQueueSend, (QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait), ());
    MOCK_METHOD(BaseType_t, xQueueGenericSend, (QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait, BaseType_t xCopyPend), ());
    MOCK_METHOD(BaseType_t, xQueueSemaphoreTake, (QueueHandle_t xQueue, TickType_t xTicksToWait), ());
};

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

// Global Mock Instances
MockSemaphore* mockSemaphore;
MockQueue* mockQueue;
MockLogger* mockLogger;

// Replace FreeRTOS functions with mock implementations
extern "C" {
    BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait) {
        return mockSemaphore->xSemaphoreTake(xSemaphore, xTicksToWait);
    }

    BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore) {
        return mockSemaphore->xSemaphoreGive(xSemaphore);
    }

    QueueHandle_t xQueueCreateMutex(uint8_t ucQueueType) {
        return mockSemaphore->xQueueCreateMutex(ucQueueType);
    }

    BaseType_t xQueueSend(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait) {
        return mockQueue->xQueueSend(xQueue, pvItemToQueue, xTicksToWait);
    }

    BaseType_t xQueueGenericSend(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait, BaseType_t xCopyPend) {
        return mockQueue->xQueueGenericSend(xQueue, pvItemToQueue, xTicksToWait, xCopyPend);
    }

    BaseType_t xQueueSemaphoreTake(QueueHandle_t xQueue, TickType_t xTicksToWait) {
        return mockQueue->xQueueSemaphoreTake(xQueue, xTicksToWait);
    }

    void logMessage(LogLevel level, const char* tag, const char* message) {
        mockLogger->logMessage(level, tag, message);
    }

    void logMessageFormatted(LogLevel level, const char* component, const char* format, ...) {
        va_list args;
        va_start(args, format);
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        mockLogger->logMessageFormattedHelper(level, component, buffer);
    }
}

// Test Fixture
class SlaveStateMachineTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockSemaphore = new MockSemaphore();
        mockQueue = new MockQueue();
        mockLogger = new MockLogger();
    }

    void TearDown() override {
        delete mockSemaphore;
        delete mockQueue;
        delete mockLogger;
    }
};

// TEST CASES

// Test initStateMachineSlave
TEST_F(SlaveStateMachineTest, InitStateMachineSlave_Success) {
    EXPECT_CALL(*mockLogger, logMessage(::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*mockSemaphore, xQueueCreateMutex(::testing::_)).WillOnce(::testing::Return((QueueHandle_t)1));

    QueueHandle_t dummyQueue = (QueueHandle_t)1;

    RetVal_t result = initStateMachineSlave(dummyQueue);

    EXPECT_EQ(result, RET_OK);
}

// Test handelStatus with valid state
TEST_F(SlaveStateMachineTest, HandelStatus_ValidState) {
    EXPECT_CALL(*mockQueue, xQueueSemaphoreTake(::testing::_, ::testing::_)).WillOnce(::testing::Return(pdTRUE));
    RetVal_t result = handelStatus(SLAVE_INPUT_STATE_RPOCES_OR_ACTIVE);

    EXPECT_EQ(result, RET_OK);
}

// Test handelStatus with invalid state
TEST_F(SlaveStateMachineTest, HandelStatus_InvalidState) {
    EXPECT_CALL(*mockLogger, logMessage(::testing::_, ::testing::_, ::testing::_)).Times(1);

    RetVal_t result = handelStatus(SLAVE_INPUT_STATE_MAX);

    EXPECT_EQ(result, RET_ERROR);
}

// Test getState with valid pointer
TEST_F(SlaveStateMachineTest, GetState_ValidPointer) {
    SlaveStates currentState;
    EXPECT_CALL(*mockLogger, logMessage(::testing::_, ::testing::_, ::testing::_)).Times(0);

    RetVal_t result = getState(&currentState);

    EXPECT_EQ(result, RET_OK);
}

// Test getState with null pointer
TEST_F(SlaveStateMachineTest, GetState_NullPointer) {
    EXPECT_CALL(*mockLogger, logMessage(::testing::_, ::testing::_, ::testing::_)).Times(1);

    RetVal_t result = getState(nullptr);

    EXPECT_EQ(result, RET_ERROR);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}