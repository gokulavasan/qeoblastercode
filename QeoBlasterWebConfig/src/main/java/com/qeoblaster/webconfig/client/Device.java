package com.qeoblaster.webconfig.client;

/**
 * Created by chenc52 on 12/31/13.
 */
public class Device {
    private String deviceID;
    private String deviceName;
    private DeviceType type;

    public Device() {

    }

    public Device(String deviceID, String deviceName, DeviceType type) {
        this.deviceID = deviceID;
        this.deviceName = deviceName;
        this.type = type;
    }

    public String getDeviceID() {
        return deviceID;
    }

    public void setDeviceID(String deviceID) {
        this.deviceID = deviceID;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public DeviceType getType() {
        return type;
    }

    public void setType(DeviceType type) {
        this.type = type;
    }
}
