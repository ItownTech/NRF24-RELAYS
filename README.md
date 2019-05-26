
![images1](https://github.com/huexpub/nrf24-relays/blob/master/images/1relays.png?raw=true)

![images2](https://github.com/huexpub/nrf24-relays/blob/master/images/2relays.png?raw=true)

![images3](https://github.com/huexpub/nrf24-relays/blob/master/images/3relays.png?raw=true)

#
继电器为AC交流电产品，请确保你理解PCB后使用！确保操作规范，焊接无误，否则将会烧坏电源和nrf24m mcu模块！
#
所有继电器均支持外部触发，支持switch button类型。采用NRF24M模块，将支持自定义和遥控模块点到点使用。

#
NRF24M刷机前建议使用arduino环境，否则将会出现无法OTA，

步骤如下：

(以下操作请不要连AC交流电，使用刷机工具即可）

1.先使用proisp刷入hex文件，刷写完成后，

2.模块分配完ID，显示在控制器节点中后，使用arduino仅刷写引导即可。

3.使用Mycontroller控制器，对节点OTA升级(过程不述，请参考mysensor.pub刷机说明)

#
模块仅支持5A继电，建议低于1000W以内，如接led大功率，请必须购买T型号，材质agsion2

##all-in-one

使用此固件，请使用控制器，找到节点ID下的子项 设置var继电器数量1 2 3  ，设置var2为SWITCH模式0  button模式1
