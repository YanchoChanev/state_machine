#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "master_comm.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "logger.h"
#include <iostream>

// ==========================
// Handle Macro Conflicts
// ==========================
#ifdef xQueueSend
#undef xQueueSend
#endif

#ifdef xQueueReceive
#undef xQueueReceive
#endif

// ==========================
// Define Missing Constants
// ==========================
#ifndef TICK_TO_WAIT_SEND_MS
#define TICK_TO_WAIT_SEND_MS 100
#endif

// ==========================
// Mock Class for FreeRTOS and Logger
// ==========================
// This mock class is used to simulate FreeRTOS functions and logger functionality
class FreeRTOSMock {
public:
    MOCK_METHOD(BaseType_t, xQueueSend, (QueueHandle_t, const void*, TickType_t));
    MOCK_METHOD(BaseType_t, xQueueReceive, (QueueHandle_t, void*, TickType_t));
    MOCK_METHOD(void, logMessage, (LogLevel, const char*, const char*));
    MOCK_METHOD(void, vTaskDelay, (TickType_t xTicksToDelay));
};

// Global Mock Object
// A global pointer to the mock object to facilitate redirection from C functions
FreeRTOSMock* freeRTOSMock = nullptr;

// ==========================
// C Function Redirection
// ==========================
// Weakly linked C functions redirected to the mock object
extern "C" {
    __attribute__((weak)) BaseType_t xQueueSend(QueueHandle_t queue, const void* item, TickType_t wait) {
        if (freeRTOSMock == nullptr) {
            abort();
        }
        if (queue == nullptr || item == nullptr) {
            abort();
        }
        return freeRTOSMock->xQueueSend(queue, item, wait);
    }

    __attribute__((weak)) BaseType_t xQueueReceive(QueueHandle_t queue, void* item, TickType_t wait) {
        if (freeRTOSMock == nullptr) {
            abort();
        }
        if (queue == nullptr || item == nullptr) {
            abort();
        }
        return freeRTOSMock->xQueueReceive(queue, item, wait);
    }

    __attribute__((weak)) BaseType_t xQueueGenericSend(QueueHandle_t queue, const void* item, TickType_t wait, BaseType_t copyPosition) {
        if (freeRTOSMock == nullptr) {
            abort();
        }
        return freeRTOSMock->xQueueSend(queue, item, wait);
    }

    __attribute__((weak)) QueueHandle_t xQueueGenericCreate(UBaseType_t queueLength, UBaseType_t itemSize, uint8_t queueType) {
        return reinterpret_cast<QueueHandle_t>(1); // Return a dummy valid handle
    }

    __attribute__((weak)) void vQueueAddToRegistry(QueueHandle_t queue, const char* name) {
    }

    __attribute__((weak)) void vQueueWaitForMessageRestricted(QueueHandle_t xQueue, TickType_t xTicksToWait, BaseType_t xWaitIndefinitely) {
    }

    __attribute__((weak)) BaseType_t xQueueGenericSendFromISR(QueueHandle_t queue, const void* item, BaseType_t* pxHigherPriorityTaskWoken, BaseType_t copyPosition) {
        return pdPASS;
    }

    __attribute__((weak)) void vTaskDelay(TickType_t xTicksToDelay) {
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
// Test Fixture
// ==========================
// Provides a consistent test environment for MasterComm
class MasterCommTest : public ::testing::Test {
protected:
    QueueHandle_t stateQueueHandle_ = nullptr;
    void SetUp() override {
        freeRTOSMock = new FreeRTOSMock();
        ASSERT_NE(freeRTOSMock, nullptr);

        stateQueueHandle_ = reinterpret_cast<QueueHandle_t>(0x1234);
        EXPECT_CALL(*freeRTOSMock, logMessage(testing::_, testing::_, testing::_))
        .Times(testing::AnyNumber());
        initMasterComm(stateQueueHandle_);
    }

    void TearDown() override {
        delete freeRTOSMock;
        freeRTOSMock = nullptr;
    }
};

// ==========================
// Unit Tests
// ==========================
// Valid QueueHandle Test
TEST_F(MasterCommTest, InitMasterComm_ValidQueueHandle_ReturnsRET_OK) {
    ASSERT_NE(stateQueueHandle_, nullptr);
    EXPECT_EQ(initMasterComm(stateQueueHandle_), RET_OK);
}

// Null QueueHandle Test
TEST_F(MasterCommTest, InitMasterComm_NullQueueHandle_ReturnsRET_ERROR) {
    ASSERT_NE(freeRTOSMock, nullptr);
    EXPECT_EQ(initMasterComm(nullptr), RET_ERROR);
}

// Successful Message Send Test
TEST_F(MasterCommTest, SendMsgMaster_QueueSendSuccess_ReturnsRET_OK) {
    uint8_t data = 1;
    EXPECT_CALL(*freeRTOSMock, xQueueSend(stateQueueHandle_, &data, pdMS_TO_TICKS(TICK_TO_WAIT_SEND_MS)))
        .WillOnce(testing::Return(pdPASS));
    EXPECT_EQ(sendMsgMaster(&data), RET_OK);
}

// Failed Message Send Test
TEST_F(MasterCommTest, SendMsgMaster_QueueSendFailure_ReturnsRET_ERROR) {
    uint8_t data = 1;
    EXPECT_CALL(*freeRTOSMock, xQueueSend(stateQueueHandle_, testing::_, pdMS_TO_TICKS(TICK_TO_WAIT_SEND_MS)))
        .WillOnce(testing::Return(pdFAIL));
    EXPECT_EQ(sendMsgMaster(&data), RET_ERROR);
}

// Successful Message Receive Test
TEST_F(MasterCommTest, ReciveMsgMaster_QueueReceiveSuccess_ReturnsRET_OK) {
    uint8_t data = 1;
    EXPECT_CALL(*freeRTOSMock, xQueueReceive(stateQueueHandle_, testing::_, portMAX_DELAY))
        .WillOnce(testing::Return(pdPASS));
    EXPECT_EQ(reciveMsgMaster(&data), RET_OK);
}

// Failed Message Receive Test
TEST_F(MasterCommTest, ReciveMsgMaster_QueueReceiveFailure_ReturnsRET_ERROR) {
    uint8_t data = 1;
    EXPECT_CALL(*freeRTOSMock, xQueueReceive(stateQueueHandle_, testing::_, portMAX_DELAY))
        .WillOnce(testing::Return(pdFAIL));
    EXPECT_EQ(reciveMsgMaster(&data), RET_ERROR);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
