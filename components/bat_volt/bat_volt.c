#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

const static char *TAG = "EXAMPLE";

//ADC1 Channels
#define BAT_ADC_IO_PIN              10
#define BAT_VOLT_ADC_ATTEN          ADC_ATTEN_DB_12

static bool bat_volt_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void bat_volt_adc_calibration_deinit(adc_cali_handle_t handle);

static adc_oneshot_unit_handle_t s_bat_adc_handle = NULL;
static adc_cali_handle_t s_bat_adc_cali_handle = NULL;
static adc_channel_t s_bat_adc_channel;

void bat_volt_monitor_init(void)
{
    adc_oneshot_unit_init_cfg_t bat_adc_init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = BAT_VOLT_ADC_ATTEN,
    };

    // Init
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&bat_adc_init_cfg, &s_bat_adc_handle));
    // Config
    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(BAT_ADC_IO_PIN, &(bat_adc_init_cfg.unit_id), &s_bat_adc_channel));
    ESP_LOGW(TAG, "Use adc channel %d for bat volt.", s_bat_adc_channel);

    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_bat_adc_handle, s_bat_adc_channel, &config));
#if 0 /* Show GPIO num in log. */
    int adc_channel_ch0_gpio;
    ESP_ERROR_CHECK(adc_oneshot_channel_to_io(bat_adc_init_cfg.unit_id, s_bat_adc_channel, &adc_channel_ch0_gpio));
    ESP_LOGI(TAG, "ADC channel 0 on gpio: %d", adc_channel_ch0_gpio);
#endif
    // Calibration
    // bool do_calibration1_chan0 = example_adc_calibration_init(ADC_UNIT_1, s_bat_adc_channel, EXAMPLE_ADC_ATTEN, &s_bat_adc_cali_handle);
    (void)bat_volt_adc_calibration_init(ADC_UNIT_1, s_bat_adc_channel, BAT_VOLT_ADC_ATTEN, &s_bat_adc_cali_handle);
}

void bat_volt_monitor_deinit(void)
{
    ESP_ERROR_CHECK(adc_oneshot_del_unit(s_bat_adc_handle));
    bat_volt_adc_calibration_deinit(s_bat_adc_cali_handle);
}

int bat_volt_read_mv(void)
{
    int bat_adc_raw;
    int bat_volt_val = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(s_bat_adc_handle, s_bat_adc_channel, &bat_adc_raw));
    // ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, bat_adc_raw);
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(s_bat_adc_cali_handle, bat_adc_raw, &bat_volt_val));
    // ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, bat_volt_val);
    return bat_volt_val;
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool bat_volt_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif
    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

static void bat_volt_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}
