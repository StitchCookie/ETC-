#pragma once
#include <Qtcore/qglobal.h>
#pragma pack(1)
enum class HandrailControl :quint8 {
    HANDRAIL_RAISE = 0X00,
     HANDRAIL_DOWN = 0X01

};
/**     **
	@brief: 设备类型编码表
	@detail:自动栏杆机	11H
			车检	21H
*/
enum class DeviceTypeTable :quint8
{
	HandrailDeviceId = 0X11,
	CarDeviceId = 0X21
};
/**     **
	@brief: 抬落杆到位信号
			@detail:1	CmdType	指令代码，此处取值A2H
			DeviceId	设备编码，参见设备类型编码
			DeviceNo	设备序号，从01H开始编号
			BarStaus	栏杆状态，00H-落杆到位，01H-抬杆到位
	@note: 栏杆机反馈栏杆状态，状态发生改变时上报，外场控制服务无需应答。
*/
struct Handrail_OK {
	quint8  mdType = 0XA2;
	DeviceTypeTable DeviceId = DeviceTypeTable::HandrailDeviceId;
	quint8	DeviceNo;
	quint8	BarStaus;
};
/**     **
	@brief: 车检结构体
	@detail:CmdType	指令代码，此处取值B1H
			DeviceId	设备编码，参见设备类型编码
			DeviceNo	设备序号，从01H开始编号
			Status	车检状态，00H-无车，01H-有车
			Direction	行车方向，01H-驶出，02H-驶入，03H-倒车，默认00H
			Axle	检测车轴数量，默认00H
			Reserved	保留字节，默认00H
	@note:设备检测到车辆时主动上报，外场控制服务无需应答。
		  不需要判断信号持续时间。
*/
struct  VehicleInspection {
	quint8 CmdType = 0XB1;
	DeviceTypeTable DeviceId = DeviceTypeTable::CarDeviceId;
	quint8 DeviceNo;
	quint8 Status;
	quint8 Direction = 0X00;
	quint8 Axle = 0X00;
	quint64 Reserved = 0X00;
};
/**     **
	@brief: 心跳请求及应答包
	@detail:CmdType	指令代码，此处取值EDH
			UnixTm	Unix时间戳
	@note:  外设控制服务定时向设备发送心跳，心跳发送间隔10秒，客户端收到后应答。
*/
struct HeartBeat{
	quint8 CmdType = 0XED;
	quint32 UnixTm;
};

/**     **
	@brief: 栏杆机请求包
	@detail:CmdType	指令代码，此处取值A1H
			MsgID	消息ID
			DeviceId	设备编码，参见设备类型编码
			DeviceNo	设备序号，从01H开始编号
			WarmStaus	00H-落杆 01H-抬杆
			Reserved	保留字节，默认00H

	@note: 外场控制服务发送栏杆控制指令给设备，设备收到后应答
*/
struct Handrail_Request {
	quint8	CmdType = 0XA1;
	qint32	MsgID;
	DeviceTypeTable DeviceId = DeviceTypeTable::CarDeviceId;
	quint8	DeviceNo;
	quint8	WarmStaus;
	quint8  Reserved = 0X00;
};
/**     **
	@brief: 栏杆机应答包
	@detail:CmdType	指令代码，此处取值A1H
			ErrCode	状态码，00H-控制成功，其它失败
			MsgID	消息ID，和请求包里的MsgID一致
			UnixTm1	收到通知时间，Unix时间戳，单位毫秒
			UnixTm2	控制完成时间，Unix时间戳，单位毫秒
			Reserved	保留字节，默认00H
	@note: 栏杆机完成控制后返回
*/
struct Handrail_Ack {
	quint8	CmdType = 0XA1;
	quint8  ErrCode;
	quint32	MsgID;
    quint64  UnixTm1;
	quint64  UnixTm2;
	quint8	Reserved = 0X00;
};
/**     **
	@brief: 通信数据帧
	@detail:STX	帧开始标志，2字节，取值为FFFFH；
			VER	协议版本号，当前版本为00H
			SEQ	帧序列号，1个字节；
				(1)  服务端的帧序列号的低半字节为1～9，高半字节为0；
				(2)  帧序列号每次自动加1，用于标识每一次的通信；
				(3)  服务端发送的帧序号为0XH，其中X为1,2,3,4,5,6,7,8,9；
				(4)  客户端发送的帧序号为X0H，其中X为1,2,3,4,5,6,7,8,9；
			LEN	DATA域的长度，4字节（VER= 0x00，高2字节保留，低两字节为DATA域长度）
			DATA	帧数据内容
			CRC	从VER到DATA所有字节的CRC16校验值，2字节，初始值为FFFFH
	@note: 以字节表示的数据，均为高字节在前、低位字节在后的大端模式；对于未定义或者保留的参数默认值填00H。
*/
struct Data_frame_Handrail_Ack {
	quint16 STX = 0XFFFF;
	quint8 VER = 0X00;
	quint8 SEQ;
	quint32 LEN;
	Handrail_Ack DATA;
	quint16 CRC = 0XFFFF;
};
struct Data_frame_Handrail_Request {
	quint16 STX = 0XFFFF;
	quint8 VER = 0X00;
	quint8 SEQ;
	quint32 LEN;
	Handrail_Request DATA;
	quint16 CRC = 0XFFFF;
};
struct Data_frame_Handrail_Ok {
	quint16 STX = 0XFFFF;
	quint8 VER = 0X00;
	quint8 SEQ;
	quint32 LEN;
	Handrail_OK  DATA;
	quint16 CRC = 0XFFFF;
};
struct Data_frame_VehicleInspection {
	quint16 STX = 0XFFFF;
	quint8 VER = 0X00;
	quint8 SEQ;
	quint32 LEN;
	VehicleInspection  DATA;
	quint16 CRC = 0XFFFF;
};
struct Data_frame_HeartBeat {
	quint16 STX = 0XFFFF;
	quint8 VER = 0X00;
	quint8 SEQ;
	quint32 LEN;
	HeartBeat  DATA;
	quint16 CRC = 0XFFFF;
};
#pragma pack()
