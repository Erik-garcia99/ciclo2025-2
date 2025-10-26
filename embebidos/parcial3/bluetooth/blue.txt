/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "driver/touch_pad.h"

#include "time.h"
#include "sys/time.h"

#define LED_PIN 2
#define BUFF_RECV_LEN   256

EventGroupHandle_t event_group;
const int bt_data_ready = BIT0;

#define TOUCH_PAD_CHANNEL TOUCH_PAD_NUM4
#define TOUCH_THRESHOLD   0x200   /* promedio obtenido de los experimentos */

enum {
    LED_ON = 97, // a
    LED_OFF,
    LED_STATE,
    CAP_SENSOR_STATE,
};

typedef struct {
    uint16_t filtered_value;
    uint16_t raw_value;
    uint8_t touch;
} TOUCH_SENSOR_READ_t;

uint32_t bt_connection_handle;
bool led_state = 0;

#define SPP_TAG "SPP_ACCEPTOR_DEMO"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_EJEM_SPP"

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const bool esp_spp_enable_l2cap_ertm = true;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

char data_recv[BUFF_RECV_LEN];

static void init_led(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    ESP_LOGI(SPP_TAG, "Init led completed");
}

static void init_touch_sensor(void)
{
    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCH_PAD_CHANNEL, -1);
    touch_pad_filter_start(10);
}

static void read_touch_sensor(TOUCH_SENSOR_READ_t * read)
{
    touch_pad_read_raw_data(TOUCH_PAD_CHANNEL, &read->raw_value);
    touch_pad_read_filtered(TOUCH_PAD_CHANNEL, &read->filtered_value);

    read->touch = (read->filtered_value < TOUCH_THRESHOLD)? 1 : 0 ;
}

#define TOUCH_SENSOR_BUF_LEN 18
#define TOUCH_SENSOR_RESP_LEN 16
void process_bt_data(void *params)
{
  
  while (true)
  {
    xEventGroupWaitBits(event_group, bt_data_ready, true, true, portMAX_DELAY);
    switch(data_recv[0])
    {
        case LED_ON:
        {
            gpio_set_level(LED_PIN, 1);
            led_state = true;
            break;
        }
        case LED_OFF:
        {
            gpio_set_level(LED_PIN, 0);
            led_state = false;
            break;
        }
        case LED_STATE:
        {
            uint8_t state = led_state + '0';
            esp_spp_write(bt_connection_handle, 1, &state);
            break;
        }
        case CAP_SENSOR_STATE:
        {
            TOUCH_SENSOR_READ_t value;
            read_touch_sensor(&value);
            uint8_t resp[TOUCH_SENSOR_RESP_LEN] = {0};
            snprintf((char *)resp, TOUCH_SENSOR_BUF_LEN, "%04X,%04X,%X", value.raw_value, value.filtered_value, value.touch);
            esp_spp_write(bt_connection_handle, TOUCH_SENSOR_RESP_LEN, resp);
            break;
        }
        default: 
            break;
    }
  }
}

static char *bda2str(uint8_t * bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    char bda_str[18] = {0};

    switch (event) {
    /* Is triggered when the SPP layer has been initialized successfully. Usually it's the first event received after initializing the SPP profile. */
    case ESP_SPP_INIT_EVT:
        if (param->init.status == ESP_SPP_SUCCESS) {
            ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
            /* Used to start a Bluetooth SPP server on the ESP32. This allows the ESP32 to accept incoming SPP connections over Bluetooth Classic — acting like a Bluetooth serial port.
             * Function parameters: Requires authentication. Slave role. Let the system automatically assign a free Server Channel Number (SCN). Server's name.
            */
            esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        } else {
            ESP_LOGE(SPP_TAG, "ESP_SPP_INIT_EVT status:%d", param->init.status);
        }
        break;
    /* Is triggered when the SPP service discovery process has completed. The event is relevant when the ESP32 is is acting as a Bluetooth SPP client and is trying to connect to a 
     * remote SPP server. Before initiating the actual connection, the ESP32 needs to discover the remote device's SPP service channel using SDP (Service Discovery Protocol). 
    */
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    /* Is triggered when an SPP client successfully opens a connection to a remote SPP server over Bluetooth Classic. This event does not occur on the server side. 
    */
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
        /* Is triggered when an SPP connection is closed, either by the ESP32 or by the remote device.
        */
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT status:%d handle:%"PRIu32" close_by_remote:%d", param->close.status,
                 param->close.handle, param->close.async);
        break;
    /* Indicates that the SPP server has been started successfully on the ESP32.
    */
    case ESP_SPP_START_EVT:
        if (param->start.status == ESP_SPP_SUCCESS) {
            ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT handle:%"PRIu32" sec_id:%d scn:%d", param->start.handle, param->start.sec_id,
                     param->start.scn);
            esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME); /* Used to set the Bluetooth device name */
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE); /* Configure the Bluetooth scan mode */
        } else {
            ESP_LOGE(SPP_TAG, "ESP_SPP_START_EVT status:%d", param->start.status);
        }
        break;
    /* Is triggered when the SPP client initialization is complete */
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    /* Is triggered when data is received on an SPP connection, whether the ESP32 is acting as a server or client. */
    case ESP_SPP_DATA_IND_EVT:
        uint16_t data_len = param->data_ind.len;
        if (data_len > BUFF_RECV_LEN)
        {
            data_len = BUFF_RECV_LEN;
        }
        memcpy(data_recv, param->data_ind.data, data_len);
        bt_connection_handle = param->data_ind.handle;
        xEventGroupSetBits(event_group, bt_data_ready);
        break;
    /* Occurs when the SPP connection buffer is full or congested, meaning that the ESP32 device can no longer accept more data for transmission. 
    */
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    /* Is triggered when data writing to an SPP connection has been completed successfully.
    */
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    /* Is triggered when an SPP server has been successfully opened and is ready to start communication with a remote SPP client.
        */
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT status:%d handle:%"PRIu32", rem_bda:[%s]", param->srv_open.status,
                 param->srv_open.handle, bda2str(param->srv_open.rem_bda, bda_str, sizeof(bda_str)));
        break;
    /* Is triggered when the SPP server is stopped. This can happen for various reasons, such as:
     * the server is explicitly stopped by the application. Or the Bluetooth stack decides to stop the server due to errors or when the device is no longer in the Bluetooth server mode */
    case ESP_SPP_SRV_STOP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT");
        break;
    /* Is triggered when the SPP subsystem has been uninitialized, meaning the SPP functionalities have been disabled or the SPP stack has been deinitialized. 
     * This can happen for various reasons, such as:
     * The application deinitialize the SPP subsystem. Or the application shuts down or disables Bluetooth functionality entirely. 
    */
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    char bda_str[18] = {0};

    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{ /* Indicates the completion of the authentication (pairing) process when using Bluetooth Classic (not BLE). The event is triggered after a pairing process finishes, whether it succeeds or fails.*/
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s bda:[%s]", param->auth_cmpl.device_name,
                     bda2str(param->auth_cmpl.bda, bda_str, sizeof(bda_str)));
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{ /* Is triggered when a Bluetooth Classic pairing request occurs, specifically when the remote device requests a PIN */
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            /* Responds to a PIN request during the Bluetooth Classic pairing process.
             * The function parameters are: Bluetooth address of the remote device that requested the PIN. Accept or reject the PIN request. PIN length. PIN number. */
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }
    /* Is triggered when the Bluetooth mode of the device changes. Notifies that the Bluetooth mode has transitioned from one state to another, 
     * such as enabling/disabling Bluetooth or switching between Classic Bluetooth and BLE modes. */
    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d bda:[%s]", param->mode_chg.mode,
                 bda2str(param->mode_chg.bda, bda_str, sizeof(bda_str)));
        break;

    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

void app_main(void)
{
    char bda_str[18] = {0};

    init_led();
    init_touch_sensor();
    event_group = xEventGroupCreate();
    xTaskCreate(process_bt_data, "process_bt_data", 2048, NULL, 10, NULL); /* This task will be used to parse the received data and change the LED state */

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE)); /* Releasing memory related to Bluetooth Low Energy (BLE) */

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) { /* Initialize the Bluetooth Controller to allocate tasks and other resources. To handle the physical Bluetooth layer (Bluetooth controller hardware) */
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) { /* Enable the Bluetooth Controller */
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) { /* Initialize the Bluetooth stack (Bluedroid), preparing the device to handle Bluetooth communication */
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) { /* Enable the Bluetooth stack and make it operational. The device is ready to use Bluetooth functionality, such as connecting to other Bluetooth devices and managing Bluetooth profiles. */
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) { /* Register a callback function to handle GAP (Generic Access Profile) events. GAP events are related to the process of discovering devices, initiating and accepting connections. */
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    /* Register a callback function for handling Serial Port Profile (SPP) events. Events related to the connection, disconnection, 
     * data reception, and other SPP-specific actions will trigger callbacks. This function allows to register a handler that will respond to these events.
    */
    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    esp_spp_cfg_t bt_spp_cfg = {
        .mode = esp_spp_mode, /* A callback function is used to handle events such as data reception, connection, disconnection */
        .enable_l2cap_ertm = esp_spp_enable_l2cap_ertm, /* Enable enhanced retransmission mode. Improves reliability, but it may add latency due to retransmission and flow control mechanisms */
        .tx_buffer_size = 0, /* The size of the transmit buffer in bytes. The buffer temporarily stores outgoing data before it is sent over Bluetooth. Only used for ESP_SPP_MODE_VFS mode */
    };
    if ((ret = esp_spp_enhanced_init(&bt_spp_cfg)) != ESP_OK) { /* Initialize Bluetooth SPP with enhanced configuration options */
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code; /* NULL */
    esp_bt_gap_set_pin(pin_type, 0, pin_code); /* The pin is handled in the GAP callback */

    ESP_LOGI(SPP_TAG, "Own address:[%s]", bda2str((uint8_t *)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
}