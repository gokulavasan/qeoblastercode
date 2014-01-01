package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.user.client.ui.IsWidget;
import com.google.gwt.user.client.ui.Widget;

/**
 * Created by christopher on 12/28/13.
 */
public class TriggersPage implements IsWidget {
    @Override
    public Widget asWidget() {
        return root;
    }

    interface TriggersPageUiBinder extends UiBinder<Widget, TriggersPage> {
    }

    private static TriggersPageUiBinder ourUiBinder = GWT.create(TriggersPageUiBinder.class);

    private static TriggersPage instance = null;

    public static TriggersPage get() {
        if (instance == null) {
            instance = new TriggersPage();
        }
        return instance;
    }

    private Widget root;

    public TriggersPage() {
        root = ourUiBinder.createAndBindUi(this);
    }
}