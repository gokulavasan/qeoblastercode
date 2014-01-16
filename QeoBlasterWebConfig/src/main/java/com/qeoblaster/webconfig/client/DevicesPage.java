package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.event.logical.shared.ValueChangeEvent;
import com.google.gwt.event.logical.shared.ValueChangeHandler;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.IsWidget;

import com.google.gwt.user.client.ui.Widget;
import com.google.gwt.user.client.ui.Label;
import com.sencha.gxt.cell.core.client.form.ComboBoxCell;
import com.sencha.gxt.core.client.Style;
import com.sencha.gxt.data.shared.ListStore;
import com.sencha.gxt.widget.core.client.Dialog;
import com.sencha.gxt.widget.core.client.ListView;

import com.sencha.gxt.widget.core.client.box.AlertMessageBox;
import com.sencha.gxt.widget.core.client.container.VerticalLayoutContainer;
import com.sencha.gxt.widget.core.client.button.TextButton;
import com.sencha.gxt.widget.core.client.event.SelectEvent;
import com.sencha.gxt.widget.core.client.form.ComboBox;
import com.sencha.gxt.widget.core.client.form.FieldLabel;
import com.sencha.gxt.widget.core.client.form.TextField;
import com.sencha.gxt.widget.core.client.grid.ColumnConfig;
import com.sencha.gxt.widget.core.client.grid.ColumnModel;
import com.sencha.gxt.widget.core.client.grid.Grid;
import com.sencha.gxt.widget.core.client.selection.SelectionChangedEvent;
import com.sencha.gxt.widget.core.client.info.Info;
import com.sencha.gxt.widget.core.client.toolbar.ToolBar;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.List;

/**
 * Created by christopher on 12/28/13.
 */
public class DevicesPage implements IsWidget {

    public static final String PLEASE_RECORD_IR_CMD = "Please \"Record\" IR CMD...";
    private ListView<Device, String> registeredDevices;
    private Label lblDeviceID;
    private Label lblDeviceName;
    private Label lblDeviceType;
    private ComboBox<DeviceTypeModel> cmbDeviceType;
    private TextField newDeviceName;
    private Label lblSignals;
    private Grid<Signal> signalTable;
    private ToolBar irToolbar;
    private TextField newIRSignalName;
    private TextField newIRSignalValue;
    private ComboBox<SignalTypeModel> cmbSignalType;

    @Override
    public Widget asWidget() {
        return root;
    }

    @UiField
    VerticalLayoutContainer deviceDetail;

    @UiField
    TextButton btnAddDevice;

    @UiField
    TextButton btnRemoveDevice;

    @UiField
    VerticalLayoutContainer registeredDeviceListView;


    private static DevicesPage instance = null;


    interface DevicesPageUiBinder extends UiBinder<Widget, DevicesPage> {
    }

    private static DevicesPageUiBinder ourUiBinder = GWT.create(DevicesPageUiBinder.class);

    private Widget root;

    static public DevicesPage get() {
        if (instance == null) {
            instance = new DevicesPage();
        }
        return instance;
    }

    private DevicesPage() {
        root = ourUiBinder.createAndBindUi(this);

        btnAddDevice.addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                showAddDeviceDialog();
            }
        });

        btnRemoveDevice.addSelectHandler(new SelectEvent.SelectHandler() {

            @Override
            public void onSelect(SelectEvent selectEvent) {
                List<Device> list = registeredDevices.getSelectionModel().getSelection();
                if (list != null && list.size() > 0) {
                    removeDevice(list.get(0));
                    clearDeviceDetail();
                }
            }
        });

        showRegisteredDevices();

    }

    private void showAddDeviceDialog() {

        final Dialog complex = new Dialog();
        complex.setBodyBorder(false);
        complex.setHeadingText("Add new Device...");
        complex.setWidth(400);
        complex.setHeight(225);
        //TODO: need to make this dialog blocking all other interactions

        VerticalLayoutContainer p = new VerticalLayoutContainer();
        complex.add(p);

        newDeviceName = new TextField();
        newDeviceName.setAllowBlank(false);
        newDeviceName.setEmptyText("Enter device name...");
        newDeviceName.addValueChangeHandler(new ValueChangeHandler<String>() {
            @Override
            public void onValueChange(ValueChangeEvent<String> event) {
                Info.display("Value Changed", "device name changed to " + event.getValue() == null ? "blank" : event.getValue());
            }
        });
        p.add(new FieldLabel(newDeviceName, "Device Name"), new VerticalLayoutContainer.VerticalLayoutData(1, -1));

        DeviceTypeModelProperties props = GWT.create(DeviceTypeModelProperties.class);
        ListStore<DeviceTypeModel> deviceTypeListStore = new ListStore<DeviceTypeModel>(props.key());
        DeviceType[] types = DeviceType.values();
        for (int i = 0; i < types.length; i++) {
            DeviceTypeModel model = new DeviceTypeModel();
            model.setKey(types[i].name());
            model.setName(types[i].name());
            deviceTypeListStore.add(model);
        }

        cmbDeviceType = new ComboBox<DeviceTypeModel>(deviceTypeListStore, props.name());
        cmbDeviceType.setTriggerAction(ComboBoxCell.TriggerAction.ALL);
        cmbDeviceType.setForceSelection(true);

        p.add(new FieldLabel(cmbDeviceType, "Device Type"), new VerticalLayoutContainer.VerticalLayoutData(-18, -1));
        complex.setPredefinedButtons(Dialog.PredefinedButton.OK, Dialog.PredefinedButton.CANCEL);
        complex.getButtonById(Dialog.PredefinedButton.OK.name()).addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                System.out.println("OK clicked");
                if (addNewDevice()) {
                    complex.hide();
                } else {
                    AlertMessageBox alertBox = new AlertMessageBox("Alert", "Failed to add new device");
                    alertBox.show();
                }


            }
        });

        complex.getButtonById(Dialog.PredefinedButton.CANCEL.name()).addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                System.out.println("CANCEL clicked");
                complex.hide();
            }
        });

        complex.show();
    }

    private boolean addNewDevice() {
        //TODO: save data back and do validation
        if (newDeviceName.getText() != null && newDeviceName.getText().length() > 0
                && cmbDeviceType.getText() != null && cmbDeviceType.getText().length() > 0) {
            Device newDevice = new Device();
            newDevice.setDeviceID((long)(Math.random() * Long.MAX_VALUE));
            newDevice.setDeviceName(newDeviceName.getText());
            newDevice.setType(DeviceType.valueOf(cmbDeviceType.getText()));


            registeredDevices.getStore().add(newDevice);
            return true;
        }

        return false;
    }


    private void removeDevice(Device device) {
        registeredDevices.getStore().remove(device);
    }

    private void showRegisteredDevices() {
        Label lblRegistered = new Label();
        lblRegistered.setText("Registered Devices:");
        registeredDeviceListView.add(lblRegistered);

        final DeviceProperties props = GWT.create(DeviceProperties.class);
        registeredDevices = new ListView<Device, String>(new ListStore<Device>(props.key()), props.name());
        registeredDevices.getSelectionModel().setSelectionMode(Style.SelectionMode.SINGLE);
        registeredDevices.getSelectionModel().addSelectionChangedHandler(new SelectionChangedEvent.SelectionChangedHandler<Device>() {
            @Override
            public void onSelectionChanged(SelectionChangedEvent<Device> deviceSelectionChangedEvent) {
                List<Device> list = deviceSelectionChangedEvent.getSelection();
                if (list.size() > 0) {
                    clearDeviceDetail();
                    showDeviceDetail(list.get(0));
                }
            }
        });
        registeredDevices.getStore().addAll(ServerData.data.devices);
        registeredDevices.setBorders(false);
        registeredDeviceListView.add(registeredDevices);
    }


    /**
     * show detail information for one device. User also configure signals here
     *
     * @param device
     */
    private void showDeviceDetail(final Device device) {


        // Create Labels
        if (lblDeviceID == null) lblDeviceID = new Label();
        lblDeviceID.setText("ID: " + device.getDeviceID());

        if (lblDeviceName == null) lblDeviceName = new Label();
        lblDeviceName.setText("Name: " + device.getDeviceName());

        if (lblDeviceType == null) lblDeviceType = new Label();
        lblDeviceType.setText("Type: " + device.getType());

        if (lblSignals == null) lblSignals = new Label();
        lblSignals.setText("Supported Signals: ");


        deviceDetail.add(lblDeviceID);
        deviceDetail.add(lblDeviceName);
        deviceDetail.add(lblDeviceType);
        deviceDetail.add(lblSignals);


        //show signal grid


        SignalProperties sp = GWT.create(SignalProperties.class);

        ColumnConfig<Signal, String> nameCol = new ColumnConfig<Signal, String>(sp.names(), 100, "Name");
        ColumnConfig<Signal, String> valueCol = new ColumnConfig<Signal, String>(sp.value(), 100, "Value");
        ColumnConfig<Signal, SignalType> typeCol = new ColumnConfig<Signal, SignalType>(sp.signalType(), 100, "Type");


        List<ColumnConfig<Signal, ?>> l = new ArrayList<ColumnConfig<Signal, ?>>();
        l.add(nameCol);
        l.add(valueCol);
        l.add(typeCol);

        ColumnModel<Signal> cm = new ColumnModel<Signal>(l);

        ListStore<Signal> store = new ListStore<Signal>(sp.id());
        if (device.getSupportedSignal() == null) {
            store.addAll(new ArrayList<Signal>());
        } else {
            store.addAll(device.getSupportedSignal());
        }


        signalTable = new Grid<Signal>(store, cm);
        signalTable.getView().setAutoExpandColumn(nameCol);
        signalTable.getView().setStripeRows(true);
        signalTable.getView().setColumnLines(true);
        signalTable.setBorders(false);
        signalTable.setColumnReordering(true);
        signalTable.setStateful(false);

        if (device.getType().equals(DeviceType.IR)) {
            irToolbar = new ToolBar();
            TextButton btnAddIRSignal = new TextButton();
            btnAddIRSignal.setText("Add IR CMD");
            btnAddIRSignal.addSelectHandler(new SelectEvent.SelectHandler() {
                @Override
                public void onSelect(SelectEvent selectEvent) {
                    showAddIRSignalDiaglog(device);
                }
            });


            TextButton btnRemoveIRSignal = new TextButton();
            btnRemoveIRSignal.setText("Remove IR CMD");
            btnRemoveIRSignal.addSelectHandler(new SelectEvent.SelectHandler() {

                @Override
                public void onSelect(SelectEvent selectEvent) {
                    List<Signal> list = signalTable.getSelectionModel().getSelection();
                    if (list != null && list.size() > 0) {
                        for (int i = 0; i < list.size(); i++) {
                            signalTable.getStore().remove(list.get(i));
                            //TODO: need to remove the signal from data store
                        }
                    }
                }
            });

            irToolbar.add(btnAddIRSignal);
            irToolbar.add(btnRemoveIRSignal);

            deviceDetail.add(irToolbar);
        }

        deviceDetail.add(signalTable, new VerticalLayoutContainer.VerticalLayoutData(1, 1));


    }

    private void showAddIRSignalDiaglog(final Device device) {

        final Dialog complex = new Dialog();
        complex.setBodyBorder(false);
        complex.setHeadingText("Add new IR CMD...");
        complex.setWidth(400);
        complex.setHeight(225);
        //TODO: need to make this dialog blocking all other interactions

        VerticalLayoutContainer p = new VerticalLayoutContainer();


        newIRSignalName = new TextField();
        newIRSignalName.setAllowBlank(false);
        newIRSignalName.setEmptyText("Enter IR CMD name...");


        newIRSignalValue = new TextField();
        newIRSignalValue.setAllowBlank(false);
        newIRSignalValue.setEnabled(false);
        newIRSignalValue.setEmptyText(PLEASE_RECORD_IR_CMD);


        SignalTypeModelProperties stmp = GWT.create(SignalTypeModelProperties.class);
        ListStore<SignalTypeModel> signalTypeListStore = new ListStore<SignalTypeModel>(stmp.key());
        SignalType[] types = SignalType.values();
        for (int i = 0; i < types.length; i++) {
            SignalTypeModel model = new SignalTypeModel();
            model.setKey(types[i].name());
            model.setName(types[i].name());
            signalTypeListStore.add(model);
        }

        cmbSignalType = new ComboBox<SignalTypeModel>(signalTypeListStore, stmp.name());
        cmbSignalType.setTriggerAction(ComboBoxCell.TriggerAction.ALL);
        cmbSignalType.setForceSelection(true);


        ToolBar toolBar = new ToolBar();

        TextButton btnStart = new TextButton();
        btnStart.setText("Record");
        btnStart.addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                newIRSignalValue.setText("newIRCommand Recorded" + Math.random());
                //TODO: replace with the actual command recorded from service

            }
        });
        toolBar.add(btnStart);


        complex.add(p);
        p.add(toolBar);
        p.add(new FieldLabel(newIRSignalName, "IR CMD Name"), new VerticalLayoutContainer.VerticalLayoutData(1, -1));
        p.add(new FieldLabel(cmbSignalType, "IR CMD Type"), new VerticalLayoutContainer.VerticalLayoutData(1, -1));
        p.add(new FieldLabel(newIRSignalValue, "IR CMD Value"), new VerticalLayoutContainer.VerticalLayoutData(1, -1));


        complex.setPredefinedButtons(Dialog.PredefinedButton.OK, Dialog.PredefinedButton.CANCEL);
        complex.getButtonById(Dialog.PredefinedButton.OK.name()).addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                if (addNewIRCMD(device)) {
                    complex.hide();
                } else {
                    AlertMessageBox alertBox = new AlertMessageBox("Alert", "Failed to add new IR CMD");
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

    private boolean addNewIRCMD(Device device) {
        if (newIRSignalName != null && newIRSignalName.getText() != null && newIRSignalName.getText().length() > 0
                && newIRSignalValue != null && newIRSignalValue.getText() != null
                && newIRSignalValue.getText().length() > 0 && !(newIRSignalValue.getText().equals(PLEASE_RECORD_IR_CMD))
                && cmbSignalType != null && cmbSignalType.getText() != null && cmbSignalType.getText().length() > 0) {


            Signal irSignal = new Signal(new Date().getTime(), newIRSignalName.getText()
                    , newIRSignalValue.getText(), SignalType.valueOf(cmbSignalType.getText()), device);

            signalTable.getStore().add(irSignal);

            return true;
        }
        return false;
    }


    private void clearDeviceDetail() {


        if (lblDeviceID != null)
            lblDeviceID.setText("");

        if (lblDeviceName != null)
            lblDeviceName.setText("");

        if (lblDeviceType != null)
            lblDeviceType.setText("");

        if (lblSignals != null) {
            lblSignals.setText("");
        }

        if (irToolbar != null) {
            irToolbar.removeFromParent();
        }

        if (signalTable != null) {
            signalTable.removeFromParent();
        }
    }


}