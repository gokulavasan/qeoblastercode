package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.user.client.ui.IsWidget;
import com.google.gwt.user.client.ui.Widget;

/**
 * Created by christopher on 12/28/13.
 */
public class DevicesPage implements IsWidget {
    @Override
    public Widget asWidget() {
        return root;
    }

    private static DevicesPage instance = null;

    interface DevicesPageUiBinder extends UiBinder<Widget, DevicesPage> {
    }

    private static DevicesPageUiBinder ourUiBinder = GWT.create(DevicesPageUiBinder.class);

    private Widget root;

    static public DevicesPage get() {
        if(instance == null) {
            instance = new DevicesPage();
        }
        return instance;
    }

    private DevicesPage() {
        root = ourUiBinder.createAndBindUi(this);
    }
}