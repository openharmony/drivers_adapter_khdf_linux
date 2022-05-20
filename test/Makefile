# Copyright (c) 2021-2022 Huawei Device Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
include drivers/hdf/khdf/test/test_khdf.mk
HDF_FRAMWORK_TEST_ROOT = ../../../../framework/test/unittest

obj-y  += $(HDF_FRAMWORK_TEST_ROOT)/common/hdf_main_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/osal/osal_test_entry.o \
          $(HDF_FRAMWORK_TEST_ROOT)/osal/osal_all_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/osal/osal_list_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/osal/osal_file_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/osal/osal_work_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/osal/osal_get_case_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/manager/sample_driver_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/utils/hcs_parser/unittest/hcs_config_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/utils/hcs_parser/unittest/hcs_parser_test.o \
          $(HDF_FRAMWORK_TEST_ROOT)/pm/hdf_pm_driver_test.o

obj-$(CONFIG_DRIVERS_HDF_PLATFORM) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/platform_device_test.o \
                                      $(HDF_FRAMWORK_TEST_ROOT)/platform/common/platform_driver_test.o \
                                      $(HDF_FRAMWORK_TEST_ROOT)/platform/common/platform_event_test.o \
                                      $(HDF_FRAMWORK_TEST_ROOT)/platform/common/platform_manager_test.o \
                                      $(HDF_FRAMWORK_TEST_ROOT)/platform/common/platform_queue_test.o \
                                      $(HDF_FRAMWORK_TEST_ROOT)/platform/common/platform_dumper_test.o \
                                      $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_platform_entry_test.o

ifeq ($(CONFIG_DRIVERS_HDF_PLATFORM_TRACE), y)
obj-$(CONFIG_DRIVERS_HDF_PLATFORM) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/platform_trace_test.o
endif

obj-$(CONFIG_DRIVERS_HDF_PLATFORM_GPIO) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/gpio_test.o \
	                                   $(HDF_FRAMWORK_TEST_ROOT)/platform/common/gpio_driver_test.o \
                                           $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_gpio_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_I2C) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/i2c_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/common/i2c_driver_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_i2c_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_PWM) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/pwm_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/common/pwm_driver_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_pwm_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_RTC) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/rtc_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/common/rtc_driver_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_rtc_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_SPI) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/spi_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/common/spi_driver_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_spi_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_SDIO) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/sdio_test.o \
                                           $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_sdio_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_EMMC) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/emmc_test.o \
                                           $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_emmc_entry_test.o

obj-$(CONFIG_DRIVERS_HDF_PLATFORM_UART) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/uart_test.o \
                                           $(HDF_FRAMWORK_TEST_ROOT)/platform/common/uart_driver_test.o \
                                           $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_uart_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_WATCHDOG) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/watchdog_test.o \
                                               $(HDF_FRAMWORK_TEST_ROOT)/platform/common/watchdog_driver_test.o \
                                               $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_watchdog_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_MIPI_DSI) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/mipi_dsi_test.o \
                                               $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_mipi_dsi_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_MIPI_CSI) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/mipi_csi_test.o \
                                               $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_mipi_csi_entry_test.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_REGULATOR) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/regulator_test.o \
                                                $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_regulator_entry_test.o \
                                                $(HDF_FRAMWORK_TEST_ROOT)/platform/virtual/regulator_linux_voltage_virtual_driver.o \
                                                $(HDF_FRAMWORK_TEST_ROOT)/platform/virtual/regulator_linux_current_virtual_driver.o
obj-$(CONFIG_DRIVERS_HDF_PLATFORM_ADC) += $(HDF_FRAMWORK_TEST_ROOT)/platform/common/adc_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/common/adc_driver_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/entry/hdf_adc_entry_test.o \
                                          $(HDF_FRAMWORK_TEST_ROOT)/platform/virtual/adc_linux_virtual_iio_driver.o

obj-$(CONFIG_DRIVERS_HDF_WIFI) += $(HDF_FRAMWORK_TEST_ROOT)/wifi/hdf_wifi_test.o \
                                  $(HDF_FRAMWORK_TEST_ROOT)/model/network/wifi/unittest/netdevice/net_device_test.o \
                                  $(HDF_FRAMWORK_TEST_ROOT)/model/network/wifi/unittest/module/hdf_module_test.o \
                                  $(HDF_FRAMWORK_TEST_ROOT)/model/network/wifi/unittest/net/hdf_netbuf_test.o \
                                  $(HDF_FRAMWORK_TEST_ROOT)/model/network/wifi/unittest/qos/flow_control_test.o \
                                  $(HDF_FRAMWORK_TEST_ROOT)/model/network/wifi/unittest/message/hdf_queue_test.o \
                                  $(HDF_FRAMWORK_TEST_ROOT)/model/network/wifi/unittest/message/hdf_single_node_message_test.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR) += $(HDF_FRAMWORK_TEST_ROOT)/sensor/hdf_sensor_test.o

obj-$(CONFIG_DRIVERS_HDF_AUDIO_TEST) += $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/hdf_audio_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_host_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_core_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_parse_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_sapm_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_codec_base_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_dai_base_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_dma_base_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_dsp_base_test.o \
                                        $(HDF_FRAMWORK_TEST_ROOT)/model/audio/src/audio_platform_base_test.o \
                                        $(HDF_AUDIO_DRIVER_TEST_ROOT)/src/hdf_audio_driver_test.o \
                                        $(HDF_AUDIO_DRIVER_TEST_ROOT)/src/hi3516_common_func.o \
                                        $(HDF_AUDIO_DRIVER_TEST_ROOT)/src/hi3516_dai_ops_test.o \
                                        $(HDF_AUDIO_DRIVER_TEST_ROOT)/src/hi3516_dma_ops_test.o \
                                        $(HDF_AUDIO_DRIVER_TEST_ROOT)/src/hi3516_codec_ops_test.o \
                                        $(HDF_AUDIO_DRIVER_TEST_ROOT)/src/hi3516_codec_impl_test.o \
                                        $(HDF_AUDIO_DRIVER_TEST_ROOT)/src/hi3516_aiao_impl_test.o

ccflags-y += -I$(srctree)/drivers/hdf/framework/include \
             -I$(srctree)/drivers/hdf/framework/include/osal \
             -I$(srctree)/drivers/hdf/framework/include/utils \
             -I$(srctree)/drivers/hdf/framework/include \
             -I$(srctree)/drivers/hdf/framework/utils/include \
             -I$(srctree)/drivers/hdf/framework/include/core \
             -I$(srctree)/drivers/hdf/framework/core/host/include \
             -I$(srctree)/drivers/hdf/framework/core/manager/include \
             -I$(srctree)/drivers/hdf/framework/core/shared/include \
             -I$(srctree)/drivers/hdf/framework/core/common/include/host \
             -I$(srctree)/drivers/hdf/framework/core/common/include/manager \
             -I$(srctree)/drivers/hdf/framework/core/adapter/vnode/include \
             -I$(srctree)/bounds_checking_function/include \
             -I$(srctree)/drivers/hdf/framework/test/unittest/config \
             -I$(srctree)/drivers/hdf/framework/test/unittest/utils/hcs_parser/unittest \
             -I$(srctree)/drivers/hdf/framework/include/config \
             -I$(srctree)/drivers/hdf/framework/test/unittest/osal \
             -I$(srctree)/drivers/hdf/khdf/osal/include
