package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.event.logical.shared.ValueChangeEvent;
import com.google.gwt.event.logical.shared.ValueChangeHandler;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.IsWidget;
import com.google.gwt.user.client.ui.Widget;
import com.sencha.gxt.core.client.resources.ThemeStyles;
import com.sencha.gxt.core.client.util.ToggleGroup;
import com.sencha.gxt.widget.core.client.ContentPanel;
import com.sencha.gxt.widget.core.client.button.ToggleButton;
import com.sencha.gxt.widget.core.client.container.SimpleContainer;
import com.sencha.gxt.widget.core.client.event.SelectEvent;

/**
 * Created by christopher on 12/28/13.
 */
public class MainMenu implements IsWidget {

    interface MainMenuUiBinder extends UiBinder<Widget, MainMenu> {
    }

    @UiField(provided = true)
    ThemeStyles.Styles themeStyles = ThemeStyles.getStyle();
    @UiField
    ToggleButton devicesButton;
    @UiField
    ToggleButton triggersButton;
    @UiField
    ToggleButton settingsButton;
    @UiField
    SimpleContainer container;
    @UiField
    ContentPanel cp;

    private static MainMenuUiBinder ourUiBinder = GWT.create(MainMenuUiBinder.class);

    private Widget root;

    public MainMenu() {
        root = ourUiBinder.createAndBindUi(this);
        ToggleGroup group = new ToggleGroup();
        group.add(devicesButton);
        group.add(triggersButton);
        group.add(settingsButton);
        devicesButton.addValueChangeHandler(new ValueChangeHandler<Boolean>() {
            @Override
            public void onValueChange(ValueChangeEvent<Boolean> booleanValueChangeEvent) {
                if (booleanValueChangeEvent.getValue() == true) {
//                    cp.setHeadingText("Devices");
                    cp.setHeadingHtml("<div style=\"font-size: 18px; font-weight: bold\"> Devices</div>");
                    container.setWidget(DevicesPage.get());
                }
            }
        });
        triggersButton.addValueChangeHandler(new ValueChangeHandler<Boolean>() {
            @Override
            public void onValueChange(ValueChangeEvent<Boolean> booleanValueChangeEvent) {
                if (booleanValueChangeEvent.getValue() == true) {
//                    cp.setHeadingText("Triggers");
                    cp.setHeadingHtml("<div style=\"font-size: 18px; font-weight: bold\"> Triggers</div>");
                    container.setWidget(TriggersPage.get());
                }
            }
        });
        settingsButton.addValueChangeHandler(new ValueChangeHandler<Boolean>() {
            @Override
            public void onValueChange(ValueChangeEvent<Boolean> booleanValueChangeEvent) {
                if (booleanValueChangeEvent.getValue() == true) {
//                    cp.setHeadingText("Settings");
                    cp.setHeadingHtml("<div style=\"font-size: 18px; font-weight: bold;\"> Settings</div>");
                    container.setWidget(SettingsPage.get());
                }
            }
        });
        devicesButton.setValue(true);
        cp.setHeadingHtml("<div style=\"height 30px; font-size: 18px; font-weight: bold\"> Devices</div>");
        container.setWidget(DevicesPage.get());
    }

    @Override
    public Widget asWidget() {
        return root;
    }
}