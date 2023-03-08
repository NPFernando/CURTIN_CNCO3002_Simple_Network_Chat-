+------------------------------------------------------------------------------------+
|	Student Name	: Naveen Pabodha Fernando					
|	Curtin ID	: 20897525							
+------------------------------------------------------------------------------------+
|	Simple Network Chat (SNC) Instructions to run server and client programs	
+------------------------------------------------------------------------------------+
|											
+---> How to run server-side								
|	|									
|	+--> Create the executable using the make functions				
|	|	|								
|	|	+--> command: make -f makeserver				
|	|									
|	Run the executable file 						
|		|								
|		+--> command: ./server_snc.out [m] [max_wait_game]		
|			|							
|			+--> [max_wait_game]	- Maximum time the server waits	
|			+--> [m]	- Maximum number of Clients													|	
+---> How to run the client side							
|	|									
|	+--> Create the executable using the make functions				
|	|	|								
|	|	+--> command: make -f makeclient					
|	|										
|	+--> Run the executable file 							
|		|									
|		+-->command: ./client_snc.out 127.0.0.1 52001				
|			|								
|			|								
|			+--> 127.0.0.1		- ip address of server			
|			+--> 52002		- port number	 			
|											
+------------------------------------------------------------------------------------+
