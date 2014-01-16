package com.qeoblaster.webconfig.server;

import com.google.gwt.regexp.shared.RegExp;
import com.google.gwt.regexp.shared.SplitResult;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;
import com.qeoblaster.webconfig.client.*;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by gammagec on 1/15/14.
 */
public class WebConfigServiceImpl extends RemoteServiceServlet implements WebConfigService {

    @Override
    public ServerData getServerData() {
        String hostName = "127.0.0.1";
        int portNumber = 8113;
        ServerData serverData = new ServerData();
        serverData.devices = new ArrayList<Device>();
        serverData.signals = new ArrayList<Signal>();
        serverData.triggers = new ArrayList<Trigger>();
        try {
            Socket socket = new Socket(hostName, portNumber);
            System.out.println("Connected To Socket Server");
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String line = in.readLine();
            System.out.println("read line: " + line);
            out.println("JavaReady");
            out.println("GET_NUM_DEVICES");
            int numDevices = Integer.valueOf(in.readLine());
            System.out.println("Client got " + numDevices + " number of devices");
            out.println("GET_DEVICE_IDS");
            String deviceIds = in.readLine();
            String[] idsStr = deviceIds.split("_");
            List<Integer> ids = new ArrayList<Integer>();
            for(String str : idsStr) {
                ids.add(Integer.valueOf(str));
            }
            for(Integer i : ids) {
                out.println("GET_DEVICE_INFO_" + i);
                Device d = new Device(i, in.readLine());
                serverData.devices.add(d);
                for(Signal s : d.getSupportedSignal()) {
                    serverData.signals.add(s);
                }
                System.out.println("Got device " + d);
            }
            out.println("GET_ALL_MAP");
            String triggers = in.readLine();
            RegExp reg = RegExp.compile(",");
            SplitResult trigs = reg.split(triggers);
            for(int i = 0; i < trigs.length(); i++) {
                String trigger = trigs.get(i);
                serverData.triggers.add(new Trigger(trigger, serverData.devices));
            }
            out.println("END_CONVERSATION");
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return serverData;
    }
}
