# instructions to compile & run

## compile

`gcc -Wall client.c utils.c student_ll.c server_store_ll.c -o client`
`gcc -Wall server.c utils.c student_ll.c server_store_ll.c -o server`

## to run

to run server
`./server <port>`
eg:
`./server 9000`

to run client
`./client <server-ip> <server-port> <client-id>`
eg:
`./client 127.0.0.1 9000 6`
