package server.ws;

import javax.jws.WebService;

@WebService(endpointInterface = "server.ws.PrintString")
public class PrintStringImpl implements PrintString{
	public void printStr(String s){
		System.out.println(s);
	}
}