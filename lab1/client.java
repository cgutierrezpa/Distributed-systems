import java.io.*;
import gnu.getopt.Getopt;
import java.net.Socket;
import java.net.ServerSocket;


class client {
	
	/********************* TYPES **********************/
	
	/**
	 * @brief Return codes for the protocol methods
	 */
	private static enum RC {
		OK,
		ERROR,
		USER_ERROR
	};
	
	/******************* ATTRIBUTES *******************/
	
	private static String _server   = null;
	private static int _port = -1;
	/* Controls the user bound to the client when executing a CONNECT command */
	private static String connected_user = null;
	/* Instantiate and prepare an empty ServerThread for further connection */
	private static ServerThread server_thread = new ServerThread();
		
	
	/********************* METHODS ********************/
	
	/**
	 * @param user - User name to register in the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user is already registered
	 * @return ERROR if another error occurred
	 */
	static RC register(String user) 
	{
		// Write your code here
		///////////////////////////////////////////////
		///////////////     PROTOCOL    ///////////////
		///////////////////////////////////////////////
		try{
			//1. Connect to the server, using the IP and port passed in the command line
			Socket sc = new Socket(_server, _port);

			DataOutputStream out = new DataOutputStream(sc.getOutputStream());
			DataInputStream in = new DataInputStream(sc.getInputStream());

			//2. The string "REGISTER" is sent indicating the operation
			String operation = new String("REGISTER");
			out.writeBytes(operation);
			out.write('\0');			//Insert ASCII 0 at the end

			//3. A string of characters is sent with the user to be registered
			out.writeBytes(user);
			out.write('\0');

			//4. Check response from the server. If 0, success; if 1 user is previously registered; if 2 other case
			byte response = in.readByte();

			System.out.println("Response is: " + response);

			//5. Close connection
			sc.close();
			out.close();
			in.close();
			
			//Decode the response from the server
			switch(response){
				case 0:
					System.out.println("c> REGISTER OK");
					return RC.OK;
				case 1:
					System.out.println("c> USERNAME IN USE");
					return RC.USER_ERROR;
				case 2:
					System.out.println("c> REGISTER FAIL");
					return RC.ERROR;
			}

		}
		catch (java.io.IOException e) {
			System.out.println("Exception: " + e);
			e.printStackTrace();
		}
		System.out.println("c> REGISTER FAIL");
		return RC.ERROR;
	}
	
	/**
	 * @param user - User name to unregister from the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist
	 * @return ERROR if another error occurred
	 */
	static RC unregister(String user) 
	{
		///////////////////////////////////////////////
		///////////////     PROTOCOL    ///////////////
		///////////////////////////////////////////////
		try{
			//1. Connect to the server, using the IP and port passed in the command line
			Socket sc = new Socket(_server, _port);

			DataOutputStream out = new DataOutputStream(sc.getOutputStream());
			DataInputStream in = new DataInputStream(sc.getInputStream());

			//2. The string "UNREGISTER" is sent indicating the operation
			String operation = new String("UNREGISTER");
			out.writeBytes(operation);
			out.write('\0');			//Insert ASCII 0 at the end

			//3. A string of characters is sent with the user to be unregistered
			out.writeBytes(user);
			out.write('\0');

			//4. Check response from the server. If 0, success; if 1 user does not exist; if 2 other case
			byte response = in.readByte();

			System.out.println("Response is: " + response);

			//5. Close connection
			sc.close();
			out.close();
			in.close();
			
			//Decode the response from the server
			switch(response){
				case 0:
					/* If we try to unregister the user that is currently bound and connected
					to the client, it is unbound from the client */
					if(connected_user != null){
						/* This protects against null pointer exception when a CONNECT command is executed and
						the server marks the user as connected, but then the client is terminated without 
						executing DISCONNECT from the server */
						if(connected_user.equals(user)){
							connected_user = null;
							/* If the unregister also disconnect a user linked to the client (connected and with a 
							server thread running, kill also the running thread*/
							server_thread.kill();
						}
					}
					
					System.out.println("c> UNREGISTER OK");
					return RC.OK;
				case 1:
					System.out.println("c> USER DOES NOT EXIST");
					return RC.USER_ERROR;
				case 2:
					System.out.println("c> UNREGISTER FAIL");
					return RC.ERROR;
			}

		}
		catch (java.io.IOException e) {
			System.out.println("Exception: " + e);
			e.printStackTrace();
		}
		System.out.println("c> UNREGISTER FAIL");
		return RC.ERROR;
	}
	
    /**
	 * @param user - User name to connect to the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist or if it is already connected
	 * @return ERROR if another error occurred
	 */
	static RC connect(String user) 
	{	

		///////////////////////////////////////////////
		///////////////     PROTOCOL    ///////////////
		///////////////////////////////////////////////
		try{
			/* Before trying to connect, if a user is already connected, quit the function with RC.ERROR */
			if(connected_user != null){
				System.out.println("c> CONNECT FAIL");
				return RC.ERROR;
			}

			//1. Connect to the server, using the IP and port passed in the command line
			Socket sc = new Socket(_server, _port);

			DataOutputStream out = new DataOutputStream(sc.getOutputStream());
			DataInputStream in = new DataInputStream(sc.getInputStream());

			//2. The string "CONNECT" is sent indicating the operation
			String operation = new String("CONNECT");
			out.writeBytes(operation);
			out.write('\0');			//Insert ASCII 0 at the end

			//3. A string of characters is sent with the user to be connected
			out.writeBytes(user);
			out.write('\0');

			/* Create ServerSocket. We provide 0 to assign any available port number and 10 as maximum
			number of queued requests */
			ServerSocket sock = new ServerSocket(0, 10);
			/* Get the port at which the socket is listening */
			int port = sock.getLocalPort();

			System.out.println("Asigned port number: " + port);

			//4. A string is sent with the port number listening in the client
			out.writeBytes(String.valueOf(port));
			out.write('\0');

			//5. Check response from the server. If 0, success; if 1 user does not exist; if 2 other case
			byte response = in.readByte();

			System.out.println("Response is: " + response);

			//6. Close connection
			sc.close();
			out.close();
			in.close();
			
			//Decode the response from the server
			switch(response){
				case 0:
					/* Start a new thread where */
					server_thread.start(sock);
					//server_thread = new ServerThread(sock).start();
					/* Bind the user to the client */
					connected_user = user;
					System.out.println("c> CONNECT OK");
					return RC.OK;
				case 1:
					System.out.println("c> CONNECT FAIL, USER DOES NOT EXIST");
					return RC.USER_ERROR;
				case 2:
					System.out.println("c> USER ALREADY CONNECTED");
					return RC.USER_ERROR;
				case 3:
					System.out.println("c> CONNECT FAIL");
					return RC.ERROR;
			}

		}
		catch (java.io.IOException e) {
			System.out.println("Exception: " + e);
			e.printStackTrace();
		}
		System.out.println("c> CONNECT FAIL");
		return RC.ERROR;
	}
	
	 /**
	 * @param user - User name to disconnect from the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist or if it is already disconnected
	 * @return ERROR if another error occurred
	 */
	static RC disconnect(String user) 
	{
		///////////////////////////////////////////////
		///////////////     PROTOCOL    ///////////////
		///////////////////////////////////////////////
		try{
			/* Check if is trying to disconnect a user that is not connected. Exit with RC.ERROR if it does */
			if(connected_user != null){
				/* This protects against null pointer exception when a CONNECT command is executed and
					the server marks the user as connected, but then the client is terminated without 
					executing DISCONNECT from the server */
				if(!connected_user.equals(user)){
					System.out.println("c> DISCONNECT FAIL");
					return RC.ERROR;
				}
			}

			//1. Connect to the server, using the IP and port passed in the command line
			Socket sc = new Socket(_server, _port);

			DataOutputStream out = new DataOutputStream(sc.getOutputStream());
			DataInputStream in = new DataInputStream(sc.getInputStream());

			//2. The string "DISCONNECT" is sent indicating the operation
			String operation = new String("DISCONNECT");
			out.writeBytes(operation);
			out.write('\0');			//Insert ASCII 0 at the end

			//3. A string of characters is sent with the user to be disconnected
			out.writeBytes(user);
			out.write('\0');

			//4. Check response from the server. If 0, success; if 1 user does not exist; if 2 other case
			byte response = in.readByte();

			System.out.println("Response is: " + response);

			//5. Close connection
			sc.close();
			out.close();
			in.close();
			
			//Decode the response from the server
			switch(response){
				case 0:
					/* Unbind the user from the client. We need to check if the  */
					if(connected_user != null) connected_user = null;
					server_thread.kill();
					System.out.println("c> DISCONNECT OK");
					return RC.OK;
				case 1:
					System.out.println("c> DISCONNECT FAIL / USER DOES NOT EXIST");
					return RC.USER_ERROR;
				case 2:
					System.out.println("c> DISCONNECT FAIL / USER NOT CONNECTED");
					return RC.USER_ERROR;
				case 3:
					System.out.println("c> DISCONNECT FAIL");
					return RC.ERROR;
			}

		}
		catch (java.io.IOException e) {
			System.out.println("Exception: " + e);
			e.printStackTrace();
		}
		System.out.println("c> DISCONNECT FAIL");
		return RC.ERROR;
	}

	 /**
	 * @param user    - Receiver user name
	 * @param message - Message to be sent
	 * 
	 * @return OK if the server had successfully delivered the message
	 * @return USER_ERROR if the user is not connected (the message is queued for delivery)
	 * @return ERROR the user does not exist or another error occurred
	 */
	static RC send(String user, String message) 
	{
		///////////////////////////////////////////////
		///////////////     PROTOCOL    ///////////////
		///////////////////////////////////////////////
		try{
			/* If there is not a user connected in the client, return error RC.ERROR */
			if(connected_user == null){
				System.out.println("c> SEND FAIL");
				return RC.ERROR;
			}
			//1. Connect to the server, using the IP and port passed in the command line
			Socket sc = new Socket(_server, _port);

			DataOutputStream out = new DataOutputStream(sc.getOutputStream());
			DataInputStream in = new DataInputStream(sc.getInputStream());


			//2. The string "SEND" is sent indicating the operation
			String operation = new String("SEND");
			out.writeBytes(operation);
			out.write('\0');			//Insert ASCII 0 at the end

			//3. A string of characters is sent with the user that sends the message
			out.writeBytes(connected_user);
			out.write('\0');

			//4. A string of characters is sent with the user that receives the message
			System.out.println("Usuario es: " + user);
			out.writeBytes(user);
			out.write('\0');

			//5. A string of maximum 256 (including ASCII 0) characters is sent with the message to be sent
			
			System.out.println("Mensaje es: " + message);
			System.out.println("Mensaje cortado es: " + trimMessage(message));
			out.writeBytes(trimMessage(message)); //Sends a string of 255 characters
			out.write('\0');

			//6. Check response from the server. If 0, success; if 1 user does not exist; if 2 other case
			byte response = in.readByte();

			System.out.println("Response is: " + response);

			String msg_id = new String();
			/* If response is 0 (OK), prepare to read the ID of the message */
			if(response == 0){
				
				/* Create  BufferedReader for easy reading a string */
				/*
				BufferedReader inString = new BufferedReader(new InputStreamReader(sc.getInputStream()));
				msg_id = inString.readLine();*/
				byte ch;

				while ((ch = in.readByte()) != 0){
					msg_id = msg_id + ((char)ch);
				}
			}

			//7. Close connection
			sc.close();
			out.close();
			in.close();

			//Decode the response from the server
			switch(response){
				case 0:
					System.out.println("c> SEND OK - MESSAGE " + msg_id);
					return RC.OK;
				case 1:
					System.out.println("c> SEND FAIL / USER DOES NOT EXIST");
					return RC.USER_ERROR;
				case 2:
					System.out.println("c> SEND FAIL");
					return RC.ERROR;
			}

		}
		catch (java.io.IOException e) {
			System.out.println("Exception: " + e);
			e.printStackTrace();
		}
		System.out.println("c> SEND FAIL");
		return RC.ERROR;
	}

	static String trimMessage(String message){
		/* Maximum length is of 255 characters because 1 character is reserved for ASCII 0 */
		int maxLength = 255;

		if(message.length() > maxLength){
			message = message.substring(0, maxLength);
		}

		return message;
	}
	
	/**
	 * @brief Command interpreter for the client. It calls the protocol functions.
	 */
	static void shell() 
	{
		boolean exit = false;
		String input;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** REGISTER *************/
					if (line[0].equals("REGISTER")) {
						if  (line.length == 2) {
							register(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: REGISTER <userName>");
						}
					} 
					
					/********** UNREGISTER ************/
					else if (line[0].equals("UNREGISTER")) {
						if  (line.length == 2) {
							unregister(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNREGISTER <userName>");
						}
                    } 
                    
                    /************ CONNECT *************/
                    else if (line[0].equals("CONNECT")) {
						if  (line.length == 2) {
							connect(line[1]); // userName = line[1] AQUI CREAMOS EL HILO SERVIDOR
						} else {
							System.out.println("Syntax error. Usage: CONNECT <userName>");
                    	}
                    } 
                    
                    /********** DISCONNECT ************/
                    else if (line[0].equals("DISCONNECT")) {
						if  (line.length == 2) {
							disconnect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: DISCONNECT <userName>");
                    	}
                    } 
                    
                    /************** SEND **************/
                    else if (line[0].equals("SEND")) {
						if  (line.length >= 3) {
							// Remove first two words
							String message = input.substring(input.indexOf(' ')+1);
							message = message.substring(message.indexOf(' ')+1);
							send(line[1], message); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: SEND <userName> <message>");
                    	}
                    } 
                    
                    /************** QUIT **************/
                    else if (line[0].equals("QUIT")){
						if (line.length == 1) {
							exit = true;
						} else {
							System.out.println("Syntax error. Use: QUIT");
						}
					} 
					
					/************* UNKNOWN ************/
					else {						
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}				
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * @brief Prints program usage
	 */
	static void usage() 
	{
		System.out.println("Usage: java -cp . client -s <server> -p <port>");
	}
	
	/**
	 * @brief Parses program execution arguments 
	 */ 
	static boolean parseArguments(String [] argv) 
	{
		Getopt g = new Getopt("client", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				//case 'd':
				//	_debug = true;
				//	break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Integer.parseInt(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}
		
		if (_server == null)
			return false;
		
		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}
	
	
	
	/********************* MAIN **********************/
	
	public static void main(String[] argv) 
	{
		if(!parseArguments(argv)) {
			usage();
			return;
		}

		char a = '0';
		System.out.println("a: " + a);
		byte b = (byte) a;
		System.out.println("b: " + b);
		char c = (char) b;
		boolean t = c == '\0';
		System.out.println("Equal? " + t);
		
		shell();
		System.out.println("+++ FINISHED +++");
	}
}

class ServerThread extends Thread{
	private ServerSocket sc;
	private volatile Thread blinker;

	/**
	 * @brief Starts the server thread and initializes the ServerSocket property
	 */ 
	public void start(ServerSocket sc){
		blinker = new Thread(this);
		blinker.start();
		this.sc = sc;
	}

	/**
	 * @brief Destroys the server thread 
	 */ 
	public void kill(){
		blinker = null;
		System.out.println("Server thread is dying... :(");
	}

	/**
	 * @brief Main execution code sequence of the server thread. Listens to incoming connections 
	 */ 
	public void run(){
		Thread thisThread = Thread.currentThread();
		Socket sd = null;

		while(blinker == thisThread){
			try{
				/* Waiting for connection */
				
				sd = this.sc.accept();

				DataInputStream msg_in = new DataInputStream(sd.getInputStream());

				String operation = new String();
				byte ch;
				do{
					ch = msg_in.readByte();
					if (ch != 0) operation = operation + ((char) ch);
					
				} while(ch != 0);

				System.out.println("[SERVER_THREAD] - RECEIVED COMMAND: " + operation);
				String id = new String();

				switch(operation){
					case "SEND_MESSAGE":
						String sender = new String();
						do{
							ch = msg_in.readByte();
							if (ch != 0) sender = sender + ((char) ch);
							
						} while(ch != 0);
						do{
							ch = msg_in.readByte();
							if (ch != 0) id = id + ((char) ch);
						} while(ch != 0);
						String msg = new String();
						do{
							ch = msg_in.readByte();
							if (ch != 0) msg = msg + ((char) ch);
							
						} while(ch != 0);
						System.out.println("MESSAGE " + id + " FROM " + sender + ":");
						System.out.println("\t" + msg);
						System.out.println("END");
						break;

					case "SEND_MESS_ACK":
						do{
							ch = msg_in.readByte();
							if (ch != 0) break;
							id = id + ((char) ch);
						} while(ch != 0);
						System.out.println("SEND MESSAGE " + id + " OK");
						break;
				}
				sd.close();
				
			}
			catch(Exception e){
				System.out.println("Exception: " + e);
				e.printStackTrace();
				this.kill();
			}
		}
		
	}
}
