import java.io.*;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * Created by gammagec on 1/15/14.
 */
public class FakeServer {
    static public void main(String[] args) {
        System.out.println("FakeServer 2.0");

        for(;;) {
            try {
                ServerSocket serverSocket = new ServerSocket(8113);
                Socket socket = serverSocket.accept();
                System.out.println("Got Connection");
                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                BufferedReader in = new BufferedReader(
                        new InputStreamReader(socket.getInputStream()));
                out.println("SwitchAppReady");
                String resp = in.readLine();
                System.out.println("Got Response : " + resp);
                for(;;) {
                    String cmd = in.readLine();
                    System.out.println("Got cmd: " + cmd);
                    if(cmd.equals("GET_NUM_DEVICES")) {
                        out.println("4");
                    } else if(cmd.equals("GET_DEVICE_IDS")) {
                        out.println("1_2_3_4");
                    } else if(cmd.startsWith("GET_DEVICE_INFO_")) {
                        if(cmd.endsWith("1")) {
                            String r = "Qeo Blaster Kinect_EP_LEFTCIRCLE:RIGHTCIRCLE_SP_ES_SS_";
                            out.println(r);
                        } else if(cmd.endsWith("2")) {
                            String r = "ZWave light Bulb_EP_SP_ES_ON:OFF_SS_";
                            out.println(r);
                        } else if(cmd.endsWith("3")) {
                            String r = "ZigBee Switch_EP_DOWN:UP_SP_ES_SS_";
                            out.println(r);
                        } else if(cmd.endsWith("4")) {
                            String r = "DVD Player IR_EP_SP_ES_PLAY:PAUSE_SS_";
                            out.println(r);
                        }
                    } else if(cmd.startsWith("SET_MAP_")) {

                    } else if(cmd.startsWith("SET_UNMAP_")) {

                    } else if(cmd.equals("GET_ALL_MAP")) {
                        StringBuilder r = new StringBuilder();
                        r.append("3_E_DOWN_2_E_ON");
                        r.append(",");
                        r.append("3_E_UP_2_E_OFF");
                        r.append(",");
                        r.append("1_E_LEFTCIRCLE_4_E_PLAY");
                        r.append(",");
                        r.append("1_E_RIGHTCIRCLE_4_E_PAUSE");
                        out.println(r.toString());
                    } else if(cmd.equals("END_CONVERSATION")) {
                        break;
                    }
                }
            } catch(UnknownHostException e) {
                e.printStackTrace();
            } catch(IOException e) {
                e.printStackTrace();
            }
        }
    }

}
