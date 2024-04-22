#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

TaskHandle_t tempReadingTaskHandle, heatingTaskHandle, coolingTaskHandle;
SemaphoreHandle_t tempMutex, heatingSemaphore, coolingSemaphore;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
#define TEMP_SENSOR_Pin GPIO_PIN_0

void TempReadingTask(void *pvParameters);
void HeatingTask(void *pvParameters);
void CoolingTask(void *pvParameters);
void ButtonTask(void *pvParameters);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void){

	HAL_Init();

	vTaskStartScheduler();

	tempMutex = xSemaphoreCreateMutex();
	heatingSemaphore = xSemaphoreCreateBinary();
	coolingSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(TempReadingTask, "TempRead", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &tempReadingTaskHandle);
	xTaskCreate(HeatingTask, "HeatCtrl", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &heatingTaskHandle);
	xTaskCreate(CoolingTask, "CoolCtrl", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &coolingTaskHandle);

	vTaskStartScheduler();
}

void TempReadingTask(void *pvParameters) {
    while(1) {
        if(xSemaphoreTake(tempMutex, portMAX_DELAY) == pdTRUE) {
            xSemaphoreGive(tempMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void HeatingTask(void *pvParameters) {
    while(1) {
        if(xSemaphoreTake(heatingSemaphore, portMAX_DELAY) == pdTRUE) {
            HAL_GPIO_WritePin(HEATING_GPIO_Port, HEATING_Pin, SET);
            vTaskDelay(pdMS_TO_TICKS(500));
            HAL_GPIO_WritePin(HEATING_GPIO_Port, HEATING_Pin, RESET);
        }
    }
}

void CoolingTask(void *pvParameters) {
    while(1) {
        if(xSemaphoreTake(coolingSemaphore, portMAX_DELAY) == pdTRUE) {
            HAL_GPIO_WritePin(COOLING_GPIO_Port, COOLING_Pin, SET);
            vTaskDelay(pdMS_TO_TICKS(500));
            HAL_GPIO_WritePin(COOLING_GPIO_Port, COOLING_Pin, RESET);
        }
    }
}

void ButtonTask(void *pvParameters){
    while(1) {
        if(HAL_GPIO_ReadPin(HEATING_BUTTON_GPIO_Port, HEATING_BUTTON_Pin) == GPIO_PIN_SET){
            xSemaphoreGive(heatingSemaphore);
        }

        if(HAL_GPIO_ReadPin(COOLING_BUTTON_GPIO_Port, COOLING_BUTTON_Pin) == GPIO_PIN_SET){
            xSemaphoreGive(coolingSemaphore);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
