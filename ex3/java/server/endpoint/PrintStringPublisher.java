package server.endpoint;

import javax.xml.ws.Endpoint;
import server.ws.PrintStringImpl;

public class PrintStringImpl{
	public static void main(String[] args){
		final String url = "http://localhost:8080/ws/printString";
		System.out.println("Publishing PrintString service at endpoint: " + url);
		Endpoint.publish(url, new PrintStringImpl());
	}
}