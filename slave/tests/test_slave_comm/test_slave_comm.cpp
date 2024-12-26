#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "logger.h"
#include "slave_comm.h"
#include "task.h"

// ==========================
// **Define Missing Constants**
// ==========================
// Default timeout for queue send operations in milliseconds
#ifndef TICK_TO_WAIT_SEND_MS
#define TICK_TO_WAIT_SEND_MS 100
#endif

// ==========================
// **Handle Macro Conflicts**
// ==========================
// Prevent macro conflicts with FreeRTOS queue functions
#ifdef xQueueSend
#undef xQueueSend
#endif

#ifdef xQueueGenericSend
#undef xQueueGenericSend
#endif

// ==========================
// **Mock Class for FreeRTOS and Logger**
// ==========================
// Mock class for simulating FreeRTOS and Logger behaviors
class FreeRTOSMock {
public:
    MOCK_METHOD(BaseType_t, xQueueSend, (QueueHandle_t queue, const void* item, TickType_t wait));
    MOCK_METHOD(BaseType_t, xQueueGenericSend, (QueueHandle_t queue, const void* item, TickType_t wait, BaseType_t copyPosition));
    MOCK_METHOD(BaseType_t, xQueueReceive, (QueueHandle_t queue, void* item, TickType_t wait));
    MOCK_METHOD(void, logMessage, (LogLevel level, const char* module, const char* message));
    MOCK_METHOD(void, vTaskDelay, (TickType_t xTicksToDelay));
};

// ==========================
// **Global Mock Object**
// ==========================
// Global mock object for FreeRTOS interactions
FreeRTOSMock* freeRTOSMock = nullptr;

// ==========================
// **C Function Redirection**
// ==========================
// Redirect C-style FreeRTOS API calls to the mock object
extern "C" {
    BaseType_t xQueueSend(QueueHandle_t queue, const void* item, TickType_t wait) {
        if (freeRTOSMock == nullptr) {
            abort();
        }
        return freeRTOSMock->xQueueSend(queue, item, wait);
    }

    BaseType_t xQueueGenericSend(QueueHandle_t queue, const void* item, TickType_t wait, BaseType_t copyPosition) {
        if (freeRTOSMock == nullptr) {
            abort();
        }
        return freeRTOSMock->xQueueGenericSend(queue, item, wait, copyPosition);
    }

    BaseType_t xQueueReceive(QueueHandle_t queue, void* item, TickType_t wait) {
        if (freeRTOSMock == nullptr) {
            abort();
        }
        return freeRTOSMock->xQueueReceive(queue, item, wait);
    }

    void vTaskDelay(TickType_t xTicksToDelay) {
        if (freeRTOSMock == nullptr) {
            abort();
        }
        freeRTOSMock->vTaskDelay(xTicksToDelay);
    }

    void logMessage(LogLevel level, const char* module, const char* message) {
        if (freeRTOSMock == nullptr) {
            abort();
        }
        freeRTOSMock->logMessage(level, module, message);
    }
}

// ==========================
// **Test Fixture**
// ==========================
// Test setup and teardown for SlaveComm module
class SlaveCommTest : public ::testing::Test {
protected:
    QueueHandle_t stateQueueHandler_ = nullptr;
    void SetUp() override {
        freeRTOSMock = new FreeRTOSMock();
        ASSERT_NE(freeRTOSMock, nullptr);

        stateQueueHandler_ = reinterpret_cast<QueueHandle_t>(0x1234);
        initSlaveComm(stateQueueHandler_);
    }

    void TearDown() override {
        delete freeRTOSMock;
        freeRTOSMock = nullptr;
    }
};

// ==========================
// **1. Initialization Tests**
// ==========================
// Test successful initialization of SlaveComm
TEST_F(SlaveCommTest, InitSlaveComm_SuccessfulInitialization) {
    RetVal_t result = initSlaveComm(stateQueueHandler_);
    EXPECT_EQ(result, RET_OK);
}

// Test failed initialization of SlaveComm with NULL queue
TEST_F(SlaveCommTest, InitSlaveComm_FailedInitialization) {
    RetVal_t result = initSlaveComm(NULL);
    EXPECT_EQ(result, RET_ERROR);
}

// ==========================
// **2. Message Sending Tests**
// ==========================
// Test successful message sending
TEST_F(SlaveCommTest, SendMsgSlave_Success) {
    uint8_t data = 1;
    EXPECT_CALL(*freeRTOSMock, xQueueSend(stateQueueHandler_, &data, pdMS_TO_TICKS(TICK_TO_WAIT_SEND_MS)))
        .WillOnce(testing::Return(pdPASS));
    EXPECT_EQ(sendMsgSlave(&data), RET_OK);
}

// Test failed message sending
TEST_F(SlaveCommTest, SendMsgSlave_Failure) {
    uint8_t data = 1;
    EXPECT_EQ(sendMsgSlave(&data), RET_ERROR);
}

// ==========================
// **3. Message Receiving Tests**
// ==========================
// Test successful message receiving
TEST_F(SlaveCommTest, ReciveMsgSlave_Success) {
    EXPECT_CALL(*freeRTOSMock, xQueueReceive(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(testing::Return(pdPASS));
    char buffer[10];
    RetVal_t result = reciveMsgSlave(buffer);
    EXPECT_EQ(result, RET_OK);
}

// Test failed message receiving
TEST_F(SlaveCommTest, ReciveMsgSlave_Failure) {
    EXPECT_CALL(*freeRTOSMock, xQueueReceive(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(testing::Return(pdFAIL));
    char buffer[10];
    RetVal_t result = reciveMsgSlave(buffer);
    EXPECT_EQ(result, RET_ERROR);
}

// ==========================
// **Main Test Runner**
// ==========================
// Entry point for the test runner
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
