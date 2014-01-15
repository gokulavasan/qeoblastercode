package com.qeoblaster.webconfig.client;

import java.io.Serializable;

/**
 * Created by chenc52 on 1/13/14.
 */
public class Trigger implements Serializable {

    private long id;
    private Signal inputSignal;
    private Signal outputSignal;
    private String inputSignalName;
    private String inputSignalValue;
    private String outputSignalName;
    private String outputSignalValue;


    public Trigger() {}

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
        if (inputSignal == null) {
            return "";
        }
        return inputSignal.getName();
    }

    public void setInputSignalName(String inputSignalName) {
        if (inputSignal != null)
            inputSignal.setName(inputSignalName);
    }

    public String getInputSignalValue() {
        if (inputSignal == null) {
            return "";
        }
        return inputSignal.getValue();
    }

    public void setInputSignalValue(String inputSignalValue) {
        if (inputSignal != null)
            inputSignal.setValue(inputSignalValue);
    }

    public String getOutputSignalName() {
        if (outputSignal == null) {
            return "";
        }
        return outputSignal.getName();
    }

    public void setOutputSignalName(String outputSignalName) {
        if (outputSignal != null)
            outputSignal.setName(outputSignalName);
    }

    public String getOutputSignalValue() {
        if (outputSignal == null) {
            return "";
        }
        return outputSignal.getValue();
    }

    public void setOutputSignalValue(String outputSignalValue) {
        if (outputSignal != null)
            outputSignal.setValue(outputSignalValue);
    }
}
