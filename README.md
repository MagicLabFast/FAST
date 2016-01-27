开源SDN交换机项目-FAST
1.项目背景
现有SDN交换机开源项目（如OVS）主要基于软件编写，虽然其分组转发查表等功能在内核中实现，但分组转发性能仍然是主要问题。将软件SDN交换机的分组转发模块卸载到FPGA中实现，不仅能提高交换性能，而且利用FPGA可重构特性可以扩展SDN交换机的功能。
基于NetMagic平台已经有OFS-08和OFS-pro两款支持openflow1.3的SDN交换机实现，在当前的教学科研中得到广泛应用。但这两款平台中，FPGA主要实现CPU的端口扩展功能，openflow转发平面的功能，如协议分析，多元组查表等并没有卸载到FPGA中进行加速，因此在性能上难以进一步提升。
2.FAST模型
FAST（FPGA Accelerated SDN swiTch）的设计思想不同于POF或P4（P4.org）的SDN交换机数据面设计思想。FAST针对网络设备功能随着部署的时间和空间变化而变化的特点，利用FAST中F（FPGA）代表的可现场编程和重构能力，只是针对特定场景的交换需求加载特定的SDN交换机转发平面逻辑。
例如SDN部署在园区网中需要支持IPv6，但不需要支持VxLAN。但在数据中心网络中，可能需要支持VxLAN，但无需支持IPv6。因此可针对园区网和数据中心网络的需求分别设计转发面FPGA逻辑。由于FAST交换机运行时，可针对不同环境需求，加载不同的FPGA逻辑。因此，FAST交换机转发平面的设计逻辑简单，性能高，功耗低。
3.FAST实现平台
3.1 OFS-08交换机
NetMagic08是NetMagic产品系列中为网络教学与研究定制的轻量级开放可重构网络平台，平台采用Altera FPGA，具有硬件可重构性，配合软件提供的NMAC API，可大大降低开发难度，适合于FPGA的初级开发人员使用。
 
基于NetMagic08/OFS-08的FAST开发环境
OFS-08是基于NetMagic08的SDN交换机解决方案，其基本思路是在用户主机（笔记本电脑，服务器等）上运行OVS软件，通过Linux底层虚拟驱动的设计，将主机的1个千兆网络接口扩展成NetMagic08的8个网络接口。通过在主机上下载安装开源的OFS-08软件，以及将NetMagic08配置特定的FPGA映像，用户可轻松拥有一套具有8个千兆网络接口，支持openflow 1.3的交换机，随时在实验室、宿舍或图书馆构建SDN实验网络。
3.2 FAST/NetMagic08平台
FAST基于NetMagic08/OFS-08平台，但NetMagic08的功能不仅仅是通过虚拟驱动扩展用户主机为例接口的数目，而是将更多的openflow1.3的处理功能卸载到FPGA上实现。具体基于OPS-08的实现架构，参见FAST技术白皮书。
4.开源方案
FAST开源遵循Apache2.0许可，范围包括：
	NetMagic08平台硬件PCB的逻辑图，BOM清单，所有Verilog源代码；NetMagic08平台相关软件，包括NMAC协议开发库源代码，demo软件源代码，调试工具源代码等；
	OFS-08交换机的源码，包括硬件FPGA（OFS-08 UM）verilog源代码，Linux协议栈中虚拟驱动程序代码等；
	FAST FPGA中实现OVS datapach功能的协议分析，关键字产生，带掩码多元组查表和action逻辑的所有verilog代码和文档；
5.项目组织
国防科技大学计算机学院NetMagic研究团队（www.netmagic.org）负责项目的整体规划，技术路线的协调；
湖南新实网络科技有限公司（www.magiclab-networks.com）负责FAST软硬件开发库的维护，版本测试和发布，相关培训等。
目前北京邮电大学、东南大学相关课题组已经参与FAST的开发，欢迎更多对SDN感兴趣的项目组加入。
	联系方式：netmagic@nudt.edu.cn

