package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.user.client.ui.IsWidget;
import com.google.gwt.user.client.ui.Widget;

/**
 * Created by christopher on 12/28/13.
 */
public class SettingsPage implements IsWidget {
    @Override
    public Widget asWidget() {
        return root;
    }

    interface SettingsPageUiBinder extends UiBinder<Widget, SettingsPage> {
    }

    private static SettingsPageUiBinder ourUiBinder = GWT.create(SettingsPageUiBinder.class);
    private Widget root;

    private static SettingsPage instance = null;

    public static SettingsPage get() {
        if (instance == null) {
            instance = new SettingsPage();
        }
        return instance;
    }

    private SettingsPage() {
        root = ourUiBinder.createAndBindUi(this);
    }
}