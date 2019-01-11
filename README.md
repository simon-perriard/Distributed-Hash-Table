# Distributed-Hash-Table
## An Amazon's Dynamo inspired DHT written in C
This program has been developed during the "Projet de programation système" 2018 at EPFL.
This is a simple distributed hashtable that uses UDP protocol

How to run it :

0. Compile the program : ```make -B```
1. Specify servers in servers.txt with the following syntax : IP port #of nodes (i.e. 127.0.0.1 1234 3)
2. Launch each server with ```./pps-launch-server``` and by specifying which server from servers.txt you want to launch (IP and port)
3. You can either :

    a. Put an element (key, value) in the table : ```./pps-client-put key value```
    
    b. Get a value from the table : ```./pps-client-get key```
    
    c. Concatenate values from multiple keys and store the result with a new key : ```./pps-client-cat k1 k2 k3 ... newKey```
    
    d. Get a substring of a value and store it with a new key : ```./pps-client-substr k1 fromIndex length k2```
    
    e. Find the index of a matching substring in another key-value pair : ```./pps-client-find k1 k2```, -1 means that the value of k2 does not contain the value of k1
    
    f. List all the nodes and know which one is up and which one is down : ```./pps-list-nodes```
    
    g. Dump the content of a given node : ```./pps-dump-node IP port```

    Note : if a collision occurs, the old value will be overwritten

Developed by Simon Perriard and Hédi Sassi
