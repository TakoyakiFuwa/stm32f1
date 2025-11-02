/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Descriptors for QYHID Mouse Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_desc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const uint8_t QYHID_DeviceDescriptor[JOYSTICK_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize 64*/
    0x83,                       /*idVendor (0x0483)*/
    0x04,
    0x11,                       /*idProduct = 0x5711*/
    0x57,
    0x00,                       /*bcdDevice rel. 2.00*/
    0x02,
    1,                          /*Index of string descriptor describing
                                                  manufacturer */
    2,                          /*Index of string descriptor describing
                                                 product*/
    3,                          /*Index of string descriptor describing the
                                                 device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* QYHID_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t QYHID_ConfigDescriptor[JOYSTICK_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    JOYSTICK_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x01,         /*bNumInterfaces: 1 interface*/
    0x01,         /*bConfigurationValue: Configuration value*/
    0x00,         /*iConfiguration: Index of string descriptor describing
                                     the configuration*/
    0xE0,         /*bmAttributes: Self powered */
    0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

    /************** Descriptor of QYHID Mouse interface ****************/
    /* 09 */
    0x09,         /*bLength: Interface Descriptor size*/
    USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
    0x00,         /*bInterfaceNumber: Number of Interface*/
    0x00,         /*bAlternateSetting: Alternate setting*/
    0x01,         /*bNumEndpoints*/
    0x03,         /*bInterfaceClass: HID*/
    0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x01,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0,            /*iInterface: Index of string descriptor*/
    /******************** Descriptor of QYHID Mouse HID ********************/
    /* 18 */
    0x09,         /*bLength: HID Descriptor size*/
    HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x11,         /*bcdHID: HID Class Spec release number*/
    0x01,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    JOYSTICK_SIZ_REPORT_DESC,/*wItemLength: Total length of Report descriptor*/						//这里....
    0x00,
    /******************** Descriptor of QYHID Mouse endpoint ********************/
    /* 27 */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

    0x81,          /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    0x08,          /*wMaxPacketSize: 4 Byte max */
    0x00,
    0x0A,          /*bInterval: Polling Interval (32 ms)*/										//原来是0x20
    /* 34 */
  }
  ; /* MOUSE_ConfigDescriptor */
 
 const uint8_t QYHID_ReportDescriptor[] =
{
    0x05, 0x01,       /* Usage Page (Generic Desktop Controls) 
                         —— 指明本设备属于“通用桌面控制”类别，
                            HID 键盘/鼠标等都在这个类别下定义。 */

    0x09, 0x06,       /* Usage (Keyboard)
                         —— 表示这是一个键盘设备。 */

    0xA1, 0x01,       /* Collection (Application)
                         —— 开始一个“应用集合”，
                            表示该集合对应一个完整的键盘应用。 */

    /* ------------------ 修饰键（Modifier Keys） ------------------ */
    0x05, 0x07,       /* Usage Page (Keyboard/Keypad)
                         —— 后续的键值属于键盘/小键盘用途页。 */

    0x19, 0xE0,       /* Usage Minimum (Keyboard LeftControl = 224)
                         —— 修饰键的起始键码（E0）代表左 Ctrl。 */
    0x29, 0xE7,       /* Usage Maximum (Keyboard Right GUI = 231)
                         —— 修饰键的结束键码（E7）代表右 Win/Command。 */

    0x15, 0x00,       /* Logical Minimum (0)
                         —— 数据逻辑最小值为 0（未按下）。 */
    0x25, 0x01,       /* Logical Maximum (1)
                         —— 数据逻辑最大值为 1（按下）。 */

    0x75, 0x01,       /* Report Size (1)
                         —— 每个修饰键占 1 bit。 */
    0x95, 0x08,       /* Report Count (8)
                         —— 一共有 8 个修饰键（Ctrl、Shift、Alt、GUI 各左右）。 */

    0x81, 0x02,       /* Input (Data,Var,Abs)
                         —— 定义输入项：
                            Data 表示真实数据；
                            Var 表示每个 bit 是单独变量；
                            Abs 表示绝对值。
                            --> 这 8 bit 对应修饰键状态。 */

    /* ------------------ 保留字节 ------------------ */
    0x95, 0x01,       /* Report Count (1)
                         —— 报告中包含 1 个保留字节。 */
    0x75, 0x08,       /* Report Size (8)
                         —— 该保留字节为 8 bit。 */
    0x81, 0x03,       /* Input (Const,Var,Abs)
                         —— 常量输入（主机忽略此字节），
                            用作填充对齐。 */

    /* ------------------ LED 指示灯输出 ------------------ */
    0x95, 0x05,       /* Report Count (5)
                         —— 有 5 个 LED 指示灯（NumLock、CapsLock、ScrollLock 等）。 */
    0x75, 0x01,       /* Report Size (1)
                         —— 每个 LED 状态占 1 bit。 */
    0x05, 0x08,       /* Usage Page (LEDs)
                         —— 表示后续用途属于 LED 控制页。 */
    0x19, 0x01,       /* Usage Minimum (Num Lock) */
    0x29, 0x05,       /* Usage Maximum (Kana)
                         —— 定义 1~5 分别对应 NumLock, CapsLock, ScrollLock, Compose, Kana。 */
    0x91, 0x02,       /* Output (Data,Var,Abs)
                         —— 定义输出数据（从主机到设备），
                            表示 LED 状态。 */

    0x95, 0x01,       /* Report Count (1)
                         —— 额外增加 1 个字节用于填充。 */
    0x75, 0x03,       /* Report Size (3)
                         —— 3 个 bit 填充以对齐到整字节。 */
    0x91, 0x03,       /* Output (Const,Var,Abs)
                         —— 固定输出，不使用，纯填充用。 */

    /* ------------------ 普通按键输入区 ------------------ */
    0x95, 0x06,       /* Report Count (6)
                         —— 一次最多同时报告 6 个按键。 */
    0x75, 0x08,       /* Report Size (8)
                         —— 每个按键占 1 字节。 */

    0x15, 0x00,       /* Logical Minimum (0)
                         —— 按键未按下时的键码值最小为 0。 */
    0x25, 0x65,       /* Logical Maximum (101)
                         —— 最大键码值 = 101，对应标准键盘的按键数。 */

    0x05, 0x07,       /* Usage Page (Keyboard/Keypad)
                         —— 使用键盘用途页。 */
    0x19, 0x00,       /* Usage Minimum (0)
                         —— 最小键值 0。 */
    0x29, 0x65,       /* Usage Maximum (101)
                         —— 最大键值 101。 */

    0x81, 0x00,       /* Input (Data,Array)
                         —— 定义按键输入区：
                            Data 表示真实数据；
                            Array 表示一组按键值数组；
                            主机会从这 6 字节中读取当前按下的按键代码。 */

    0xC0              /* End Collection
                         —— 结束整个键盘集合。 */
};


/* USB String Descriptors (optional) */
const uint8_t QYHID_StringLangID[JOYSTICK_SIZ_STRING_LANGID] =
  {
    JOYSTICK_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

const uint8_t QYHID_StringVendor[JOYSTICK_SIZ_STRING_VENDOR] =
  {
    JOYSTICK_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
    'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
    'c', 0, 's', 0
  };
const uint8_t QYHID_StringProduct[JOYSTICK_SIZ_STRING_PRODUCT] =
  {
    JOYSTICK_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'J', 0,
    'o', 0, 'y', 0, 's', 0, 't', 0, 'i', 0, 'c', 0, 'k', 0
  };
uint8_t QYHID_StringSerial[JOYSTICK_SIZ_STRING_SERIAL] =
  {
    JOYSTICK_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

