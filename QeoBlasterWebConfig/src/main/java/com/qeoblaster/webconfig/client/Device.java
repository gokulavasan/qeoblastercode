package com.qeoblaster.webconfig.client;

import java.io.Serializable;
import java.util.Arrays;
import java.util.List;


/**
 * Created by chenc52 on 12/31/13.
 */
public class Device implements Serializable {
    private String deviceID;
    private String deviceName;
    private DeviceType type;
    private List<Signal> supportedSignal;

    public Device() {

    }

    public Device(String deviceID, String deviceName, DeviceType type, List<Signal> signals) {
        this.deviceID = deviceID;
        this.deviceName = deviceName;
        this.type = type;
        setSupportedSignal(signals);
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


    public List<Signal> getSupportedSignal() {
        return supportedSignal;
    }

    public void setSupportedSignal(List<Signal> supportedSignal) {
        this.supportedSignal = supportedSignal;
        if(supportedSignal != null) {
            for(Signal signal : supportedSignal) {
                signal.setOwner(this);
            }
        }
    }
}
