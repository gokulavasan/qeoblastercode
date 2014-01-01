package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.event.logical.shared.ValueChangeEvent;
import com.google.gwt.event.logical.shared.ValueChangeHandler;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.IsWidget;

import com.google.gwt.user.client.ui.Widget;
import com.google.gwt.user.client.ui.Label;
import com.sencha.gxt.cell.core.client.TextButtonCell;
import com.sencha.gxt.cell.core.client.form.ComboBoxCell;
import com.sencha.gxt.core.client.Style;
import com.sencha.gxt.core.client.ValueProvider;
import com.sencha.gxt.data.shared.LabelProvider;
import com.sencha.gxt.data.shared.ListStore;
import com.sencha.gxt.data.shared.ModelKeyProvider;
import com.sencha.gxt.data.shared.PropertyAccess;
import com.sencha.gxt.widget.core.client.Dialog;
import com.sencha.gxt.widget.core.client.ListView;

import com.sencha.gxt.widget.core.client.box.AlertMessageBox;
import com.sencha.gxt.widget.core.client.container.VerticalLayoutContainer;
import com.sencha.gxt.widget.core.client.button.TextButton;
import com.sencha.gxt.widget.core.client.event.SelectEvent;
import com.sencha.gxt.widget.core.client.form.ComboBox;
import com.sencha.gxt.widget.core.client.form.FieldLabel;
import com.sencha.gxt.widget.core.client.form.TextField;
import com.sencha.gxt.widget.core.client.selection.SelectionChangedEvent;
import com.sencha.gxt.widget.core.client.info.Info;

import java.util.Arrays;
import java.util.Date;
import java.util.List;

import static com.google.gwt.editor.client.Editor.Path;

/**
 * Created by christopher on 12/28/13.
 */
public class DevicesPage implements IsWidget {

    private ListView<Device, String> registeredDevices;
    private Label lblDeviceID;
    private Label lblDeviceName;
    private Label lblDeviceType;
    private ComboBox<DeviceTypeModel> cmbDeviceType;
    private TextField newDeviceName;

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

    interface DeviceProperties extends PropertyAccess<Device> {
        @Path("deviceID")
        ModelKeyProvider<Device> key();

        @Path("deviceName")
        ValueProvider<Device, String> name();

        @Path("type")
        ValueProvider<Device, DeviceType> type();
    }

    interface DeviceTypeModelProperties extends PropertyAccess<DeviceTypeModel> {
        @Path("key")
        ModelKeyProvider<DeviceTypeModel> key();

        @Path("name")
        LabelProvider<DeviceTypeModel> name();

    }

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
            newDevice.setDeviceID(new Date().getTime() + "");
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


        //TODO: change the cell type later
        TextButtonCell textButtonCell = new TextButtonCell();
        registeredDevices.getSelectionModel().setSelectionMode(Style.SelectionMode.SINGLE);
        registeredDevices.getSelectionModel().addSelectionChangedHandler(new SelectionChangedEvent.SelectionChangedHandler<Device>() {
            @Override
            public void onSelectionChanged(SelectionChangedEvent<Device> deviceSelectionChangedEvent) {
                List<Device> list = deviceSelectionChangedEvent.getSelection();
                if (list.size() > 0) {
                    showDeviceDetail(list.get(0));
                }
            }
        });

        registeredDevices.setCell(textButtonCell);
        registeredDevices.getStore().addAll(Arrays.asList(TestData.DEVICES));


        registeredDeviceListView.add(registeredDevices);
    }

    private void showDeviceDetail(Device device) {

        if (lblDeviceID == null) lblDeviceID = new Label();
        lblDeviceID.setText("ID: " + device.getDeviceID());

        if (lblDeviceName == null) lblDeviceName = new Label();
        lblDeviceName.setText("Name: " + device.getDeviceName());

        if (lblDeviceType == null) lblDeviceType = new Label();
        lblDeviceType.setText("Type: " + device.getType());

        deviceDetail.add(lblDeviceID);
        deviceDetail.add(lblDeviceName);
        deviceDetail.add(lblDeviceType);
    }

    private void clearDeviceDetail() {

        if (lblDeviceID != null)
            lblDeviceID.setText("");

        if (lblDeviceName != null)
            lblDeviceName.setText("");

        if (lblDeviceType != null)
            lblDeviceType.setText("");
    }


    class DeviceTypeModel {
        String key;
        String name;

        public String getKey() {
            return key;
        }

        public void setKey(String key) {
            this.key = key;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }
    }

}