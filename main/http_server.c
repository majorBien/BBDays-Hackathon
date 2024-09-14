/*
 * http_server.c
 *
 *  Created on: 14 wrz 2024
 *      Author: majorbien
 */

#include "http_server.h"


#include "esp_http_server.h"
#include "esp_log.h"

#include "tasks_settings.h"
#include "wifi.h"

#include <string.h> 
#include <cJSON.h> 


static const char TAG[] = "http_server";

static httpd_handle_t http_server_handle = NULL;

static TaskHandle_t task_http_server_monitor = NULL;

static QueueHandle_t http_server_monitor_queue_handle;

static esp_err_t http_server_json_handler(httpd_req_t *req);


extern const uint8_t index_html_start[]				asm("_binary_Index_html_start");
extern const uint8_t index_html_end[]				asm("_binary_Index_html_end");



static void http_server_monitor(void *parameter)
{
	http_server_queue_message_t msg;

	for (;;)
	{
		if (xQueueReceive(http_server_monitor_queue_handle, &msg, portMAX_DELAY))
		{
			switch (msg.msgID)
			{
				case HTTP_MSG_WIFI_CONNECT_INIT:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_INIT");

					break;

				case HTTP_MSG_WIFI_CONNECT_SUCCESS:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");

					break;

				case HTTP_MSG_WIFI_CONNECT_FAIL:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_FAIL");

					break;

				case HTTP_MSG_FIRMWARE_UPDATE_SUCCESSFUL:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPDATE_SUCCESSFUL");

					break;

				case HTTP_MSG_FIRMWARE_UPDATE_FAILED:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPDATE_FAILED");

					break;

				case HTTP_MSG_FIRMWARE_UPATE_INITIALIZED:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPATE_INITIALIZED");

					break;
				case HTTP_MSG_USER_LOGIN_DONE:
					ESP_LOGI(TAG, "HTTP_MSG_USER_LOGIN_DONE");

					break;
					
				case HTTP_MGS_USER_LOGIN_FAIL:
					ESP_LOGI(TAG, "HTTP_MGS_USER_LOGIN_FAIL");

					break;
				case HTTP_MSG_USER_REGISTER_DONE:
					ESP_LOGI(TAG, "HTTP_MSG_USER_REGISTER_DONE");

					break;
				case HTTP_MGS_USER_REGISTER_FAIL:
					ESP_LOGI(TAG, "HTTP_MGS_USER_REGISTER_FAIL");

					break;
				case HTTP_MGS_FIRST_AID_KIT_STATUS:
					ESP_LOGI(TAG, "HTTP_MGS_FIRST_AID_KIT_STATUS");

					break;
				default:
					break;
			}
		}
	}
}




static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "index.html requested");

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);

	return ESP_OK;
}






static httpd_handle_t http_server_configure(void)
{

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	xTaskCreatePinnedToCore(&http_server_monitor, "http_server_monitor", HTTP_SERVER_MONITOR_STACK_SIZE, NULL, HTTP_SERVER_MONITOR_PRIORITY, &task_http_server_monitor, HTTP_SERVER_MONITOR_CORE_ID);

	http_server_monitor_queue_handle = xQueueCreate(3, sizeof(http_server_queue_message_t));

	config.core_id = HTTP_SERVER_TASK_CORE_ID;

	config.task_priority = HTTP_SERVER_TASK_PRIORITY;

	config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

	config.max_uri_handlers = 20;

	config.recv_wait_timeout = 10;
	config.send_wait_timeout = 10;

	ESP_LOGI(TAG,
			"http_server_configure: Starting server on port: '%d' with task priority: '%d'",
			config.server_port,
			config.task_priority);

	if (httpd_start(&http_server_handle, &config) == ESP_OK)
	{
		ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");



		httpd_uri_t Index_html = {
				.uri = "/",
				.method = HTTP_GET,
				.handler = http_server_index_html_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &Index_html);



		httpd_uri_t json_post1 = {
    			.uri = "/register",
   				 .method = HTTP_POST,
   				 .handler = http_server_json_handler,
   				 .user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &json_post1);
				
		httpd_uri_t json_post2 = {
    			.uri = "/login",
   				 .method = HTTP_POST,
   				 .handler = http_server_json_handler,
   				 .user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &json_post2);
		
		httpd_uri_t json_post3 = {
    			.uri = "/index",
   				 .method = HTTP_POST,
   				 .handler = http_server_json_handler,
   				 .user_ctx = NULL
		};		
		
		httpd_register_uri_handler(http_server_handle, &json_post3);
		
		return http_server_handle;
	}

	return NULL;
}


void http_server_start(void)
{
	if (http_server_handle == NULL)
	{
		http_server_handle = http_server_configure();
	}
}

void http_server_stop(void)
{
	if (http_server_handle)
	{
		httpd_stop(http_server_handle);
		ESP_LOGI(TAG, "http_server_stop: stopping HTTP server");
		http_server_handle = NULL;
	}
	if (task_http_server_monitor)
	{
		vTaskDelete(task_http_server_monitor);
		ESP_LOGI(TAG, "http_server_stop: stopping HTTP server monitor");
		task_http_server_monitor = NULL;
	}
}

BaseType_t http_server_monitor_send_message(http_server_message_e msgID)
{
	http_server_queue_message_t msg;
	msg.msgID = msgID;
	return xQueueSend(http_server_monitor_queue_handle, &msg, portMAX_DELAY);
}


static esp_err_t http_server_json_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "JSON data requested");


    return ESP_OK;
}


