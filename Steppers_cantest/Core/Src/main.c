/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskCanRx */
osThreadId_t TaskCanRxHandle;
const osThreadAttr_t TaskCanRx_attributes = {
  .name = "TaskCanRx",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskCanTx */
osThreadId_t TaskCanTxHandle;
const osThreadAttr_t TaskCanTx_attributes = {
  .name = "TaskCanTx",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TaskCanReset */
osThreadId_t TaskCanResetHandle;
const osThreadAttr_t TaskCanReset_attributes = {
  .name = "TaskCanReset",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
void StartDefaultTask(void *argument);
void can_rx(void *argument);
void can_tx(void *argument);
void can_reset(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define PING_ID 0x120
#define THROTTLE_POS 0x180
#define THROTTLE_MAX 0X181
#define CAR_MODE 0x100
#define CURTIS_MODE 0x101
#define THROTTLE_SEND 0x182
CAN_TxHeaderTypeDef TxHeader;
uint8_t buf[8];
uint32_t  TxMailbox;
uint8_t CarTxData[] = {0x00};
bool transmit_tx;
void can_init(){

	  CAN_FilterTypeDef canfilterconfig;
	    canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	    canfilterconfig.FilterBank = 3;		// Specify filter bank to use
	    canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0; //Incoming data is saved here
	    canfilterconfig.FilterIdHigh = 0x120<<5;//0x000<<5;
	    canfilterconfig.FilterIdLow = 0x0000;
	    canfilterconfig.FilterMaskIdHigh= 0xFF0<<5;//0x600<<5;
	    canfilterconfig.FilterMaskIdLow = 0x0000;
	    canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	    canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	    canfilterconfig.SlaveStartFilterBank = 15;
	  if (HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig) != HAL_OK)
	  {
		Error_Handler();
	  }
	  if(HAL_CAN_Start(&hcan1)!= HAL_OK)
	  {
		  Error_Handler();
	  }

	  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	  {
		/* Notification Error */
		Error_Handler();
	  }
}
void can_parse_msg(CAN_RxHeaderTypeDef *header, uint8_t *data){
	if(data == NULL) return;
	//uint8_t id = can_parse_id(data, header->DLC);
	uint32_t id = header->StdId;
	if(id == PING_ID){
		transmit_tx = true;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  can_init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of TaskCanRx */
  TaskCanRxHandle = osThreadNew(can_rx, NULL, &TaskCanRx_attributes);

  /* creation of TaskCanTx */
  TaskCanTxHandle = osThreadNew(can_tx, NULL, &TaskCanTx_attributes);

  /* creation of TaskCanReset */
  TaskCanResetHandle = osThreadNew(can_reset, NULL, &TaskCanReset_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 40;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_can_rx */
/**
* @brief Function implementing the TaskCanRx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can_rx */
void can_rx(void *argument)
{
  /* USER CODE BEGIN can_rx */
  /* Infinite loop */
  for(;;)
  {
	CAN_RxHeaderTypeDef rxHeader;
	if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) != 0){
			HAL_StatusTypeDef ret = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, buf);
			if(ret == HAL_OK) {
				can_parse_msg(&rxHeader, buf);
//				HAL_GPIO_WritePin(Debug_1_GPIO_Port, Debug_1_Pin, GPIO_PIN_SET);
			}
			else{
//				HAL_GPIO_WritePin(Debug_1_GPIO_Port, Debug_1_Pin, GPIO_PIN_RESET);
			}
		}
	osDelay(20);
  }
  /* USER CODE END can_rx */
}

/* USER CODE BEGIN Header_can_tx */
/**
* @brief Function implementing the TaskCanTx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can_tx */
void can_tx(void *argument)
{
  /* USER CODE BEGIN can_tx */
  /* Infinite loop */
  for(;;)
  {
	  //transmit_tx = 1;
	  if(transmit_tx){
		  if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 0) {
			  TxHeader.StdId = 0x110;
			  TxHeader.IDE = CAN_ID_STD;
			  TxHeader.RTR = CAN_RTR_DATA;
			  TxHeader.DLC = 1;
			  TxHeader.TransmitGlobalTime = DISABLE;
			  CarTxData[0] = 0x1;
			if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, CarTxData, &TxMailbox)== HAL_OK) {

//				HAL_GPIO_WritePin(GPIOB, Debug_2_Pin, GPIO_PIN_SET);
					  if(!HAL_CAN_IsTxMessagePending(&hcan1, TxMailbox)){
						  printf("Message sent 1\n");

					  }
					  else{
						  printf("Message pending 1\n");
					  }
				  } else{

						 HAL_CAN_ResetError(&hcan1);
				  }
		  }else{
//			  HAL_GPIO_WritePin(GPIOB, Debug_2_Pin, GPIO_PIN_RESET);
		  }
			transmit_tx=false;
	  }
	    osDelay(20);
	  }
  /* USER CODE END can_tx */
}

/* USER CODE BEGIN Header_can_reset */
/**
* @brief Function implementing the TaskCanReset thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can_reset */
void can_reset(void *argument)
{
  /* USER CODE BEGIN can_reset */
  /* Infinite loop */
  for(;;)
  {
	  bool initializingBus = false;

	  // Check for bus off bit
	  if(READ_BIT(hcan1.Instance->ESR, CAN_ESR_BOFF)){
		  // Bus is off!!!! Reset bus
		  SET_BIT(hcan1.Instance->MCR, CAN_MCR_INRQ);
		  initializingBus = true;
	  }
	  if(initializingBus && READ_BIT(hcan1.Instance->MSR,
	  CAN_MSR_INAK)){
		  // Bus is initialized, reset init bit
		  CLEAR_BIT(hcan1.Instance->MCR, CAN_MCR_INRQ);
		  initializingBus = false;
	  }
    osDelay(20);
  }
  /* USER CODE END can_reset */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
