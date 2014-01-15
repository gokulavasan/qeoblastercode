package com.qeoblaster.webconfig.client;

import java.util.List;

/**
 * Created by christopher on 1/14/14.
 */
public class SignalsData {

    private static SignalsData instance = new SignalsData();

    public static SignalsData get() { return instance; }

    private List<Signal> signals;

    public void setSignals(List<Signal> signals) { this.signals = signals; }

    public List<Signal> getSignals() { return signals; }
}
