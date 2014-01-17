package com.qeoblaster.webconfig.client;

import com.google.gwt.regexp.shared.RegExp;
import com.google.gwt.regexp.shared.SplitResult;

import java.io.Serializable;
import java.util.List;

/**
 * Created by chenc52 on 1/13/14.
 */
public class Trigger implements Serializable {

    private long id;
    private Signal inputSignal;
    private Signal outputSignal;

    private static long last_id = 0;

    public Trigger() {}

    public Trigger(String data, List<Device> devices) {
        RegExp reg = RegExp.compile("_");
        SplitResult res = reg.split(data);

        int inputDeviceId = Integer.valueOf(res.get(0));
        RegExp convertAmp = RegExp.compile("&");
        String inputSignalName = convertAmp.replace(res.get(2), "_");
        int outputDeviceId = Integer.valueOf(res.get(3));
        String outputSignalName = convertAmp.replace(res.get(5), "_");
        for(Device device : devices) {
            if(device.getDeviceID() == inputDeviceId) {
                for(Signal signal : device.getSupportedSignal()) {
                    if(signal.getType() == SignalType.OUTPUT_MSG && signal.getName().equals(inputSignalName)) {
                        inputSignal = signal;
                        break;
                    }
                }
            } else if(device.getDeviceID() == outputDeviceId) {
                for(Signal signal : device.getSupportedSignal()) {
                    if(signal.getType() == SignalType.INPUT_MSG && signal.getName().equals(outputSignalName)) {
                        outputSignal = signal;
                        break;
                    }
                }
            }

        }
        id = last_id++;
    }

    public Trigger(long id, Signal inputSignal, Signal outputSignal) {
        this.id = id;
        this.inputSignal = inputSignal;
        this.outputSignal = outputSignal;
    }

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public Signal getInputSignal() {
        return inputSignal;
    }

    public void setInputSignal(Signal inputSignal) {
        this.inputSignal = inputSignal;
    }

    public Signal getOutputSignal() {
        return outputSignal;
    }

    public void setOutputSignal(Signal outputSignal) {
        this.outputSignal = outputSignal;
    }

    public String getInputSignalName() {
        if(inputSignal == null) {
            return "";
        }
        return inputSignal.getName();
    }

    public void setInputSignalName(String inputSignalName) {
    }

    public String getOutputSignalName() {
        if(outputSignal == null) {
            return "";
        }
        return outputSignal.getName();
    }

    public void setOutputSignalName(String outputSignalName) {
    }

    public String getInputSignalValue() {
        return "";
    }

    public void setInputSignalValue(String inputSignalValue) {
    }

    public String getOutputSignalValue() {
        return "";
    }

    public void setOutputSignalValue(String outputSignalValue) {
    }
}
