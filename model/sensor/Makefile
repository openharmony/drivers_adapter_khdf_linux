# Copyright (c) 2021 Huawei Device Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

SENSOR_ROOT_DIR = ../../../../../framework/model/sensor/driver

obj-$(CONFIG_DRIVERS_HDF_SENSOR) += \
               $(SENSOR_ROOT_DIR)/common/src/sensor_config_controller.o \
               $(SENSOR_ROOT_DIR)/common/src/sensor_config_parser.o \
               $(SENSOR_ROOT_DIR)/common/src/sensor_device_manager.o \
               $(SENSOR_ROOT_DIR)/common/src/sensor_platform_if.o


obj-$(CONFIG_DRIVERS_HDF_SENSOR_ACCEL) += $(SENSOR_ROOT_DIR)/accel/sensor_accel_driver.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_ACCEL_BMI160) += $(SENSOR_ROOT_DIR)/chipset/accel/accel_bmi160.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_ALS) += $(SENSOR_ROOT_DIR)/als/sensor_als_driver.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_ALS_BH1745) += $(SENSOR_ROOT_DIR)/chipset/als/als_bh1745.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_PROXIMITY) += $(SENSOR_ROOT_DIR)/proximity/sensor_proximity_driver.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_PROXIMITY_APDS9960) += $(SENSOR_ROOT_DIR)/chipset/proximity/proximity_apds9960.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_GYRO) += $(SENSOR_ROOT_DIR)/gyro/sensor_gyro_driver.o 
               
obj-$(CONFIG_DRIVERS_HDF_SENSOR_GYRO_BMI160) += $(SENSOR_ROOT_DIR)/chipset/gyro/gyro_bmi160.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_BAROMETER) += $(SENSOR_ROOT_DIR)/barometer/sensor_barometer_driver.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_BAROMETER_BMP180) += $(SENSOR_ROOT_DIR)/chipset/barometer/barometer_bmp180.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_HALL) += $(SENSOR_ROOT_DIR)/hall/sensor_hall_driver.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_HALL_AK8789) += $(SENSOR_ROOT_DIR)/chipset/hall/hall_ak8789.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_MAGNETIC) += $(SENSOR_ROOT_DIR)/magnetic/sensor_magnetic_driver.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_MAGNETIC_LSM303) += $(SENSOR_ROOT_DIR)/chipset/magnetic/magnetic_lsm303.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_GRAVITY) += $(SENSOR_ROOT_DIR)/accel/sensor_gravity_driver.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_PEDOMETER) += $(SENSOR_ROOT_DIR)/pedometer/sensor_pedometer_driver.o

obj-$(CONFIG_DRIVERS_HDF_SENSOR_PEDOMETER_BMI160) += $(SENSOR_ROOT_DIR)/chipset/pedometer/pedometer_bmi160.o

ccflags-y += -I$(srctree)/drivers/hdf/framework/model/sensor/driver/include \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/common/include \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/chipset/accel \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/accel \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/als \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/chipset/als \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/gyro \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/chipset/gyro \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/pedometer \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/chipset/pedometer \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/barometer \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/chipset/barometer \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/hall \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/chipset/hall \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/chipset/magnetic \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/magnetic \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/proximity \
             -I$(srctree)/drivers/hdf/framework/model/sensor/driver/chipset/proximity \
             -I$(srctree)/drivers/hdf/framework/include/core \
             -I$(srctree)/drivers/hdf/framework/core/common/include/host \
             -I$(srctree)/drivers/hdf/framework/include/utils \
             -I$(srctree)/drivers/hdf/framework/include/osal \
             -I$(srctree)/drivers/hdf/framework/include/platform \
             -I$(srctree)/drivers/hdf/framework/include/config \
             -I$(srctree)/drivers/hdf/khdf/osal/include \
             -I$(srctree)/bounds_checking_function/include
