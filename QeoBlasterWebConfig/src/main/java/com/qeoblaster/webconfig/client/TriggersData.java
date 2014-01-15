package com.qeoblaster.webconfig.client;

import java.util.List;

/**
 * Created by christopher on 1/14/14.
 */
public class TriggersData {

    private static TriggersData instance = new TriggersData();

    public static TriggersData get() { return instance; }

    private List<Trigger> triggers;

    public void setTriggers(List<Trigger> triggers) { this.triggers = triggers; }

    public List<Trigger> getTriggers() { return triggers; }
}
