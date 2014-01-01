package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.IsWidget;
import com.google.gwt.user.client.ui.SimplePanel;
import com.google.gwt.user.client.ui.Widget;
import com.sencha.gxt.widget.core.client.container.CenterLayoutContainer;
import com.sencha.gxt.widget.core.client.container.SimpleContainer;

/**
 * Created by christopher on 12/28/13.
 */
public class MainPage implements IsWidget {

    static MainPage instance = null;

    interface MainPageUiBinder extends UiBinder<Widget, MainPage> {
    }

    Widget root;

    private static MainPageUiBinder ourUiBinder = GWT.create(MainPageUiBinder.class);
    @UiField
    SimpleContainer center;

    @Override
    public Widget asWidget() {
        return root;
    }

    public SimpleContainer getCenter() {
        return center;
    }

    static public MainPage get() {
        if (instance == null) {
            instance = new MainPage();
        }
        return instance;
    }

    private MainPage() {
        root = ourUiBinder.createAndBindUi(this);
    }
}