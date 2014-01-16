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
                for(;;) {
                    Socket socket = serverSocket.accept();
                    System.out.println("Got Connection");
                    PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                    BufferedReader in = new BufferedReader(
                            new InputStreamReader(socket.getInputStream()));
                    out.println("SwitchAppReady");
                    String resp = in.readLine();
                    System.out.println("Got Response : " + resp);

                    String cmd = in.readLine();
                    System.out.println("Got cmd: " + cmd);
                    if(cmd == "GET_NUM_DEVICES") {
                        out.println("4");
                    } else if(cmd == "GET_DEVICE_IDS") {
                        out.println("1_2_3_4");
                    } else if(cmd.startsWith("GET_DEVICE_INFO_")) {
                        if(cmd.endsWith("1")) {
                            String r = "Qeo Blaster Kinect_EP_SP_ES_SS_";
                            out.println(r);
                        } else if(cmd.endsWith("2")) {
                            String r = "ZWave light Bulb_EP_SP_ES_SS_";
                            out.println(r);
                        } else if(cmd.endsWith("3")) {
                            String r = "Other Device_EP_SP_ES_SS_";
                            out.println(r);
                        } else if(cmd.endsWith("4")) {
                            String r = "Standard Qeo Device_EP_SP_ES_SS_";
                            out.println(r);
                        }
                    } else if(cmd.startsWith("SET_MAP_")) {

                    } else if(cmd.startsWith("SET_UNMAP_")) {

                    } else if(cmd == "GET_ALL_MAP") {
                        StringBuilder r = new StringBuilder();
                        r.append("1_E_Button Down_2_E_LightOn");
                        r.append(",");
                        r.append("1_E_Button Up_2_E_LightOff");
                        r.append(",");
                        r.append("3_E_Leftcircle_2_E_LightOn");
                        r.append(",");
                        r.append("3_E_Rightcircle_2_E_LightOff");
                        out.println(r.toString());
                    } else if(cmd == "END_CONVERSATION") {
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
