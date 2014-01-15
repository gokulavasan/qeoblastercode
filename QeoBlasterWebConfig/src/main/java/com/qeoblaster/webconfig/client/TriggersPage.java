package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.editor.client.Editor;
import com.google.gwt.event.logical.shared.SelectionEvent;
import com.google.gwt.event.logical.shared.SelectionHandler;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiFactory;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.IsWidget;
import com.google.gwt.user.client.ui.Widget;
import com.sencha.gxt.cell.core.client.form.ComboBoxCell;
import com.sencha.gxt.core.client.Style;
import com.sencha.gxt.core.client.ValueProvider;
import com.sencha.gxt.data.shared.ListStore;
import com.sencha.gxt.data.shared.ModelKeyProvider;
import com.sencha.gxt.data.shared.PropertyAccess;
import com.sencha.gxt.state.client.GridStateHandler;
import com.sencha.gxt.widget.core.client.Dialog;
import com.sencha.gxt.widget.core.client.box.AlertMessageBox;
import com.sencha.gxt.widget.core.client.button.TextButton;
import com.sencha.gxt.widget.core.client.container.VerticalLayoutContainer;
import com.sencha.gxt.widget.core.client.event.SelectEvent;
import com.sencha.gxt.widget.core.client.form.ComboBox;
import com.sencha.gxt.widget.core.client.form.FieldLabel;
import com.sencha.gxt.widget.core.client.grid.ColumnConfig;
import com.sencha.gxt.widget.core.client.grid.ColumnModel;
import com.sencha.gxt.widget.core.client.grid.Grid;
import com.sencha.gxt.widget.core.client.grid.GridView;
import com.sencha.gxt.widget.core.client.toolbar.ToolBar;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.List;

/**
 * Created by christopher on 12/28/13.
 */
public class TriggersPage implements IsWidget {

    private ComboBox<Signal> inputSignal;
    private ComboBox<Signal> outputSignal;
    private ComboBox<Device> outputDevice;
    private ComboBox<Device> inputDevice;

    @Override
    public Widget asWidget() {
        return root;
    }

    interface TriggersPageUiBinder extends UiBinder<Widget, TriggersPage> {
    }

    interface TriggerProperties extends PropertyAccess<Trigger> {
        @Editor.Path("id")
        ModelKeyProvider<Trigger> id();


        @Editor.Path("inputSignalName")
        ValueProvider<Trigger, String> inputSignalName();


        @Editor.Path("inputSignalValue")
        ValueProvider<Trigger, String> inputSignalValue();

        @Editor.Path("outputSignalName")
        ValueProvider<Trigger, String> outputSignalName();


        @Editor.Path("outputSignalValue")
        ValueProvider<Trigger, String> outputSignalValue();

        @Editor.Path("inputSignal")
        ValueProvider<Trigger, Signal> inputSignal();

        @Editor.Path("outputSignal")
        ValueProvider<Trigger, Signal> outputSignal();
    }


    @UiField
    VerticalLayoutContainer gridPanel;

    ColumnModel<Trigger> cm;


    ListStore<Trigger> store;


    Grid<Trigger> grid;

    @UiField
    TextButton btnAddTrigger;

    @UiField
    TextButton btnRemoveTrigger;

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


        TriggerProperties props = GWT.create(TriggerProperties.class);

        ColumnConfig<Trigger, String> inputCmdName = new ColumnConfig<Trigger, String>(props.inputSignalName(), 150, "Input Command");
        ColumnConfig<Trigger, String> inputCmdValue = new ColumnConfig<Trigger, String>(props.inputSignalValue(), 150, "Input Value");
        ColumnConfig<Trigger, String> outputCmdName = new ColumnConfig<Trigger, String>(props.outputSignalName(), 150, "Output Command");
        ColumnConfig<Trigger, String> outputCmdValue = new ColumnConfig<Trigger, String>(props.outputSignalValue(), 150, "Output Value");

        List<ColumnConfig<Trigger, ?>> l = new ArrayList<ColumnConfig<Trigger, ?>>();
        l.add(inputCmdName);
        l.add(inputCmdValue);
        l.add(outputCmdName);
        l.add(outputCmdValue);

        cm = new ColumnModel<Trigger>(l);
        store = new ListStore<Trigger>(props.id());

        store.addAll(ServerData.data.triggers);

        grid = new Grid<Trigger>(store, cm);
        grid.getView().setAutoExpandColumn(inputCmdName);
        grid.getView().setStripeRows(true);
        grid.getView().setColumnLines(true);
        grid.getSelectionModel().setSelectionMode(Style.SelectionMode.MULTI);
        grid.setBorders(false);

        grid.setColumnReordering(true);
        grid.setStateful(false);


        gridPanel.add(grid);


        btnRemoveTrigger.addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                List<Trigger> list = grid.getSelectionModel().getSelection();
                if (list != null && list.size() > 0) {
                    for (int i = 0; i < list.size(); i++) {
                        grid.getStore().remove(list.get(i));
                        //TODO: add code to remove data from storage as well
                    }
                }
            }
        });


        btnAddTrigger.addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                showAddTriggerDialog();
            }
        });

    }

    private void showAddTriggerDialog() {
        final Dialog complex = new Dialog();
        complex.setBodyBorder(false);
        complex.setHeadingText("Add new Trigger...");
        complex.setWidth(400);
        complex.setHeight(225);
        //TODO: need to make this dialog blocking all other interactions

        VerticalLayoutContainer p = new VerticalLayoutContainer();
        complex.add(p);


        //create device list store
        DeviceProperties deviceProps = GWT.create(DeviceProperties.class);
        ListStore<Device> deviceStore = new ListStore<Device>(deviceProps.key());
        deviceStore.addAll(ServerData.data.devices);

        SignalProperties inputSignalProps = GWT.create(SignalProperties.class);
        final ListStore<Signal> inputSignalStore = new ListStore<Signal>(inputSignalProps.id());
        SignalProperties outputSignalProps = GWT.create(SignalProperties.class);
        final ListStore<Signal> outputSignalStore = new ListStore<Signal>(outputSignalProps.id());


        //create device combo box
        inputDevice = new ComboBox<Device>(deviceStore, deviceProps.names());
        inputDevice.setAllowBlank(false);
        inputDevice.setForceSelection(true);
        inputDevice.setTriggerAction(ComboBoxCell.TriggerAction.ALL);
        p.add(new FieldLabel(inputDevice, "Input Device"), new VerticalLayoutContainer.VerticalLayoutData(1, -1));


        //create input signal combo box
        inputSignal = new ComboBox<Signal>(inputSignalStore, inputSignalProps.nameLabel());
        inputSignal.setAllowBlank(false);
        inputSignal.setForceSelection(true);
        inputSignal.setTriggerAction(ComboBoxCell.TriggerAction.ALL);
        p.add(new FieldLabel(inputSignal, "Input Signal"), new VerticalLayoutContainer.VerticalLayoutData(1, -1));


        inputDevice.addSelectionHandler(new SelectionHandler<Device>() {
            @Override
            public void onSelection(SelectionEvent<Device> deviceSelectionEvent) {
                if (inputDevice.getText() != null && inputDevice.getText().length() > 0) {
                    inputSignal.getStore().clear();
                    for(Signal signal : SignalsData.get().getSignals()) {
                    //for (int i = 0; i < SignalsData.get().getSignals().size(); i++) {
                        if (inputDevice.getCurrentValue().getDeviceID().equals(signal.getOwner().getDeviceID())) {
//                                && TestData.SIGNALS[i].getType().equals(SignalType.INPUT_MSG)) {
                            inputSignal.getStore().add(signal);
                        }
                    }

                }
            }
        });


        outputDevice = new ComboBox<Device>(deviceStore, deviceProps.names());
        outputDevice.setAllowBlank(false);
        outputDevice.setForceSelection(true);
        outputDevice.setTriggerAction(ComboBoxCell.TriggerAction.ALL);
        p.add(new FieldLabel(outputDevice, "Output Device"), new VerticalLayoutContainer.VerticalLayoutData(1, -1));


        outputSignal = new ComboBox<Signal>(outputSignalStore, outputSignalProps.nameLabel());
        outputSignal.setAllowBlank(false);
        outputSignal.setForceSelection(true);
        outputSignal.setTriggerAction(ComboBoxCell.TriggerAction.ALL);
        p.add(new FieldLabel(outputSignal, "Output Signal"), new VerticalLayoutContainer.VerticalLayoutData(1, -1));


        outputDevice.addSelectionHandler(new SelectionHandler<Device>() {
            @Override
            public void onSelection(SelectionEvent<Device> deviceSelectionEvent) {
                if (outputDevice.getText() != null && outputDevice.getText().length() > 0) {
                    outputSignalStore.clear();
                    for(Signal signal : SignalsData.get().getSignals()) {
                        if (outputDevice.getCurrentValue().getDeviceID().equals(signal.getOwner().getDeviceID())) {
                            outputSignal.getStore().add(signal);
                        }
                    }
                }
            }
        });

        complex.setPredefinedButtons(Dialog.PredefinedButton.OK, Dialog.PredefinedButton.CANCEL);
        complex.getButtonById(Dialog.PredefinedButton.OK.name()).addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                if (addNewTrigger()) {
                    complex.hide();
                } else {
                    AlertMessageBox alertBox = new AlertMessageBox("Alert", "Failed to add new Trigger");
                    alertBox.show();
                }


            }
        });

        complex.getButtonById(Dialog.PredefinedButton.CANCEL.name()).addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                complex.hide();
            }
        });

        complex.show();


    }

    private boolean addNewTrigger() {
        if (inputDevice.getText() != null && inputDevice.getText().length() > 0
                && inputSignal.getText() != null && inputSignal.getText().length() > 0
                && outputDevice.getText() != null && outputDevice.getText().length() > 0
                && outputSignal.getText() != null && outputSignal.getText().length() > 0) {
            Trigger trigger = new Trigger(new Date().getTime(), inputSignal.getCurrentValue(), outputSignal.getCurrentValue());
            grid.getStore().add(trigger);
            return true;

        }
        return false;
    }
}