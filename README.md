Name: Naveen Pabodha Fernando
Curtin ID: 20897525

# Group-Chat
This is a C based program build on the concepts of Socket Programming.

In which we can join many client to a server and can share messages between each other and form a group for chatting.

In this assignment, server provides a unique ID's to each client which get connected to it and work as a routing medium
between all of them for share conversations and monitoring them and execute user input commands.

## Implementation
1. Compile both of the file [makeserver] and [makeclient] using make -f <make_file_name>.
   * make -f makeserver
   * make -f makeclient
2. The most important condition to use socket programming is that all the devices are on same network.
3. Run the server file on the system which will be use as a server.
   * ./server.out <m> <max_wait_game>  ([max_wait_game]- Maximum time the server waits, [m]- Maximum number of MClients)
4. Now run the client program on the system which will be use as a client.
   * ./client <server's_IP_address> 52002
     * __Note:__ You can check ip using **ifconfig -a** in linux.
     * If you are using server and client on same system use *127.0.0.1* as your local IP.
     
## Screenshots
![1](https://80e03c43ac18.png)
![2](https://2bdaac036156.png)
