/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This example uses SDMMC peripheral to communicate with SD card.
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sd_card.h"
#include "sdkconfig.h"

#define EXAMPLE_MAX_CHAR_SIZE    64

static const char*      TAG = {"SDIO"};

static const char*      MOUNT_POINT = {"/sdcard"};

static sdmmc_card_t*    s_sdcard_instance = NULL;

esp_err_t s_example_write_file(const char *path, char *data)
{
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    return ESP_OK;
}

esp_err_t s_example_read_file(const char *path)
{
    ESP_LOGI(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[EXAMPLE_MAX_CHAR_SIZE];
    fgets(line, sizeof(line), f);
    fclose(f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    return ESP_OK;
}

void sdcard_show_root(void)
{
    DIR *dir = opendir(MOUNT_POINT);
    // 打开当前目录
    if(dir)
    {
        ESP_LOGI(TAG, "List all files...");

        struct dirent *entry = NULL;
        while ((entry = readdir(dir)) != NULL)
        {
            ESP_LOGI(TAG, "%s", entry->d_name);
        }
        closedir(dir);
    }
    else
    {
        ESP_LOGE(TAG, "Open root dir failed.");
    }
}

esp_err_t sdcard_mount(void)
{
    esp_err_t ret;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.width = 4;
    ESP_LOGI(TAG, "Data width: 4 bits.");
#else
    slot_config.width = 1;
    ESP_LOGI(TAG, "Data width: 1 bit.");
#endif

#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = CONFIG_SDIO_PIN_CLK;
    slot_config.cmd = CONFIG_SDIO_PIN_CMD;
    slot_config.d0 = CONFIG_SDIO_PIN_D0;
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_SDIO_PIN_D1;
    slot_config.d2 = CONFIG_SDIO_PIN_D2;
    slot_config.d3 = CONFIG_SDIO_PIN_D3;
#endif  // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
#endif  // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &s_sdcard_instance);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
    }
    else
    {
        ESP_LOGI(TAG, "Filesystem mounted");
    }
    

    return ret;
}

void sdcard_info_log(void)
{
    if(s_sdcard_instance)
    {
        sdmmc_card_print_info(stdout, s_sdcard_instance);
    }
}

esp_err_t sdcard_format(void)
{
    // Format FATFS
    esp_err_t ret = esp_vfs_fat_sdcard_format(MOUNT_POINT, s_sdcard_instance);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
    }
    return ret;
}

void sdcard_unmount(void)
{
    // All done, unmount partition and disable SDMMC peripheral
    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, s_sdcard_instance);
    ESP_LOGI(TAG, "Card unmounted");
    s_sdcard_instance = NULL;
}

void sdcard_init_dummy(void)
{
#if 0
    esp_err_t ret;
    s_example_list_files();

    // First create a file.
    const char *file_hello = MOUNT_POINT"/hello.txt";
    char data[EXAMPLE_MAX_CHAR_SIZE];
    snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
    ret = s_example_write_file(file_hello, data);
    if (ret != ESP_OK) {
        return;
    }

    const char *file_foo = MOUNT_POINT"/foo.txt";
    // Check if destination file exists before renaming
    struct stat st;
    if (stat(file_foo, &st) == 0) {
        // Delete it if it exists
        unlink(file_foo);
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
    if (rename(file_hello, file_foo) != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    ret = s_example_read_file(file_foo);
    if (ret != ESP_OK) {
        return;
    }

    // Format FATFS
    ret = esp_vfs_fat_sdcard_format(mount_point, card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
        return;
    }

    if (stat(file_foo, &st) == 0) {
        ESP_LOGI(TAG, "file still exists");
        return;
    } else {
        ESP_LOGI(TAG, "file doesnt exist, format done");
    }

    const char *file_nihao = MOUNT_POINT"/nihao.txt";
    memset(data, 0, EXAMPLE_MAX_CHAR_SIZE);
    snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Nihao", card->cid.name);
    ret = s_example_write_file(file_nihao, data);
    if (ret != ESP_OK) {
        return;
    }
    //Open file for reading
    ret = s_example_read_file(file_nihao);
    if (ret != ESP_OK) {
        return;
    }
#endif
}
